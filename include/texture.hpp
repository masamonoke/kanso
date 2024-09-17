#pragma once

#include <string>
#include <vector>

#include "core.hpp"

namespace kanso {

	struct raw_tex {
		uint8_t*    bytes = nullptr;
		int32_t     width{};
		int32_t     height{};
		int32_t     nr_channels{};
		std::string path;
		std::string type;
	};

	class tex_map {
		public:
			explicit tex_map(const raw_tex& data);

			[[nodiscard]] std::string type() const {
				return type_;
			}

			[[nodiscard]] uint id() const {
				return id_;
			}

		private:
			std::string type_;
			std::string path_;
			uint        id_;
	};

	class texture {
		public:
			texture(const std::vector<raw_tex>& raw_tex);

			void bind(uint shader) const;

		private:
			std::vector<tex_map> maps_;
	};

} // namespace kanso
