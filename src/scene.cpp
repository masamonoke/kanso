#include "scene.hpp"

namespace kanso {

	scene::scene(std::vector<std::shared_ptr<model>> models, std::vector<std::shared_ptr<light>> lights)
	    : models_(std::move(models)),
	      lights_(std::move(lights)) {}

	void scene::draw(const camera& camera, const window& window) {
		auto view       = camera.view();
		auto proj       = camera.proj(window);
		auto camera_pos = camera.pos();

		for (const auto& model : models_) {
			for (const auto& light : lights_) {
				light->bind_to(model->render_shader());
			}
			model->draw(view, proj, camera_pos);
		}
	}

	void scene::add_model(std::unique_ptr<model> model) {
		models_.emplace_back(std::move(model));
	}


} // namespace kanso
