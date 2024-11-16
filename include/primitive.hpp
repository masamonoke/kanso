#pragma once

#include "model.hpp"
#include "renderer.hpp"

namespace kanso {

	class line : public model {
		public:
			line(const glm::vec3& start, const glm::vec3& end, std::string_view vert_file = "shaders/line.vert", std::string_view frag_file = "shaders/line.frag");

			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) override;
			void select_toggle() override {}

		private:
			std::unique_ptr<renderer> renderer_;
	};

}
