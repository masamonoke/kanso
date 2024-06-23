#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>

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

namespace {

	std::vector<texture_t> load_material_textures(loaded_model_t* model, aiMaterial* mat, aiTextureType ai_type, const std::string& type_name,
		std::vector<texture_t>& loaded_textures)
	{
		std::vector<texture_t> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(ai_type); i++) {
			aiString filename;
			mat->GetTexture(ai_type, i, &filename);


			auto skip = false;
			for (const auto& tex : loaded_textures) {
				if (0 == std::strcmp(tex.path, filename.C_Str())) {
					textures.push_back(tex);
					skip = true;
					break;
				}
			}

			if (!skip) {
				auto str = std::string(model->model_data.directory) + '/' + std::string(filename.C_Str());
				texture_t t;
				auto tex_id = texture_create_gl_texture(str.c_str());
				if (tex_id >= 0) {
					t.id = static_cast<uint32_t>(tex_id);
				} else {
					log_error("Failed to create texture");
					continue;
				}
				t.type = static_cast<char*>(malloc((sizeof(char) * type_name.length()) + 1));
				std::strcpy(t.type, type_name.c_str());
				t.path = static_cast<char*>(malloc(sizeof(char) * strlen(str.c_str()) + 1));
				std::strcpy(t.path, str.c_str());
				textures.push_back(t);
				loaded_textures.push_back(t);
			}
		}

		return textures;
	}

	mesh_t* process_mesh(loaded_model_t* model, aiMesh* mesh, const aiScene* scene) {
		std::vector<vertex_t> vertices;
		std::vector<int32_t> indices;
		std::vector<texture_t> textures;
		std::vector<texture_t> loaded_textures;

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
			auto diffuse_map = load_material_textures(model, material, aiTextureType_DIFFUSE, "texture_diffuse", loaded_textures);
			textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());
			auto specular_maps = load_material_textures(model, material, aiTextureType_SPECULAR, "texture_specular", loaded_textures);
			textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
			auto normal_maps = load_material_textures(model, material, aiTextureType_NORMALS, "texture_normal", loaded_textures);
			textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
			auto height_maps = load_material_textures(model, material, aiTextureType_HEIGHT, "texture_height", loaded_textures);
			textures.insert(textures.end(), height_maps.begin(), height_maps.end());
		}

		vertex_vector_t vertices_v;
		vertices_v.vertices = static_cast<vertex_t*>(malloc(sizeof(vertex_t) * vertices.size()));
		memcpy(vertices_v.vertices, vertices.data(), sizeof(vertex_t) * vertices.size());
		vertices_v.size = vertices.size();

		int32_vector_t indices_v;
		indices_v.array= static_cast<int32_t*>(malloc(sizeof(int32_t) * indices.size()));
		memcpy(indices_v.array, indices.data(), sizeof(int32_t) * indices.size());
		indices_v.size = indices.size();

		texture_vector_t textures_v;
		textures_v.textures = static_cast<texture_t*>(malloc(sizeof(texture_t) * textures.size()));
		memcpy(textures_v.textures, textures.data(), sizeof(texture_t) * textures.size());
		textures_v.size = textures.size();

		mesh_t* m;
		mesh_new(&m, vertices_v, indices_v, textures_v);
		return m;
	}

	void collect_meshes(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& meshes) { // NOLINT
		for (size_t i = 0; i < node->mNumMeshes; i++) {
			auto* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(mesh);
		}

		for (size_t i = 0; i < node->mNumChildren; i++) {
			collect_meshes(node->mChildren[i], scene, meshes);
		}
	}

	std::vector<mesh_t*> process_meshes(loaded_model_t* model, const aiScene* scene, const std::vector<aiMesh*>& ai_meshes, std::vector<mesh_t*>& meshes) {
		for (size_t i = 0; i < ai_meshes.size(); i++) {
			auto m = process_mesh(model, ai_meshes[i], scene);
			if (m) {
				meshes[i] = m;
			}
		}

		return meshes;
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

	auto path_str = std::string(path);
	path_str = path_str.substr(0, path_str.find_last_of('/')).c_str();
	char* tmp = static_cast<char*>(malloc((sizeof(char) * path_str.length()) + 1));
	std::strcpy(tmp, path_str.c_str());
	model->model_data.directory = tmp;

	std::vector<aiMesh*> ai_meshes;
	collect_meshes(scene->mRootNode, scene, ai_meshes);

	std::vector<mesh_t*> meshes;
	meshes.resize(ai_meshes.size());
	process_meshes(model, scene, ai_meshes, meshes);

	model->model_data.meshes = static_cast<mesh_t**>(malloc(sizeof(mesh_t*) * meshes.size()));
	memcpy(model->model_data.meshes, meshes.data(), sizeof(mesh_t*) * meshes.size());
	model->model_data.meshes_count = meshes.size();

	return true;
}
