#include "texture.hpp"
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
#include "shader.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace kanso {

	namespace {

		uint load_to_gpu(const raw_tex& data) {
			if (data.bytes == nullptr) {
				throw std::invalid_argument("Texture binary data is nullptr");
			}

			GLenum format{};
			switch (data.nr_channels) {
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

			glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), data.width, data.height, 0, format,
			             GL_UNSIGNED_BYTE, data.bytes);
			glGenerateMipmap(GL_TEXTURE_2D);

			return texture_id;
		}

	} // anonymous namespace

	tex_map::tex_map(const raw_tex& data) : type_(data.type), path_(data.path), id_(load_to_gpu(data)) {}

	texture::texture(const std::vector<raw_tex>& raw_textures) {
		maps_.reserve(raw_textures.size());
		for (auto&& raw : raw_textures) {
			maps_.emplace_back(raw);
		}
	}

	void texture::bind(uint shader) const {
		if (maps_.empty()) {
			shader::set_uniform(shader, "useTexture", false);
			return;
		}

		shader::set_uniform(shader, "useTexture", true);

		uint diffuse_nr  = 1;
		uint specular_nr = 1;
		uint height_nr   = 1;
		uint normal_nr   = 1;
		uint number{};
		uint index = 0;

		for (const auto& map : maps_) {

			glActiveTexture(GL_TEXTURE0 + index);
			auto name = map.type();
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
			glBindTexture(GL_TEXTURE_2D, map.id());

			index++;
		}
	}

} // namespace kanso
