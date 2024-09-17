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

namespace kanso {

	namespace {

		void collect_ai_meshes_data(aiNode* root_node, const aiScene* scene, std::vector<aiMesh*>& meshes) {
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
					meshes.emplace_back(mesh);
				}

				for (size_t i = 0; i < current_node->mNumChildren; i++) {
					node_stack.push(
					    current_node->mChildren[i]); // NOLINT(*pointer-arithmetic)
				}
			}
		}

		std::vector<mesh_vertex> parse_vertices(const aiMesh* ai_mesh) {
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			std::vector<mesh_vertex> vertices;

			for (size_t i = 0; i < ai_mesh->mNumVertices; i++) {
				const glm::vec3 pos = { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z };
				const glm::vec3 normals{ ai_mesh->HasNormals()
					                         ? glm::vec3{ ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y,
					                                      ai_mesh->mNormals[i].z }
					                         : glm::vec3{ 0 } };
				const glm::vec2 tex_coords{ ai_mesh->mTextureCoords[0] != nullptr
					                            ? glm::vec2{ ai_mesh->mTextureCoords[0][i].x,
					                                         ai_mesh->mTextureCoords[0][i].y }
					                            : glm::vec2{ 0 } };
				vertices.emplace_back(mesh_vertex{ pos, normals, tex_coords });
			}
			// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

			return vertices;
		}

		std::vector<int> parse_indices(const aiMesh* ai_mesh) {
			std::vector<int> indices;
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			for (size_t i = 0; i < ai_mesh->mNumFaces; i++) {
				auto face = ai_mesh->mFaces[i];
				for (size_t j = 0; j < face.mNumIndices; j++) {
					indices.push_back(static_cast<int>(face.mIndices[j]));
				}
			}
			// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

			return indices;
		}

		std::vector<raw_tex> parse_textures(const std::string& dir, const aiMaterial* mat, aiTextureType ai_type,
		                                    const std::string& type_name) {
			std::vector<raw_tex> raw_textures;
			for (uint i = 0; i < mat->GetTextureCount(ai_type); i++) {
				aiString filename;
				mat->GetTexture(ai_type, i, &filename);
				auto    path = dir + "/" + std::string(filename.C_Str());
				raw_tex tex;
				tex.bytes = stbi_load(path.c_str(), &tex.width, &tex.height, &tex.nr_channels, 0);
				if (tex.bytes == nullptr) {
					spdlog::warn("Null texture");
					continue;
				}
				tex.path = path;
				tex.type = type_name;

				try {
					raw_textures.emplace_back(tex);
				} catch (std::invalid_argument& e) {
					spdlog::error(e.what());
					continue;
				}
			}

			return raw_textures;
		}

		std::vector<raw_tex> create_raw_maps(const std::string& dir, const aiMaterial* mat) {
			const static std::vector<std::pair<aiTextureType, std::string>> map_conf = {
				{ aiTextureType_DIFFUSE, "texture_diffuse" },
				{ aiTextureType_SPECULAR, "texture_specular" },
				{ aiTextureType_NORMALS, "texture_normal" },
				{ aiTextureType_HEIGHT, "texture_height" }
			};

			std::vector<raw_tex> raw_maps;
			for (const auto& pair : map_conf) {
				auto raw_textures = parse_textures(dir, mat, pair.first, pair.second);
				if (!raw_textures.empty()) {
					raw_maps.emplace_back(raw_textures[0]);
				}
			}

			return raw_maps;
		}

	} // anonymous namespace

	model_data::model_data(std::string model_name, std::vector<mesh_data> meshes_data)
	    : model_name_(std::move(model_name)) {
		for (auto&& data : meshes_data) {
			meshes_.emplace_back(std::move(data));
		}
	}

	model_data_loader::model_data_loader(const std::vector<std::string>& paths) {
		load(paths);
	}

	void model_data_loader::load(const std::vector<std::string>& paths) {
		const std::set<std::string> unique_paths{ paths.begin(), paths.end() };

		{
			thread_pool pool{ 4 };
			for (const auto& path : unique_paths) {
				pool.enqueue([this, path]() { load_raw_model(path); });
			}
		}

		for (auto&& kv : raw_models_data_) {
			auto data = std::make_unique<model_data>(kv.first, std::move(kv.second));
			models_data_.emplace(kv.first, std::move(data));
		}
	}

	void model_data_loader::load_raw_model(const std::string& path) {
		spdlog::debug("Loading {} model", path);

		Assimp::Importer importer;
		const aiScene*   scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs |
		                                                             aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
		if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 ||
		    scene->mRootNode == nullptr) { // NOLINT(hicpp-signed-bitwise)
			spdlog::error("Failed to open file: {}", path);
			const std::lock_guard<std::mutex> lock(mut_);
			raw_models_data_.emplace_back(path, raw_model_data{});
			return;
		}

		std::vector<aiMesh*> ai_meshes;
		collect_ai_meshes_data(scene->mRootNode, scene, ai_meshes);

		auto dir = path.substr(0, path.find_last_of('/'));

		raw_model_data meshes_data;
		for (const auto& ai_mesh : ai_meshes) {
			auto v = parse_vertices(ai_mesh);
			auto i = parse_indices(ai_mesh);
			if (ai_mesh->mMaterialIndex >= 0) {
				const auto* mat = scene->mMaterials[ai_mesh->mMaterialIndex]; // NOLINT(*pointer-arithmetic)
				auto        t   = create_raw_maps(dir, mat);
				if (v.empty() && i.empty() && t.empty()) {
					spdlog::warn("Wrong path");
				}
				meshes_data.emplace_back(std::move(v), std::move(i), std::move(t));
			} else {
				meshes_data.emplace_back(std::move(v), std::move(i), std::vector<raw_tex>{});
			}
		}

		std::pair<std::string, raw_model_data> pair{ path, std::move(meshes_data) };
		{
			const std::lock_guard<std::mutex> lock(mut_);
			raw_models_data_.emplace_back(std::move(pair));
		}
	}

} // namespace kanso
