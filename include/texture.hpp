#pragma once

#include <string>
#include <vector>

#include "core.hpp"

#include <assimp/material.h>

namespace kanso {

	namespace internal {
		struct tex_data {
			uint8_t*    bytes;
			int32_t     width;
			int32_t     height;
			int32_t     nr_channels;
			std::string path;
			std::string type;
		};

		class tex_map {
			public:
				explicit tex_map(const tex_data& data);

				[[nodiscard]] std::string type() const {
					return type_;
				}

				[[nodiscard]] uint id() const {
					return id_;
				}

			private:
				uint        id_;
				std::string type_;
				std::string path_;
		};
	} // namespace internal

	class texture {
		public:
			texture(const std::string& dir, const aiMaterial* material);

			void bind(uint shader) const;

		private:
			std::vector<internal::tex_map> maps_;
	};

} // namespace kanso
