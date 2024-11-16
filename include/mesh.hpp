#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "texture.hpp"
#include "renderer.hpp"

namespace kanso {

	struct mesh_data {
		mesh_data(std::vector<mesh_vertex> vertices, std::vector<int> indices, std::vector<raw_tex> maps, const glm::vec3& aabb_min, const glm::vec3& aabb_max)
			: vertices(std::move(vertices)),
			  indices(std::move(indices)),
			  raw_maps(std::move(maps)),
			  aabb_min(aabb_min),
			  aabb_max(aabb_max){}

		std::vector<mesh_vertex> vertices;
		std::vector<int>         indices;
		std::vector<raw_tex>     raw_maps;
		glm::vec3 aabb_min;
		glm::vec3 aabb_max;
	};

	class mesh {
		public:
			mesh(mesh_data data);

			void draw(uint shader);

		private:
			std::vector<mesh_vertex> vertices_;
			std::vector<int>         indices_;
			texture texture_;
			std::unique_ptr<renderer> renderer_;
	};

} // namespace kanso
