#include "mesh.hpp"
#include "glad/glad.h"

namespace kanso {

	namespace {

		using mesh_internal::vertex_object;

		vertex_object create_vo(const std::vector<mesh_vertex>& vertices, const std::vector<int>& indices) {
			vertex_object vo{};

			// TODO: move to opengl proxy
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

	mesh::mesh(mesh_data data)
	    : vertices_(std::move(data.vertices)),
	      indices_(std::move(data.indices)),
	      texture_(data.raw_maps),
	      vo_(create_vo(vertices_, indices_)) {}

	mesh::~mesh() {
		// TODO: move to opengl proxy
		glDeleteVertexArrays(1, &vo_.vao);
		glDeleteBuffers(1, &vo_.vbo);
		glDeleteBuffers(1, &vo_.ebo);
	}

	void mesh::draw(uint shader) {

		texture_.bind(shader);

		// TODO: move to opengl proxy
		glBindVertexArray(vo_.vao);
		glDrawElements(GL_TRIANGLES, static_cast<int>(indices_.size()), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
} // namespace kanso
