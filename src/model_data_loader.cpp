#include "model_data_loader.hpp"
#include "stb_image.h"
#include "thread_pool.hpp"

#include <spdlog/spdlog.h>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stack>
#include <set>
#include <list>
#include <sstream>

namespace kanso {

	namespace {

		template<typename OutputIt>
		void collect_ai_meshes_data(aiNode* root_node, const aiScene* scene, OutputIt meshes) {
			if (root_node == nullptr || scene == nullptr) {
				return;
			}

			std::stack<aiNode*> node_stack;
			node_stack.push(root_node);

			while (!node_stack.empty()) {
				aiNode* current_node = node_stack.top();
				node_stack.pop();

				for (size_t i = 0; i < current_node->mNumMeshes; i++) {
					aiMesh* mesh =
					    scene->mMeshes[current_node->mMeshes[i]]; // NOLINT(*pointer-arithmetic)
					*meshes++ = mesh;
				}

				for (size_t i = 0; i < current_node->mNumChildren; i++) {
					node_stack.push(
					    current_node->mChildren[i]); // NOLINT(*pointer-arithmetic)
				}
			}
		}

		template<typename OutputIt>
		void parse_vertices(const aiMesh* ai_mesh, OutputIt out) {
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			std::for_each(ai_mesh->mVertices, ai_mesh->mVertices + ai_mesh->mNumVertices, [&](const auto& vertex) {
				const size_t i = &vertex - ai_mesh->mVertices;

				const glm::vec3 pos = { vertex.x, vertex.y, vertex.z };
				const glm::vec3 normals{ ai_mesh->HasNormals()
											 ? glm::vec3{ ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z }
											 : glm::vec3{ 0 } };
				const glm::vec2 tex_coords{ ai_mesh->mTextureCoords[0] != nullptr
												? glm::vec2{ ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y }
												: glm::vec2{ 0 } };

				*out++ = mesh_vertex{ pos, normals, tex_coords };
			});
			// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		}

		template<typename OutputIt>
		void parse_indices(const aiMesh* ai_mesh, OutputIt out) {
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			std::for_each(ai_mesh->mFaces, ai_mesh->mFaces + ai_mesh->mNumFaces, [&out](const auto& face) {
				auto begin = face.mIndices;
				auto end = face.mIndices + face.mNumIndices;
				std::for_each(begin, end, [&](const auto& value) {
					*out++ = value;
				});
			});
			// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		}

		template<typename OutputIt>
		void parse_textures(std::string_view dir, const aiMaterial* mat, aiTextureType ai_type,
		                                    std::string_view type_name, OutputIt out) {
			for (uint i = 0; i < mat->GetTextureCount(ai_type); i++) {
				aiString filename;
				mat->GetTexture(ai_type, i, &filename);
				std::ostringstream oss;
				oss << dir << '/' << filename.C_Str();
				auto path = oss.str();
				raw_tex tex;
				tex.bytes = stbi_load(path.c_str(), &tex.width, &tex.height, &tex.nr_channels, 0);
				if (tex.bytes == nullptr) {
					spdlog::warn("Null texture");
					continue;
				}
				tex.path = path;
				tex.type = type_name;

				try {
					*out++ = tex;
				} catch (std::invalid_argument& e) {
					spdlog::error(e.what());
					continue;
				}
			}
		}

		template<typename OutputIt>
		void create_raw_maps(std::string_view dir, const aiMaterial* mat, OutputIt out) {
			const static std::vector<std::pair<aiTextureType, std::string_view>> map_conf = {
				{ aiTextureType_DIFFUSE, "texture_diffuse" },
				{ aiTextureType_SPECULAR, "texture_specular" },
				{ aiTextureType_NORMALS, "texture_normal" },
				{ aiTextureType_HEIGHT, "texture_height" }
			};

			 // only needs for std::vector to prevent extra allocating
			if constexpr (std::is_same_v<OutputIt, std::vector<typename OutputIt::value_type>>) {
				out.reserve(map_conf.size());
			}

			for (const auto& pair : map_conf) {
				std::vector<raw_tex> raw_textures;
				raw_textures.reserve(mat->GetTextureCount(pair.first));
				parse_textures(dir, mat, pair.first, pair.second, std::back_inserter(raw_textures));
				if (!raw_textures.empty()) {
					*out++ = raw_textures[0];
				}
			}
		}

	} // anonymous namespace

	template<typename InputIt>
	model_data::model_data(std::string model_name, InputIt begin, InputIt end)
	    : model_name_(std::move(model_name))
	{
		meshes_.reserve(std::distance(begin, end));
		std::for_each(std::make_move_iterator(begin), std::make_move_iterator(end), [this](auto&& data) {
			// TODO:: test for models with several meshes
			for (int i = 0; i < 3; i++) {
				aabb_max_[i] = std::max(aabb_max_[i], data.aabb_max[i]);
				aabb_min_[i] = std::max(aabb_min_[i], data.aabb_min[i]);
			}
			meshes_.emplace_back(data);
		});
		/* spdlog::debug("{}: final aabb_max ({}, {}, {}), aabb_min({}, {}, {})", model_name_, aabb_max_.x, aabb_max_.y, aabb_max_.z, aabb_min_.x, aabb_min_.y, aabb_min_.z); */
	}

	model_data_loader::model_data_loader(std::vector<std::string>::iterator paths_begin, std::vector<std::string>::iterator paths_end) {
		load(paths_begin, paths_end);
	}

	template<typename InputIt>
	void model_data_loader::load(InputIt paths_begin, InputIt paths_end) {
		const std::unordered_set<std::string_view> unique_paths{ paths_begin, paths_end };

		{
			thread_pool pool{ std::thread::hardware_concurrency() };
			for (const auto& path : unique_paths) {
				pool.enqueue([this, path]() { load_raw_model(path); });
			}
		}

		for (auto&& kv : raw_models_data_) {
			/* std::for_each(kv.second.begin(), kv.second.end(), [kv](const auto& mesh_data) { */
			/* 	spdlog::debug("{}: aabb_max ({}, {}, {}), aabb_min({}, {}, {})", kv.first, mesh_data.aabb_max.x, mesh_data.aabb_max.y, mesh_data.aabb_max.z, mesh_data.aabb_min.x, mesh_data.aabb_min.y, mesh_data.aabb_min.z); */
			/* }); */
			auto data = std::make_unique<model_data>(kv.first, kv.second.begin(), kv.second.end());
			models_data_.emplace(kv.first, std::move(data));
		}
	}

	void model_data_loader::load_raw_model(std::string_view path) {
		spdlog::debug("Loading {} model", path);

		Assimp::Importer importer;
		const aiScene*   scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs |
		                                                             aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenBoundingBoxes);
		if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 ||
		    scene->mRootNode == nullptr) { // NOLINT(hicpp-signed-bitwise)
			spdlog::error("Failed to open file: {}", path);
			const std::lock_guard<std::mutex> lock(mut_);
			raw_models_data_.emplace_back(path, raw_model_data{});
			return;
		}

