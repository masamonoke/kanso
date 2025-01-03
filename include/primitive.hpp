#pragma once

#include "model.hpp"
#include "renderer.hpp"

namespace kanso {

	class line : public model {
		public:
			line(const glm::vec3& start, const glm::vec3& end, std::string_view vert_file = "shaders/line.vert", std::string_view frag_file = "shaders/line.frag");

			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) const override;
			void select_toggle() override {}

			std::string type() const override {
				return "line";
			}
			glm::vec3 pos() const override {
				return start_;
			}
			glm::vec3 rot() const override {
				return {};
			}
			glm::vec3 scale() const override {
				return {};
			}
			std::string name() const override {
				return "primitive";
			}
			bool is_scene_model() const override {
				return false;
			}

		private:
			std::unique_ptr<renderer> renderer_;
			glm::vec3 start_;
	};

}
