#include "event_system.hpp"
#include "GLFW/glfw3.h"

#include <spdlog/spdlog.h>

namespace kanso {

#ifdef GLFWAPI
#define evt_wrapper std::make_unique<glfw_wrapper>()
#endif

	event_system::event_system(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera)
	    : event_wrapper_(evt_wrapper) {
		w->subscribe_on_mouse_click(event_wrapper_->mouse_button_evt_callback());
		w->subscribe_on_scroll_change(event_wrapper_->scroll_evt_callback(camera));
		w->subsribe_on_keyboard_click(event_wrapper_->keyboard_evt_callback(w, camera));
		w->subsribe_on_mouse_pos_change(event_wrapper_->mouse_pos_evt_callback(w, camera));
	}

	glfw_wrapper::glfw_wrapper() {
		mouse_buttons_map_[KANSO_MOUSE_BUTTON_LEFT]  = GLFW_MOUSE_BUTTON_LEFT;
		mouse_buttons_map_[KANSO_MOUSE_BUTTON_RIGHT] = GLFW_MOUSE_BUTTON_RIGHT;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_W]    = GLFW_KEY_W;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_S]    = GLFW_KEY_S;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_D]    = GLFW_KEY_D;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_A]    = GLFW_KEY_A;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_Q]    = GLFW_KEY_Q;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_E]    = GLFW_KEY_E;
	}

	std::function<void(void*, double, double)>
	glfw_wrapper::mouse_pos_evt_callback(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera) {

		return [this, w, camera](void* ctx, double xpos, double ypos) {
			/* if (is_key_pressed(ctx, KANSO_MOUSE_BUTTON_RIGHT)) { */
			/* 	if (!cursor_set_) { */
			/* 		w->hide_cursor(); */
			/* 		cursor_set_ = true; */
			/* 	} */

			/* 	if (prev_point_.x == -1.0f) { */
			/* 		prev_point_.x = static_cast<float>(xpos); */
			/* 		prev_point_.y = static_cast<float>(ypos); */
			/* 	} */

			/* 	auto xshift = prev_point_.x - static_cast<float>(xpos); */
			/* 	auto yshift = prev_point_.y - static_cast<float>(ypos); */

			/* 	camera_point_.x -= xshift; */
			/* 	camera_point_.y -= yshift; */
				camera->update_view(xpos, ypos);

/* 				prev_point_.x = static_cast<float>(xpos); */
/* 				prev_point_.y = static_cast<float>(ypos); */
/* 				return; */
/* 			} */

/* 			if (cursor_set_) { */
/* 				cursor_set_ = false; */
/* 				w->show_cursor(); */
/* 			} */

/* 			prev_point_.x = -1.0f; */
/* 			prev_point_.y = -1.0f; */
		};
	}

	std::function<void(void*, enum mouse_button b, enum button_status action)>
	glfw_wrapper::mouse_button_evt_callback() {
		return [](void* ctx, enum mouse_button b, enum button_status action) {
			if (b == KANSO_MOUSE_BUTTON_LEFT && action == KANSO_STATUS_PRESS) {
				double xpos{};
				double ypos{};
				glfwGetCursorPos(static_cast<GLFWwindow*>(ctx), &xpos, &ypos);
				spdlog::debug("Button click in ({}, {})", xpos, ypos);
			}
		};
	}

	std::function<void(void*)> glfw_wrapper::keyboard_evt_callback(const std::shared_ptr<window>& w,
	                                                               const std::shared_ptr<camera>& camera) {
		return [w, camera](void* ctx) {
			(void)ctx;

			static auto last_frame = 0.0f;
			auto        cur_frame  = w->context_time();
			auto        delta_time = cur_frame - last_frame;
			last_frame             = cur_frame;

			auto  camera_speed = delta_time * 2.5f;
			auto* window       = static_cast<GLFWwindow*>(ctx);

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				camera->move_front(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				camera->move_back(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				camera->move_right(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				camera->move_left(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				camera->move_up(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
				camera->move_down(camera_speed);
			}
		};
	}

	std::function<void(void*, double, double)>
	glfw_wrapper::scroll_evt_callback(const std::shared_ptr<camera>& camera) {
		return [camera](void* ctx, double xoffset, double yoffset) {
			(void)ctx;
			(void)xoffset;

			auto fov = camera->get_fov();
			fov -= static_cast<float>(yoffset);
			camera->set_fov(fov);
		};
	}

	template <>
	bool glfw_wrapper::is_key_pressed(void* ctx, enum mouse_button key) {
		return glfwGetMouseButton(static_cast<GLFWwindow*>(ctx), mouse_buttons_map_[key]) == GLFW_PRESS;
	}

	template <>
	bool glfw_wrapper::is_key_pressed(void* ctx, enum key_button key) {
		return glfwGetKey(static_cast<GLFWwindow*>(ctx), key_buttons_map_[key]) == GLFW_PRESS;
	}
} // namespace kanso
