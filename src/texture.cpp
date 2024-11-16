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
#include "renderer.hpp"

#include <spdlog/spdlog.h>

namespace kanso {

	tex_map::tex_map(const raw_tex& data)
	    : type_(data.type),
	      path_(data.path),
	      id_(renderer_factory::make_renderer()->load_texture(data.bytes, data.nr_channels, data.width, data.height)) {}

	texture::texture(const std::vector<raw_tex>& raw_textures) : renderer_(renderer_factory::make_renderer()) {
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

			renderer_->bind_texture(shader, map.type(), map.id(), diffuse_nr, specular_nr, height_nr, normal_nr, number,
			                        index);

			index++;
		}
	}

} // namespace kanso
