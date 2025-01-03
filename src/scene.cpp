#include "scene.hpp"
#include "light.hpp"
#include "model.hpp"

namespace kanso {

	scene::scene(std::shared_ptr<object_manager> manager)
	    :  obj_manager_(std::move(manager)) {}

	void scene::draw(const camera& camera, const window& window) {
		auto view       = camera.view();
		auto proj       = camera.proj(window);
		auto camera_pos = camera.pos();

		for (auto model_it = obj_manager_->model_begin(), model_end = obj_manager_->model_end(); model_it != model_end; ++model_it) {
			for (auto light_it = obj_manager_->light_begin(), light_end = obj_manager_->light_end(); light_it != light_end; ++light_it) {
				light_it->get()->bind_to(model_it->get()->render_shader());
			}
			model_it->get()->draw(view, proj, camera_pos);
		}
	}

	void scene::add_model(std::unique_ptr<model> model) {
		obj_manager_->add_model(std::move(model));
	}

	std::vector<model_view>::iterator scene::view_begin() const {
		return obj_manager_->view_begin();
	}

	std::vector<model_view>::iterator scene::view_end() const {
		return obj_manager_->view_end();
	}

	std::vector<std::shared_ptr<model>>::const_iterator scene::model_begin() {
		return obj_manager_->model_begin();
	}

	std::vector<std::shared_ptr<model>>::const_iterator scene::model_end() {
		return obj_manager_->model_end();
	}

} // namespace kanso
