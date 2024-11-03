#include "renderer.hpp"
#include "glad/glad.h"
#include "shader.hpp"

#include <spdlog/spdlog.h>

#include <string>

namespace kanso {

	namespace {
		uint gen_vao() {
			uint vao{};
			glGenVertexArrays(1, &vao);
			return vao;
		}

		uint gen_buf() {
			uint buf{};
			glGenBuffers(1, &buf);
			return buf;
		}
	} // namespace

	opengl_renderer::opengl_renderer(const glm::vec3& start, const glm::vec3& end) : vao_(gen_vao()), vbo_(gen_buf()) {
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		const std::array<glm::vec3, 2> points{ start, end };
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
	}

	opengl_renderer::opengl_renderer(const std::vector<mesh_vertex>& vertices, const std::vector<int>& indices)
	    : vao_(gen_vao()),
	      vbo_(gen_buf()),
	      ebo_(gen_buf()),
	      indices_count_(static_cast<int>(indices.size())) {
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices.size() * sizeof(mesh_vertex)), vertices.data(),
		             GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
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
	}

	opengl_renderer::~opengl_renderer() {
		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(1, &vbo_);
		glDeleteBuffers(1, &ebo_);
	}

	void opengl_renderer::draw_triangles() {
		glBindVertexArray(vao_);
		glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	void opengl_renderer::draw_line() {
		glEnableVertexAttribArray(0);
		glBindVertexArray(vao_);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	uint opengl_renderer::load_texture(uint8_t* bytes, int nr_channels, int width, int height) {
		if (bytes == nullptr) {
			throw std::invalid_argument("Texture binary data is nullptr");
		}

		GLenum format{};
		switch (nr_channels) {
			case 1:
				format = GL_RED;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			default:
				throw std::invalid_argument("Wrong texture format");
		}

		uint texture_id{};
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
		glGenerateMipmap(GL_TEXTURE_2D);

		return texture_id;
	}

	void opengl_renderer::bind_texture(uint shader, const std::string& type, uint id, uint& diffuse_nr, uint& specular_nr,
	                            uint& height_nr, uint& normal_nr, uint& number, uint index) {
		glActiveTexture(GL_TEXTURE0 + index);
		auto name = type;
		if (name == "texture_diffuse") {
			number = diffuse_nr++;
		} else if (name == "texture_specular") {
			number = specular_nr++;
		} else if (name == "texture_height") {
			number = height_nr++;
		} else if (name == "texture_normal") {
			number = normal_nr++;
		} else {
			spdlog::error("Unknown texture type {}", name);
		}

		name += std::to_string(number);
		shader::set_uniform(shader, name, static_cast<int>(index));
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void opengl_renderer::reset_stencil_test() {
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilMask(0xff);
		glEnable(GL_DEPTH_TEST);
	}

	void opengl_renderer::enable_stencil_test() {
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
	}

	void opengl_renderer::clear(float red, float green, float blue, float alpha) {
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void opengl_renderer::enable_depth() {
		glEnable(GL_DEPTH_TEST);
	}

	void opengl_renderer::enable_stencil(uint fill) {
		glDepthFunc(GL_LESS);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, fill);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void opengl_renderer::set_viewport(int width, int height) {
		glViewport(0, 0, width, height);
	}

} // namespace kanso
