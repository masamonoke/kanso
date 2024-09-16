#include "mesh.hpp"
#include "glad/glad.h"
#include "shader.hpp"

#include <spdlog/spdlog.h>

namespace kanso {

	namespace {

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

		internal::vertex_object create_vo(const std::vector<mesh_vertex>& vertices, const std::vector<int>& indices) {
			internal::vertex_object vo{};

			glGenVertexArrays(1, &vo.vao);
			glGenBuffers(1, &vo.vbo);
			glGenBuffers(1, &vo.ebo);

			glBindVertexArray(vo.vao);
			glBindBuffer(GL_ARRAY_BUFFER, vo.vbo);

			glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices.size() * sizeof(mesh_vertex)), vertices.data(),
			             GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vo.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(indices.size() * sizeof(int)), indices.data(),
			             GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), static_cast<void*>(nullptr));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex),
			                      (void*)offsetof(mesh_vertex, normal)); // NOLINT
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex),
			                      (void*)offsetof(mesh_vertex, tex_coords)); // NOLINT

			glBindVertexArray(0);

			return vo;
		}
	} // namespace

	mesh::mesh(const std::string& dir, const aiMesh* ai_mesh, const aiScene* scene)
	    : vertices_(parse_vertices(ai_mesh)),
	      indices_(parse_indices(ai_mesh)),
	      vo_(create_vo(vertices_, indices_)) {
		if (ai_mesh->mMaterialIndex >= 0) {
			auto* material =
			    scene->mMaterials[ai_mesh->mMaterialIndex]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			textures_.emplace_back(dir, material);
		}
	}

	mesh::~mesh() {
		glDeleteVertexArrays(1, &vo_.vao);
		glDeleteBuffers(1, &vo_.vbo);
		glDeleteBuffers(1, &vo_.ebo);
	}

	void mesh::draw(uint shader) {

		for (const auto& tex : textures_) {
			tex.bind(shader);
		}

		glBindVertexArray(vo_.vao);
		glDrawElements(GL_TRIANGLES, static_cast<int>(indices_.size()), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
} // namespace kanso
