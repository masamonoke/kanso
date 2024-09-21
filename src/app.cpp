#include "app.hpp"

#include <spdlog/spdlog.h>

namespace kanso {
	app::app(nlohmann::json&& json)
	    : close_(false),
	      window_(std::make_shared<glfw_window>()),
	      loader_(std::make_unique<loader>(std::move(json))) {
		scene_        = loader_->make_scene();
		camera_       = loader_->make_camera();
		event_system_ = std::make_unique<event_system>(window_, camera_);
	}

	void app::update() {
		renderer::clear(0.5f, 0.5f, 0.5f);

		scene_->draw(*camera_, *window_);
		window_->update();
	}

	void app::input() {
		if (window_->should_close()) {
			close_ = true;
		}
	}
} // namespace kanso
