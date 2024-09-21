#pragma once

#include "core.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <cstddef>
#include <vector>

namespace kanso {

	struct vertex_object {
		uint vao;
		uint vbo;
		uint ebo;
	};

	struct mesh_vertex {
		glm::vec3 pos{};
		glm::vec3 normal{};
		glm::vec2 tex_coords{};
	};

	class renderer {
		public:
			renderer(const std::vector<mesh_vertex>& vertices, const std::vector<int>& indices);
			~renderer();

			void draw_triangles() const;

			static uint load_texture(uint8_t* bytes, int nr_channels, int width, int height);
			static void bind_texture(uint shader, const std::string& type, uint id, uint& diffuse_nr, uint& specular_nr,
			                         uint& height_nr, uint& normal_nr, uint& number, uint index);
			static void reset_stencil_test();
			static void enable_stencil_test();
			static void clear(float red, float green, float blue, float alpha = 1.0f);
			static void set_viewport(int width, int height);
			static void enable_depth();
			static void enable_stencil(uint fill);

		private:
			vertex_object vo_;
			int           indices_count_;
	};

} // namespace kanso
