#include "raycast.hpp"

// https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
namespace kanso {

	namespace {
		bool intersect_axis(const glm::vec3& axis, const glm::vec3& ray_dir, float aabb_min, float aabb_max,
		                    const glm::vec3& delta) {
			float t_min = 0.0f;
			float t_max = 100000.0f;

			const float e = glm::dot(axis, delta);
			const float f = glm::dot(ray_dir, axis);

			if ( std::abs(f) > 0.001f ){

				float t1 = (e + aabb_min) / f;
				float t2 = (e + aabb_max) / f;

				if (t1 > t2){
					std::swap(t1, t2);
				}

				if ( t2 < t_max ) {
					t_max = t2;
				}
				if ( t1 > t_min ) {
					t_min = t1;
				}
				if (t_min > t_max) {
					return false;
				}

			} else {
				if (-e + aabb_min > 0.0f || -e + aabb_max < 0.0f) {
					return false;
				}
			}

			return true;
		}
	} // namespace

	raycast::raycast(const glm::vec3& origin, const glm::vec3& direction)
	    : ray_origin_(origin),
	      ray_direction_(direction) {}

	raycast::raycast(float mouse_x, float mouse_y, int screen_width, int screen_height, const glm::mat4& view, const glm::mat4& proj) {
		std::tie(ray_origin_, ray_direction_) = world_dir(mouse_x, mouse_y, screen_width, screen_height, view, proj);
	}

	bool raycast::is_intersects(const glm::vec3& aabb_min, const glm::vec3& aabb_max, const glm::mat4& model) const {
		// NOLINTBEGIN(*union-access)
		const glm::vec3 OBBposition_worldspace(model[3].x, model[3].y, model[3].z);

		const glm::vec3 delta = OBBposition_worldspace - ray_origin_;

		if (!intersect_axis(model[0], ray_direction_, aabb_min.x, aabb_max.x, delta)) {
			return false;
		}
		if (!intersect_axis(model[1], ray_direction_, aabb_min.y, aabb_max.y, delta)) {
			return false;
		}
		if (!intersect_axis(model[2], ray_direction_, aabb_min.z, aabb_max.z, delta)) {
			return false;
		}
		// NOLINTEND(*union-access)

		return true;
	}

	std::pair<glm::vec3, glm::vec3> raycast::world_dir(float mouse_x, float mouse_y, int screen_width,
	                                                         int screen_height, const glm::mat4& view,
	                                                         const glm::mat4& proj)
	{
		const glm::vec4 ray_start_ndc{
			(2.0f * mouse_x) / static_cast<float>(screen_width) - 1.0f,
			1.0f - (2.0f * mouse_y) / static_cast<float>(screen_height),
			-1.0,
			1.0f
		};
		const glm::vec4 ray_end_ndc{
			(2.0f * mouse_x) / static_cast<float>(screen_width) - 1.0f,
			1.0f - (2.0f * mouse_y) / static_cast<float>(screen_height),
			0.0,
			1.0f
		};

		const glm::mat4 m = glm::inverse(proj * view);

		// NOLINTBEGIN(*union-access)
		glm::vec4 ray_start_world = m * ray_start_ndc;
		ray_start_world /= ray_start_world.w;

		glm::vec4 ray_end_world   = m * ray_end_ndc;
		ray_end_world /= ray_end_world.w;
		// NOLINTEND(*union-access)


		glm::vec3 ray_dir_world{ ray_end_world - ray_start_world };
		ray_dir_world = glm::normalize(ray_dir_world);


		// ray start should be from viewport center
		return { ray_start_world, ray_dir_world };
	}

} // namespace kanso
