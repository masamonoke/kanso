#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <thread>
#include <map>
#include <stack>
#include <mutex>
#include <utility>
#include <queue>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <assimp/material.h>
#include <assimp/material.inl>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#ifdef __cplusplus
extern "C" {
#include <c_log.h>

#include "model_loader.h"
#include "texture.h"
#include "mesh.h"
#ifdef __cplusplus
}
#endif
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace {

	// https://github.com/progschj/ThreadPool
	// modified to work with C++20
	class ThreadPool {
		public:
			ThreadPool(size_t);
			template<class F, class... Args>
			auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;
			~ThreadPool();
		private:
			std::vector<std::thread> workers;
			std::queue<std::function<void()>> tasks;
			std::mutex queue_mutex;
			std::condition_variable condition;
			bool stop;
	};

	inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
		for(size_t i = 0;i<threads;++i) {
			workers.emplace_back(
				[this] {
					for(;;) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
							if(this->stop && this->tasks.empty()) {
								return;
							}
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}

						task();
					}
				}
			);
		}
	}

	template<class F, class... Args>
	auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
		using return_type = typename std::invoke_result<F, Args...>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		{
			const std::unique_lock<std::mutex> lock(queue_mutex);

			if(stop) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}

			tasks.emplace([task](){ (*task)(); });
		}
		condition.notify_one();
		return res;
	}

	inline ThreadPool::~ThreadPool() {
		{
			const std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for(std::thread &worker: workers) {
			worker.join();
		}
	}

	struct tex_data {
		uint8_t* bytes;
		int32_t width;
		int32_t height;
		int32_t nr_channels;
		std::string path;
		std::string type;
	};

	std::mutex mt;

	std::vector<tex_data> load_material_textures(const std::string& directory, aiMaterial* mat, aiTextureType ai_type, const std::string& type_name,
		std::map<std::string, tex_data*>& textures_map)
	{
		std::vector<tex_data> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(ai_type); i++) {
			aiString filename;
			mat->GetTexture(ai_type, i, &filename);

			bool skip;
			{
				const std::lock_guard<std::mutex> lock(mt);
				skip = textures_map.contains(std::string(filename.C_Str()));
			}

			if (!skip) {
				auto str = directory + '/' + std::string(filename.C_Str());
				tex_data tex;
				tex.bytes = stbi_load(str.c_str(), &tex.width, &tex.height, &tex.nr_channels, 0);
				tex.path = str;
				tex.type = type_name;
				textures.push_back(std::move(tex));
				const std::lock_guard<std::mutex> lock(mt);
				textures_map[textures.back().path] = &textures.back();
			}
		}

		return textures;
	}

	void process_mesh_data(const std::string& directory, const aiMesh* mesh, const aiScene* scene, std::vector<vertex_t>& vertices, std::vector<int32_t>& indices,
			std::vector<tex_data>& textures, std::map<std::string, tex_data*>& textures_map)
	{
		for (size_t i = 0; i < mesh->mNumVertices; i++) {
			vertex_t vertex;

			vertex.pos[0] = mesh->mVertices[i].x;
			vertex.pos[1] = mesh->mVertices[i].y;
			vertex.pos[2] = mesh->mVertices[i].z;

			if (mesh->HasNormals()) {
				vertex.normal[0] = mesh->mNormals[i].x;
				vertex.normal[1] = mesh->mNormals[i].y;
				vertex.normal[2] = mesh->mNormals[i].z;
			}

			if (mesh->mTextureCoords[0]) {
				vertex.tex_coords[0] = mesh->mTextureCoords[0][i].x;
				vertex.tex_coords[1] = mesh->mTextureCoords[0][i].y;
			} else {
				vertex.tex_coords[0] = 0.0f;
				vertex.tex_coords[1] = 0.0f;
			}

			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++) {
			auto face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++) {
				indices.push_back(static_cast<int32_t>(face.mIndices[j]));
			}
		}

		if (mesh->mMaterialIndex >= 0) {
			auto material = scene->mMaterials[mesh->mMaterialIndex];

			auto diffuse_maps = load_material_textures(directory, material, aiTextureType_DIFFUSE, "texture_diffuse", textures_map);
			textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

			auto specular_maps = load_material_textures(directory, material, aiTextureType_SPECULAR, "texture_specular", textures_map);
			textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

			auto normal_maps = load_material_textures(directory, material, aiTextureType_NORMALS, "texture_normal", textures_map);
			textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

			auto height_maps = load_material_textures(directory, material, aiTextureType_HEIGHT, "texture_height", textures_map);
			textures.insert(textures.end(), height_maps.begin(), height_maps.end());
		}
	}

	void collect_ai_meshes_data(aiNode* root_node, const aiScene* scene, std::vector<aiMesh*>& meshes) {
		if (!root_node || !scene) {
			return;
		}

		std::stack<aiNode*> node_stack;
		node_stack.push(root_node);

		while (!node_stack.empty()) {
			aiNode* current_node = node_stack.top();
			node_stack.pop();

			for (size_t i = 0; i < current_node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[current_node->mMeshes[i]];
				meshes.push_back(mesh);
			}

			for (size_t i = 0; i < current_node->mNumChildren; i++) {
				node_stack.push(current_node->mChildren[i]);
			}
		}
	}

	void create_meshes(std::vector<std::vector<vertex_t>> meshes_vertices, std::vector<std::vector<int32_t>> meshes_indices, std::vector<std::vector<tex_data>> meshes_textures_data,
		std::vector<mesh_t*>& meshes)
	{
		for (size_t i = 0; i < meshes.size(); i++) {
			const std::vector<vertex_t> vertices = meshes_vertices[i];
			const std::vector<int32_t> indices = meshes_indices[i];
			const std::vector<tex_data> textures_data = meshes_textures_data[i];

			vertex_vector_t vertices_v;
			vertices_v.vertices = static_cast<vertex_t*>(malloc(sizeof(vertex_t) * vertices.size()));
			memcpy(vertices_v.vertices, vertices.data(), sizeof(vertex_t) * vertices.size());
			vertices_v.size = vertices.size();

			int32_vector_t indices_v;
			indices_v.array= static_cast<int32_t*>(malloc(sizeof(int32_t) * indices.size()));
			memcpy(indices_v.array, indices.data(), sizeof(int32_t) * indices.size());
			indices_v.size = indices.size();

			std::vector<texture_t> textures;
			for (const auto& tex : textures_data) {
				texture_t t;
				if (tex.bytes != nullptr) {
					auto tex_id = texture_create(tex.bytes, tex.width, tex.height, tex.nr_channels);
					if (tex_id >= 0) {
						t.id = static_cast<uint32_t>(tex_id);
					} else {
						log_error("Failed to create texture");
						continue;
					}
					stbi_image_free(tex.bytes);
					t.type = static_cast<char*>(malloc((sizeof(char) * tex.type.length()) + 1));
					std::strcpy(t.type, tex.type.c_str());
					t.path = static_cast<char*>(malloc(sizeof(char) * tex.path.length() + 1));
					std::strcpy(t.path, tex.path.c_str());
					textures.push_back(t);
				} else {
					log_error("Null texture %s", tex.path.c_str());
				}
			}
			texture_vector_t textures_v;
			textures_v.textures = static_cast<texture_t*>(malloc(sizeof(texture_t) * textures.size()));
			memcpy(textures_v.textures, textures.data(), sizeof(texture_t) * textures.size());
			textures_v.size = textures.size();

			mesh_t* m;
			mesh_new(&m, vertices_v, indices_v, textures_v);

			if (m) {
				meshes[i] = m;
			}
		}
	}

	void collect_meshes_data(loaded_model_t* model, const aiScene* scene, const std::vector<aiMesh*>& ai_meshes,
		std::vector<std::vector<vertex_t>>& meshes_vertices, std::vector<std::vector<int32_t>>& meshes_indices, std::vector<std::vector<tex_data>>& meshes_textures_data)
	{
		std::map<std::string, tex_data*> textures_map;
		const std::string directory { model->model_data.directory };
		const size_t POOL_THREADS_NUM = 100;
		ThreadPool pool { POOL_THREADS_NUM };

		for (size_t i = 0; i < ai_meshes.size(); i++) {
			pool.enqueue([directory, scene, ai_meshes, &meshes_vertices, &meshes_indices, &meshes_textures_data, &textures_map, i]() {
				std::vector<vertex_t> vertices;
				std::vector<int32_t> indices;
				std::vector<tex_data> textures;

				process_mesh_data(directory, ai_meshes[i], scene, vertices, indices, textures, textures_map);

				const std::lock_guard<std::mutex> lock(mt);
				meshes_vertices.push_back(std::move(vertices));
				meshes_indices.push_back(std::move(indices));
				meshes_textures_data.push_back(std::move(textures));
			});
		}
	}

	void process_meshes(loaded_model_t* model, const aiScene* scene) {
		std::vector<aiMesh*> ai_meshes;
		collect_ai_meshes_data(scene->mRootNode, scene, ai_meshes);

		std::vector<std::vector<vertex_t>> meshes_vertices;
		std::vector<std::vector<int32_t>> meshes_indices;
		std::vector<std::vector<tex_data>> meshes_textures_data;
		collect_meshes_data(model, scene, ai_meshes, meshes_vertices, meshes_indices, meshes_textures_data);

		std::vector<mesh_t*> meshes;
		meshes.resize(ai_meshes.size());
		create_meshes(meshes_vertices, meshes_indices, meshes_textures_data, meshes);

		model->model_data.meshes = static_cast<mesh_t**>(malloc(sizeof(mesh_t*) * meshes.size()));
		memcpy(model->model_data.meshes, meshes.data(), sizeof(mesh_t*) * meshes.size());
		model->model_data.meshes_count = meshes.size();
	}

	void set_path(const char* path, loaded_model_t* model) {
		auto path_str = std::string(path);
		path_str = path_str.substr(0, path_str.find_last_of('/')).c_str();
		char* tmp = static_cast<char*>(malloc((sizeof(char) * path_str.length()) + 1));
		std::strcpy(tmp, path_str.c_str());
		model->model_data.directory = tmp;
	}
}

bool model_loader_load_model(loaded_model_t* model, const char* path) {
	assert(model != NULL);
	assert(path != NULL);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		log_error("Error while loading model %s: %s", path, importer.GetErrorString());
		return false;
	}

	set_path(path, model);

	process_meshes(model, scene);

	return true;
}
