#pragma once

#include "model.hpp"
#include "primitive.hpp"
#include "model_data_loader.hpp"

namespace kanso {

	class loaded_model : public scene_model {
		public:
			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) override;

			loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			             const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data);

			loaded_model(const loaded_model&&)             = delete;
			loaded_model&& operator=(const loaded_model&&) = delete;

			glm::vec3 aabb_min() override;
			glm::vec3 aabb_max() override;

		private:
			std::shared_ptr<model_data> data_;
			std::unique_ptr<renderer>   renderer_;
			std::vector<line>           aabb_box_;

			void draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos);
			void draw_bounding_box(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos);
			void recalculate_bounding_box();
	};

} // namespace kanso
