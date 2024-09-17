#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "texture.hpp"

namespace kanso {

	namespace mesh_internal {
		struct vertex_object {
			uint vao;
			uint vbo;
			uint ebo;
		};
	} // namespace mesh_internal

	struct mesh_vertex {
		glm::vec3 pos{};
		glm::vec3 normal{};
		glm::vec2 tex_coords{};
	};

	struct mesh_data {
		mesh_data(std::vector<mesh_vertex> vertices, std::vector<int> indices, std::vector<raw_tex> maps)
			: vertices(std::move(vertices)),
			  indices(std::move(indices)),
			  raw_maps(std::move(maps)) {}
		std::vector<mesh_vertex> vertices;
		std::vector<int>         indices;
		std::vector<raw_tex>     raw_maps;
	};

	class mesh {
		public:
			mesh(mesh_data data);
			~mesh();

			void draw(uint shader);

		private:
			std::vector<mesh_vertex>     vertices_;
			std::vector<int>             indices_;
			texture                      texture_;
			mesh_internal::vertex_object vo_{};
	};

} // namespace kanso
