#include "scene.hpp"

namespace kanso {

	scene::scene(std::vector<std::shared_ptr<loaded_model>> models, std::vector<std::shared_ptr<light>> lights)
	    : models_(std::move(models)),
	      lights_(std::move(lights)) {}

	void scene::draw(const camera& camera, const window& window) {
		auto view       = camera.get_view();
		auto proj       = camera.get_proj(window);
		auto camera_pos = camera.get_pos();

		for (const auto& model : models_) {
			for (const auto& light : lights_) {
				light->load_to(model->get_render_shader());
			}
			model->draw(view, proj, camera_pos);
		}
	}

} // namespace kanso
