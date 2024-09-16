#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "window.hpp"
#include "exception.hpp"

namespace kanso {

	class camera {
		public:
			camera(const glm::vec3& pos, float fov, float near, float far);

			[[nodiscard]] glm::mat4 get_proj(const window& w) const;
			[[nodiscard]] glm::mat4 get_view() const;
			[[nodiscard]] glm::vec3 get_pos() const;
			[[nodiscard]] float     get_fov() const;

			void set_fov(float fov);

			void move_front(float speed);
			void move_left(float speed);
			void move_right(float speed);
			void move_up(float speed);
			void move_down(float speed);
			void move_back(float speed);

			void update_view(float x, float y);

		private:
			glm::vec3 pos_;
			glm::vec3 front_;
			glm::vec3 up_;
			float     fov_;
			float     far_;
			float     near_;
			bool      update_first_time_ = true;
			float     update_last_x_{};
			float     update_last_y_{};
			float     update_yaw_   = -90.0f;
			float     update_pitch_ = 0.0f;

			[[nodiscard]] glm::vec3 front_up_cross();
	};

} // namespace kanso