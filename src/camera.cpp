#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace kanso {

	constexpr glm::vec3 front(float scalar) {
		return { 0.0f, 0.0f, -1.f * scalar };
	}

	constexpr glm::vec3 up(float scalar) {
		return { 0.0f, 1.0f * scalar, 0.0f };
	}

	constexpr glm::vec3 front() {
		return front(1);
	}

	constexpr glm::vec3 up() {
		return up(1);
	}

	constexpr float MIN_FOV = 90.0f;
	constexpr float MAX_FOV = 150.0f;

	camera::camera(const glm::vec3& pos, float fov, float near, float far)
	    : pos_(pos),
	      front_(front()),
	      up_(up()),
	      fov_(fov),
	      far_(far),
	      near_(near) {}

	void camera::move_front(float speed) {
		auto inc = front(speed);
		pos_ += inc;
	}

	void camera::move_back(float speed) {
		auto inc = front(speed);
		pos_ -= inc;
	}

	glm::vec3 camera::front_up_cross() {
		static auto cross = glm::normalize(glm::cross(front_, up_));
		return cross;
	}

	void camera::move_left(float speed) {
		auto cross = front_up_cross() * speed;
		pos_ -= cross;
	}

	void camera::move_right(float speed) {
		auto cross = front_up_cross() * speed;
		pos_ += cross;
	}

	void camera::move_up(float speed) {
		auto inc = up(speed);
		pos_ += inc;
	}

	void camera::move_down(float speed) {
		auto inc = up(speed);
		pos_ -= inc;
	}

	void camera::update_view(float x, float y) {
		float       xoffset{};
		float       yoffset{};
		const float SENSITIVITY = 0.1f;

		if (update_first_time_) {
			update_last_x_     = x;
			update_last_y_     = y;
			update_first_time_ = false;
		}

		xoffset = (x - update_last_x_) * SENSITIVITY;
		yoffset = (update_last_y_ - y) * SENSITIVITY; // y coord goes from bottom to top

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

		auto front_x = std::cos(glm::radians(update_yaw_) * std::cos(glm::radians(update_pitch_)));
		auto front_y = std::sin(glm::radians(update_pitch_));
		auto front_z = std::sin(glm::radians(update_yaw_)) * std::cos(glm::radians(update_pitch_));

		const glm::vec3 front{ front_x, front_y, front_z };
		front_ = glm::normalize(front);
	}

	glm::mat4 camera::get_proj(const window& w) const {
		auto aspect = static_cast<float>(w.width()) / static_cast<float>(w.height());
		return glm::perspective(glm::radians(fov_), aspect, near_, far_);
	}

	glm::mat4 camera::get_view() const {
		auto dir = pos_ + front_;
		return glm::lookAt(pos_, dir, up_);
	}

	glm::vec3 camera::get_pos() const {
		return pos_;
	}

	float camera::get_fov() const {
		return fov_;
	}

	void camera::set_fov(float fov) {
		fov_ = glm::clamp(fov, MIN_FOV, MAX_FOV);
	}

} // namespace kanso
