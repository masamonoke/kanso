#include "app.hpp"

#include <spdlog/spdlog.h>

namespace kanso {
	app::app(nlohmann::json json)
	    : close_(false),
	      window_(std::make_shared<glfw_window>()),
	      loader_(std::make_unique<loader>(std::move(json))),
		  scene_(loader_->make_scene()),
		  camera_(loader_->make_camera()),
		  renderer_(renderer_factory::make_renderer()),
		  gui_(gui_factory::make_gui(window_, scene_)),
		  event_system_(std::make_unique<event_system>(window_, camera_, scene_, gui_)) {}

	void app::update() {
		renderer_->clear(0.5f, 0.5f, 0.5f);

		scene_->draw(*camera_, *window_);

		gui_->draw();

		// window update should be last
		window_->update();
	}

	void app::input() {
		if (window_->should_close()) {
			close_ = true;
		}
	}

	void app::run() {
		while (!close()) {
			input();
			update();
		}
	}

} // namespace kanso
