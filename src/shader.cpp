#include "shader.hpp"
#include "glad/glad.h"

#include <spdlog/spdlog.h>

#include <string>
#include <fstream>
#include <sstream>

namespace kanso {

	namespace {
		std::pair<std::string, std::string> load_shaders(const std::string& vert_file, const std::string& frag_file) {
			spdlog::debug("Loading shaders: {}, {}", vert_file, frag_file);
			std::ifstream vert_fs;
			std::ifstream frag_fs;
			std::string   vert_code;
			std::string   frag_code;

			vert_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			frag_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				vert_fs.open(vert_file);
				frag_fs.open(frag_file);

				std::stringstream vert_stream;
				std::stringstream frag_stream;

				vert_stream << vert_fs.rdbuf();
				frag_stream << frag_fs.rdbuf();

				vert_fs.close();
				frag_fs.close();

				vert_code = vert_stream.str();
				frag_code = frag_stream.str();
			} catch (std::ifstream::failure& e) {
				spdlog::error("Failed to read shader: {}, {}", vert_file, frag_file);
				throw exception::shader_load_exception(e.what());
			}

			return { vert_code, frag_code };
		}

		uint compile_shader(const char* code, GLenum type) {
			const uint vert = glCreateShader(type);
			int        res{};
			glShaderSource(vert, 1, &code, nullptr);
			glCompileShader(vert);
			glGetShaderiv(vert, GL_COMPILE_STATUS, &res);
			if (res == GL_FALSE) {
				std::array<char, 512> info{};
				glGetShaderInfoLog(vert, sizeof(info), nullptr, info.data());
				spdlog::error("{}", info.data());
				throw exception::shader_compile_exception(fmt::format("Failed to compile vertex shader"));
			}

			return vert;
		}

		uint create_shader(const std::string& vert_file, const std::string& frag_file) {
			auto code_pair = load_shaders(vert_file, frag_file);

			const char* vert_str = code_pair.first.c_str();
			const char* frag_str = code_pair.second.c_str();
			int         res{};
			uint        vert{};
			uint        frag{};

			vert = compile_shader(vert_str, GL_VERTEX_SHADER);
			frag = compile_shader(frag_str, GL_FRAGMENT_SHADER);

			const uint id = glCreateProgram();
			glAttachShader(id, vert);
			glAttachShader(id, frag);
			glLinkProgram(id);
			glGetProgramiv(id, GL_LINK_STATUS, &res);
			if (res == GL_FALSE) {
				std::array<char, 512> info{};
				glGetProgramInfoLog(id, sizeof(info), nullptr, info.data());
				throw exception::shader_linkage_exception(
				    fmt::format("Failed to link shaders: {}, {}", vert_file, frag_file));
			}

			glDeleteShader(vert);
			glDeleteShader(frag);

			return id;
		}
	} // namespace

	shader::shader(const std::string& vert_file, const std::string& frag_file)
	    : id_(create_shader(vert_file, frag_file)) {}

	void shader::set_uniform(uint shader, const std::string& name, const glm::vec3& vector) {
		glUniform3fv(glGetUniformLocation(shader, name.c_str()), 1, &vector[0]);
	}

	void shader::set_uniform(uint shader, const std::string& name, bool val) {
		glUniform1i(glGetUniformLocation(shader, name.c_str()), static_cast<int>(val));
	}

	void shader::set_uniform(uint shader, const std::string& name, int val) {
		glUniform1i(glGetUniformLocation(shader, name.c_str()), val);
	}

	void shader::set_uniform(uint shader, const std::string& name, float val) {
		glUniform1f(glGetUniformLocation(shader, name.c_str()), val);
	}

	void shader::set_uniform(uint shader, const std::string& name, const glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}

	void shader::use(uint shader) {
		glUseProgram(shader);
	}

}; // namespace kanso
