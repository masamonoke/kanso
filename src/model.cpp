#include "model.hpp"
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace kanso {

	loaded_model::loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
	                           const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data)
	    : scene_model(render_shader, outline_shader, pos, scale, rot),
	      data_(std::move(data)) {}

	void loaded_model::draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		renderer::reset_stencil_test();
		draw_model(get_render_shader(), view, proj, camera_pos);

		if (is_selected()) {
			renderer::enable_stencil_test();
			draw_model(get_outline_shader(), view, proj, camera_pos);
			renderer::reset_stencil_test();
		}
	}

	void loaded_model::draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj,
	                              const glm::vec3& camera_pos) {
		shader::use(shader);

		glm::mat4 model{ 1 };
		model = glm::translate(model, get_pos());
		model = glm::scale(model, get_scale());

		model = glm::rotate(model, glm::radians(get_rot()[0]), { 1, 0, 0 });
		model = glm::rotate(model, glm::radians(get_rot()[1]), { 0, 1, 0 });
		model = glm::rotate(model, glm::radians(get_rot()[2]), { 0, 0, 1 });

		shader::set_uniform(shader, "model", model);
		shader::set_uniform(shader, "view", view);
		shader::set_uniform(shader, "proj", proj);

		shader::set_uniform(shader, "viewPos", camera_pos);
		shader::set_uniform(shader, "material.shininess", 32.0f);

		for (auto it = data_->meshes_begin(), end = data_->meshes_end(); it != end; ++it) {
			it->draw(shader);
		}
	}

} // namespace kanso
