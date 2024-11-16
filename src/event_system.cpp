#include "event_system.hpp"
#include "GLFW/glfw3.h"
#include "raycast.hpp"
/* #include "primitive.hpp" */

#include <spdlog/spdlog.h>

namespace kanso {

#ifdef GLFWAPI
	template <typename Camera, typename Window, typename Scene>
	constexpr auto evt_wrapper(Camera&& camera, Window&& window, Scene&& scene) {
		return std::make_unique<glfw_wrapper>(std::forward<Camera>(camera), std::forward<Window>(window),
		                                      std::forward<Scene>(scene));
	}
#endif

	event_system::event_system(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera,
	                           const std::shared_ptr<scene>& scene)
	    : event_wrapper_(evt_wrapper(camera, w, scene)) {
		w->subscribe_on_mouse_click(event_wrapper_->mouse_button_evt_callback());
		w->subscribe_on_scroll_change(event_wrapper_->scroll_evt_callback());
		w->subsribe_on_keyboard_click(event_wrapper_->keyboard_evt_callback());
		w->subsribe_on_mouse_pos_change(event_wrapper_->mouse_pos_evt_callback());
	}

	glfw_wrapper::glfw_wrapper(std::shared_ptr<camera> camera, std::shared_ptr<window> window,
	                           std::shared_ptr<scene> scene)
	    : camera_(std::move(camera)),
	      window_(std::move(window)),
	      scene_(std::move(scene)) {
		mouse_buttons_map_[KANSO_MOUSE_BUTTON_LEFT]  = GLFW_MOUSE_BUTTON_LEFT;
		mouse_buttons_map_[KANSO_MOUSE_BUTTON_RIGHT] = GLFW_MOUSE_BUTTON_RIGHT;

		key_buttons_map_[KANSO_KEYBOARD_BUTTON_W] = GLFW_KEY_W;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_S] = GLFW_KEY_S;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_D] = GLFW_KEY_D;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_A] = GLFW_KEY_A;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_Q] = GLFW_KEY_Q;
		key_buttons_map_[KANSO_KEYBOARD_BUTTON_E] = GLFW_KEY_E;
	}

	std::function<void(void*, double, double)> glfw_wrapper::mouse_pos_evt_callback() {

		return [this](void* ctx, double xpos, double ypos) {
			if (is_key_pressed(ctx, KANSO_MOUSE_BUTTON_RIGHT)) {
				float x_shift{};
				float y_shift{};

				if (!cursor_set_) {
					window_->hide_cursor();
					cursor_set_ = true;
				}

				if (prev_point_.x < 0.0f) {
					prev_point_.x = static_cast<float>(xpos);
					prev_point_.y = static_cast<float>(ypos);
				}

				x_shift = prev_point_.x - static_cast<float>(xpos);
				y_shift = prev_point_.y - static_cast<float>(ypos);

				camera_point_.x -= x_shift;
				camera_point_.y -= y_shift;
				camera_->update_view(camera_point_.x, camera_point_.y);

				prev_point_.x = static_cast<float>(xpos);
				prev_point_.y = static_cast<float>(ypos);
				return;
			}

			if (cursor_set_) {
				cursor_set_ = false;
				window_->show_cursor();
			}

			prev_point_.x = -1.0f;
			prev_point_.y = -1.0f;
		};
	}

	std::function<void(void*, enum mouse_button b, enum button_status action)>
	glfw_wrapper::mouse_button_evt_callback() {

		return [this](void* ctx, enum mouse_button b, enum button_status action) {
			const auto camera_view = camera_->view();
			const auto camera_proj = camera_->proj(*window_);

			const auto screen_width  = window_->width();
			const auto screen_height = window_->height();

			if (b == KANSO_MOUSE_BUTTON_LEFT && action == KANSO_STATUS_PRESS) {
				double xpos{};
				double ypos{};
				glfwGetCursorPos(static_cast<GLFWwindow*>(ctx), &xpos, &ypos);

				const raycast ray{ static_cast<float>(xpos),
					               static_cast<float>(ypos),
					               screen_width,
					               screen_height,
					               camera_view,
					               camera_proj };

				/* auto displacement = ray.get_dir() * 50.0f; */
				/* scene_->add_model(std::make_unique<line>(ray.get_origin() + glm::vec3{ 0, 0, -0.5 }, */
				/*                                          ray.get_origin() + displacement)); */

				auto it = std::find_if(scene_->begin(), scene_->end(), [ray](const auto& model) {
					auto       sm           = std::static_pointer_cast<scene_model>(model);
					return ray.is_intersects(sm->aabb_min(), sm->aabb_max());
				});
				if (it != scene_->end()) {
					(*it)->select_toggle();
				}
			}
		};
	}

	std::function<void(void*)> glfw_wrapper::keyboard_evt_callback() {
		return [this](void* ctx) {
			(void)ctx;

			static auto last_frame = 0.0f;
			auto        cur_frame  = window_->context_time();
			auto        delta_time = cur_frame - last_frame;
			last_frame             = cur_frame;

			auto  camera_speed = delta_time * 2.5f;
			auto* window       = static_cast<GLFWwindow*>(ctx);

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				camera_->move_front(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				camera_->move_back(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				camera_->move_right(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				camera_->move_left(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				camera_->move_up(camera_speed);
			}

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
				camera_->move_down(camera_speed);
			}
		};
	}

	std::function<void(void*, double, double)> glfw_wrapper::scroll_evt_callback() {
		return [this](void* ctx, double xoffset, double yoffset) {
			(void)ctx;
			(void)xoffset;

			auto fov = camera_->fov();
			fov -= static_cast<float>(yoffset);
			camera_->fov(fov);
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