		std::list<aiMesh*> ai_meshes;
		collect_ai_meshes_data(scene->mRootNode, scene, std::back_inserter(ai_meshes));

		auto dir = path.substr(0, path.find_last_of('/'));

		raw_model_data meshes_data;
		for (const auto& ai_mesh : ai_meshes) {

			const auto aabb = ai_mesh->mAABB;
			const glm::vec3 aabb_max { aabb.mMax.x, aabb.mMax.y, aabb.mMax.z };
			const glm::vec3 aabb_min { aabb.mMin.x, aabb.mMin.y, aabb.mMin.z };

			std::vector<mesh_vertex> vertices;
			vertices.reserve(ai_mesh->mNumVertices);
			parse_vertices(ai_mesh, std::back_inserter(vertices));

			size_t total_indices = 0;
			for (size_t i = 0; i < ai_mesh->mNumFaces; ++i) {
				total_indices += ai_mesh->mFaces[i].mNumIndices;  // NOLINT(*pointer-arithmetic)
			}

			std::vector<int> indices(total_indices);
			parse_indices(ai_mesh, std::back_inserter(indices));

			if (ai_mesh->mMaterialIndex >= 0) {
				// if textures present
				const auto* mat = scene->mMaterials[ai_mesh->mMaterialIndex]; // NOLINT(*pointer-arithmetic)
				std::vector<raw_tex> raw_maps;
				create_raw_maps(dir, mat, std::back_inserter(raw_maps));
				if (vertices.empty() && indices.empty() && raw_maps.empty()) {
					spdlog::warn("Wrong path");
				}
				meshes_data.emplace_back(std::move(vertices), std::move(indices), std::move(raw_maps), aabb_min, aabb_max);

			} else {
				meshes_data.emplace_back(std::move(vertices), std::move(indices), std::vector<raw_tex>{}, aabb_min, aabb_max);
			}
		}

		std::pair<std::string_view, raw_model_data> pair{ path, std::move(meshes_data) };
		{
			const std::lock_guard<std::mutex> lock(mut_);
			raw_models_data_.emplace_back(std::move(pair));
		}
	}

} // namespace kanso
