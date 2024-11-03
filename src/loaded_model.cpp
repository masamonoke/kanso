#include "loaded_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace kanso {

	loaded_model::loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
	                           const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data)
	    : scene_model(render_shader, outline_shader, pos, scale, rot, data->get_aabb_min(), data->get_aabb_max()),
	      data_(std::move(data)), renderer_(renderer_factory::make_renderer()) {}

	void loaded_model::draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		renderer_->reset_stencil_test();
		draw_model(get_render_shader(), view, proj, camera_pos);

		if (is_selected()) {
			renderer_->enable_stencil_test();
			draw_model(get_outline_shader(), view, proj, camera_pos);
			renderer_->reset_stencil_test();
		}
	}

	void loaded_model::draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj,
	                              const glm::vec3& camera_pos) {
		shader::use(shader);

		model_matrix_ = { 1 };
		model_matrix_ = glm::translate(model_matrix_, position_);
		model_matrix_ = glm::scale(model_matrix_, scale_);

		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[0]), { 1, 0, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[1]), { 0, 1, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[2]), { 0, 0, 1 });

		shader::set_uniform(shader, "model", model_matrix_);
		shader::set_uniform(shader, "view", view);
		shader::set_uniform(shader, "proj", proj);

		shader::set_uniform(shader, "viewPos", camera_pos);
		shader::set_uniform(shader, "material.shininess", 32.0f);

		for (auto it = data_->meshes_begin(), end = data_->meshes_end(); it != end; ++it) {
			it->draw(shader);
		}
	}

	void loaded_model::draw_bounding_box(uint shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
	}

}
