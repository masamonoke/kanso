#include "raycast.hpp"

namespace kanso {
	raycast::raycast(const glm::vec3& origin, const glm::vec3& direction)
	    : ray_origin_(origin),
	      ray_direction_(direction) {}

	raycast::raycast(float mouse_x, float mouse_y, int screen_width, int screen_height, const glm::mat4& view,
	                 const glm::mat4& proj) {
		std::tie(ray_origin_, ray_direction_) = world_dir(mouse_x, mouse_y, screen_width, screen_height, view, proj);
	}

	bool raycast::is_intersects(const glm::vec3& aabb_min, const glm::vec3& aabb_max) const {
		const glm::vec3 inv_dir = 1.0f / ray_direction_;
		const glm::vec3 t_min   = (aabb_min - ray_origin_) * inv_dir;
		const glm::vec3 t_max   = (aabb_max - ray_origin_) * inv_dir;

		const glm::vec3 t1 = glm::min(t_min, t_max);
		const glm::vec3 t2 = glm::max(t_min, t_max);

		// NOLINTBEGIN(*union-access)
		const float t_entry = std::max(std::max(t1.x, t1.y), t1.z);
		const float t_exit  = std::min(std::min(t2.x, t2.y), t2.z);
		// NOLINTEND(*union-access)

		return t_entry <= t_exit && t_exit >= 0.0f;
	}

	std::pair<glm::vec3, glm::vec3> raycast::world_dir(float mouse_x, float mouse_y, int screen_width,
	                                                   int screen_height, const glm::mat4& view,
	                                                   const glm::mat4& proj) {
		const glm::vec4 ray_start_ndc{ (2.0f * mouse_x) / static_cast<float>(screen_width) - 1.0f,
			                           1.0f - (2.0f * mouse_y) / static_cast<float>(screen_height), -1.0, 1.0f };
		const glm::vec4 ray_end_ndc{ (2.0f * mouse_x) / static_cast<float>(screen_width) - 1.0f,
			                         1.0f - (2.0f * mouse_y) / static_cast<float>(screen_height), 0.0, 1.0f };

		const glm::mat4 m = glm::inverse(proj * view);

		// NOLINTBEGIN(*union-access)
		glm::vec4 ray_start_world = m * ray_start_ndc;
		ray_start_world /= ray_start_world.w;

		glm::vec4 ray_end_world = m * ray_end_ndc;
		ray_end_world /= ray_end_world.w;
		// NOLINTEND(*union-access)

		glm::vec3 ray_dir_world{ ray_end_world - ray_start_world };
		ray_dir_world = glm::normalize(ray_dir_world);

		// ray start should be from viewport center
		return { ray_start_world, ray_dir_world };
	}

} // namespace kanso
