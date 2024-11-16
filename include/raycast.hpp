#pragma once

#include <glm/glm.hpp>

#include <utility>

namespace kanso {

	class raycast {
		public:
			raycast(const glm::vec3& origin, const glm::vec3& direction);
			raycast(float mouse_x, float mouse_y, int screen_width, int screen_height, const glm::mat4& view, const glm::mat4& proj);

			bool is_intersects(const glm::vec3& aabb_min, const glm::vec3& aabb_max) const;

		private:
			glm::vec3 ray_origin_{};
			glm::vec3 ray_direction_{};

			static std::pair<glm::vec3, glm::vec3> world_dir(float mouse_x, float mouse_y, int screen_width,
			                                                       int screen_height, const glm::mat4& view,
			                                                       const glm::mat4& proj);

	};

} // namespace kanso
