#pragma once

#include "core.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <cstddef>
#include <vector>

namespace kanso {

	struct mesh_vertex {
		glm::vec3 pos{};
		glm::vec3 normal{};
		glm::vec2 tex_coords{};
	};

	class renderer {
		public:
			virtual void draw_triangles() = 0;
			virtual void draw_line()      = 0;

			virtual uint load_texture(uint8_t* bytes, int nr_channels, int width, int height) = 0;
			virtual void bind_texture(uint shader, const std::string& type, uint id, uint& diffuse_nr,
			                          uint& specular_nr, uint& height_nr, uint& normal_nr, uint& number,
			                          uint index)                                             = 0;
			virtual void reset_stencil_test()                                                 = 0;
			virtual void enable_stencil_test()                                                = 0;
			virtual void clear(float red, float green, float blue, float alpha = 1.0f)        = 0;
			virtual void set_viewport(int width, int height)                                  = 0;
			virtual void enable_depth()                                                       = 0;
			virtual void enable_stencil(uint fill)                                            = 0;

			virtual ~renderer() = default;
	};

	class opengl_renderer : public renderer {
		public:
			opengl_renderer() = default;
			opengl_renderer(const glm::vec3& start, const glm::vec3& end);
			opengl_renderer(const std::vector<mesh_vertex>& vertices, const std::vector<int>& indices);
			~opengl_renderer() override;

			void draw_triangles() override;
			void draw_line() override;

			uint load_texture(uint8_t* bytes, int nr_channels, int width, int height) override;
			void bind_texture(uint shader, const std::string& type, uint id, uint& diffuse_nr, uint& specular_nr,
			                  uint& height_nr, uint& normal_nr, uint& number, uint index) override;
			void reset_stencil_test() override;
			void enable_stencil_test() override;
			void clear(float red, float green, float blue, float alpha) override;
			void set_viewport(int width, int height) override;
			void enable_depth() override;
			void enable_stencil(uint fill) override;

		private:
			uint vao_{};
			uint vbo_{};
			uint ebo_{};
			int  indices_count_{};
	};

	struct renderer_factory {
		template <typename Renderer = opengl_renderer, typename... Args>
		static std::unique_ptr<renderer> make_renderer(Args&&... args) {
			return std::make_unique<Renderer>(std::forward<Args>(args)...);
		}
	};

} // namespace kanso
