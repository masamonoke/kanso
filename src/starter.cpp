#include "starter.hpp"

#include "spdlog/spdlog.h"

namespace kanso {

	namespace {
		nlohmann::json read_scene(const char* scene_path) {
			spdlog::debug("Reading '{}' scene", scene_path);
			return nlohmann::json::parse(std::ifstream(scene_path));
		}

	} // namespace

	starter::starter(int argc, char** argv) : argc_(argc), argv_(argv) {
#ifdef DEBUG
		spdlog::set_level(spdlog::level::debug);
#endif
	}

	std::unique_ptr<app> starter::make_app() {
		if (init_) {
			throw std::runtime_error("Trying to create app twice");
		}

		auto           args = std::span(argv_, static_cast<size_t>(argc_));
		nlohmann::json json;

		try {
			if (argc_ > 1) {
				json = read_scene(args[1]);
			} else {
				json = read_scene(kanso::DEFAULT_SCENE_PATH);
			}
		} catch (const nlohmann::json::parse_error& e) {
			spdlog::error(e.what());
			throw e;
		}

		try {
			init_ = true;
			return std::make_unique<app>(std::move(json));
		} catch (const kanso::exception::bad_scene_file_exception& e) {
			spdlog::error(e.what());
			throw e;
		}
	}

}
