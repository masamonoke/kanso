#pragma once

#include "model.hpp"

namespace kanso {

	class loaded_model : public scene_model {
		public:
			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) override;

			loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			             const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data);

		private:
			std::shared_ptr<model_data> data_;
			std::unique_ptr<renderer> renderer_;

			void draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos);
			void draw_bounding_box(uint shader, const glm::mat4& view, const glm::mat4& proj,
	                              const glm::vec3& camera_pos);
	};

}
