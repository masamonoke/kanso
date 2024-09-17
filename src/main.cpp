#include <cstddef>
#include <fstream>
#include <span>
#include <string>

#include "app.hpp"

#ifdef DEBUG
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#endif

namespace {

	nlohmann::json read_scene(const char* scene_path) {
		spdlog::debug("Reading '{}' scene", scene_path);
		return nlohmann::json::parse(std::ifstream(scene_path));
	}

	void run(std::unique_ptr<kanso::app> app) {
		while (!app->close()) {
			app->input();
			app->update();
		}
	}

} // namespace

int main(int argc, char** argv) {
#ifdef DEBUG
	spdlog::set_level(spdlog::level::debug);
#endif
	{
		auto           args = std::span(argv, static_cast<size_t>(argc));
		nlohmann::json json;

		try {
			if (argc > 1) {
				json = read_scene(args[1]);
			} else {
				json = read_scene(kanso::DEFAULT_SCENE_PATH);
			}
		} catch (const nlohmann::json::parse_error& e) {
			spdlog::error(e.what());
			return 1;
		}

		try {
			auto app = std::make_unique<kanso::app>(std::move(json));
			run(std::move(app));
		} catch (const kanso::exception::bad_scene_file_exception& e) {
			spdlog::error(e.what());
			return 1;
		}
	}

	spdlog::info("Gracefully shutting down");

	return 0;
}
