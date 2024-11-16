#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace kanso {

	constexpr float MIN_FOV = 90.0f;
	constexpr float MAX_FOV = 150.0f;

	camera::camera(const glm::vec3& pos, float fov, float near, float far)
	    : pos_(pos),
	      front_(0.0f, 0.0f, -1.f),
	      up_(0.0f, 1.0f, 0.0f),
	      fov_(fov),
	      far_(far),
	      near_(near) {}

	void camera::move_front(float speed) {
		pos_ += front_ * speed;
	}

	void camera::move_back(float speed) {
		pos_ -= front_ * speed;
	}

	glm::vec3 camera::front_up_cross() {
		static auto cross = glm::cross(front_, up_);
		return cross;
	}

	void camera::move_left(float speed) {
		pos_ -= glm::cross(front_, up_) * speed;
	}

	void camera::move_right(float speed) {
		pos_ += glm::cross(front_, up_) * speed;
	}

	void camera::move_up(float speed) {
		pos_ += up_ * speed;
	}

	void camera::move_down(float speed) {
		pos_ -= up_ * speed;
	}

	void camera::update_view(float x, float y) {
		const float SENSITIVITY = 0.1f;

		if (update_first_time_) {
			update_last_x_     = x;
			update_last_y_     = y;
			update_first_time_ = false;
		}

		auto xoffset = (x - update_last_x_) * SENSITIVITY;
		auto yoffset = (update_last_y_ - y) * SENSITIVITY; // y coord goes from bottom to top

		update_last_x_ = x;
		update_last_y_ = y;

		update_yaw_ += xoffset;
		update_pitch_ += yoffset;

		if (update_pitch_ > 89.0f) {
			update_pitch_ = 89.0f;
		}
		if (update_pitch_ < -89.0f) {
			update_pitch_ = -89.0f;
		}

		const auto front_x = std::cos(glm::radians(update_yaw_)) * std::cos(glm::radians(update_pitch_));
		const auto front_y = std::sin(glm::radians(update_pitch_));
		const auto front_z = std::sin(glm::radians(update_yaw_)) * std::cos(glm::radians(update_pitch_));

		const glm::vec3 front{ front_x, front_y, front_z };
		front_ = glm::normalize(front);
	}

	glm::mat4 camera::proj(const window& w) const {
		auto aspect = static_cast<float>(w.real_width()) / static_cast<float>(w.real_height());
		return glm::perspective(glm::radians(fov_), aspect, near_, far_);
	}

	glm::mat4 camera::view() const {
		auto dir = pos_ + front_;
		return glm::lookAt(pos_, dir, up_);
	}

	glm::vec3 camera::pos() const {
		return pos_;
	}

	float camera::fov() const {
		return fov_;
	}

	void camera::fov(float fov) {
		fov_ = glm::clamp(fov, MIN_FOV, MAX_FOV);
	}

} // namespace kanso
