#include "loaded_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace kanso {

	loaded_model::loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
	                           const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data)
	    : scene_model(render_shader, outline_shader, pos, scale, rot, data->aabb_min(), data->aabb_max()),
	      data_(std::move(data)),
	      renderer_(renderer_factory::make_renderer())
	{
		recalculate_bounding_box();
	}

	void loaded_model::draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		renderer_->reset_stencil_test();
		draw_model(render_shader(), view, proj, camera_pos);

		if (selected_) {
			renderer_->enable_stencil_test();
			draw_model(outline_shader_.id(), view, proj, camera_pos);
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

		draw_bounding_box(view, proj, camera_pos);
	}

	void loaded_model::draw_bounding_box(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		for (auto& line : aabb_box_) {
			line.draw(view, proj, camera_pos);
		}
	}

	glm::vec3 loaded_model::aabb_min() {
		model_matrix_ = { 1 };
		model_matrix_ = glm::translate(model_matrix_, position_);
		model_matrix_ = glm::scale(model_matrix_, scale_);

		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[0]), { 1, 0, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[1]), { 0, 1, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[2]), { 0, 0, 1 });

		return { model_matrix_ * glm::vec4(aabb_min_, 1.0f) };
	}

	glm::vec3 loaded_model::aabb_max() {
		model_matrix_ = { 1 };
		model_matrix_ = glm::translate(model_matrix_, position_);
		model_matrix_ = glm::scale(model_matrix_, scale_);

		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[0]), { 1, 0, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[1]), { 0, 1, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[2]), { 0, 0, 1 });

		return { model_matrix_ * glm::vec4(aabb_max_, 1.0f) };
	}

	void loaded_model::recalculate_bounding_box() {
		// NOLINTBEGIN(*union-access)
		std::array<glm::vec3, 8> aabb_box_vertices{
			{ { aabb_min_.x, aabb_min_.y, aabb_min_.z },
			  { aabb_min_.x, aabb_min_.y, aabb_max_.z },
			  { aabb_min_.x, aabb_max_.y, aabb_min_.z },
			  { aabb_min_.x, aabb_max_.y, aabb_max_.z },
			  { aabb_max_.x, aabb_min_.y, aabb_min_.z },
			  { aabb_max_.x, aabb_min_.y, aabb_max_.z },
			  { aabb_max_.x, aabb_max_.y, aabb_min_.z },
			  { aabb_max_.x, aabb_max_.y, aabb_max_.z } },
		};
		// NOLINTEND(*union-access)
		model_matrix_ = { 1 };
		model_matrix_ = glm::translate(model_matrix_, position_);
		model_matrix_ = glm::scale(model_matrix_, scale_);

		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[0]), { 1, 0, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[1]), { 0, 1, 0 });
		model_matrix_ = glm::rotate(model_matrix_, glm::radians(rotation_[2]), { 0, 0, 1 });

		for (auto& vertex : aabb_box_vertices) {
			vertex = glm::vec3(model_matrix_ * glm::vec4(vertex, 1.0f));
		}

		aabb_box_.reserve(12);
		aabb_box_.emplace_back(aabb_box_vertices[0], aabb_box_vertices[1]);
		aabb_box_.emplace_back(aabb_box_vertices[0], aabb_box_vertices[2]);
		aabb_box_.emplace_back(aabb_box_vertices[1], aabb_box_vertices[3]);
		aabb_box_.emplace_back(aabb_box_vertices[2], aabb_box_vertices[3]);

		aabb_box_.emplace_back(aabb_box_vertices[4], aabb_box_vertices[5]);
		aabb_box_.emplace_back(aabb_box_vertices[4], aabb_box_vertices[6]);
		aabb_box_.emplace_back(aabb_box_vertices[5], aabb_box_vertices[7]);
		aabb_box_.emplace_back(aabb_box_vertices[6], aabb_box_vertices[7]);

		aabb_box_.emplace_back(aabb_box_vertices[0], aabb_box_vertices[4]);
		aabb_box_.emplace_back(aabb_box_vertices[1], aabb_box_vertices[5]);
		aabb_box_.emplace_back(aabb_box_vertices[2], aabb_box_vertices[6]);
		aabb_box_.emplace_back(aabb_box_vertices[3], aabb_box_vertices[7]);
	}
} // namespace kanso
