#include "event_system.hpp"
#include "GLFW/glfw3.h"
#include "model.hpp"
#include "raycast.hpp"
/* #include "primitive.hpp" */

#include <spdlog/spdlog.h>

namespace kanso {

	event_system::event_system(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera, const std::shared_ptr<scene>& scene,
							   const std::shared_ptr<gui>& gui)
		: event_wrapper_(event_wrapper_factory::make_event_wrapper(camera, w, scene, gui)) {
		w->subscribe_on_mouse_click(event_wrapper_->mouse_button_evt_callback());
		w->subscribe_on_scroll_change(event_wrapper_->scroll_evt_callback());
		w->subsribe_on_keyboard_click(event_wrapper_->keyboard_evt_callback());
		w->subsribe_on_mouse_pos_change(event_wrapper_->mouse_pos_evt_callback());
		w->subscribe_on_focus_changed(event_wrapper_->focus_evt_callback());
		w->subscribe_on_char_input(event_wrapper_->char_input_callback());
		w->subscribe_on_cursor_enter(event_wrapper_->cursor_enter_evt_callback());
	}

	void glfw_wrapper::prepare_input_system() {
		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_W, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_front(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_S, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_back(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_D, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_right(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_A, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_left(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_Q, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_up(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_E, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::PRESS,
						[this]() { camera_->move_down(0.03f); });

		input_.bind_key(key_button::KANSO_KEYBOARD_BUTTON_F1, key_button_mod::KANSO_KEYBOARD_MOD_NONE, press_type::SINGLE_PRESS,
						[this]() { gui_->toggle_draw(); });
	}

	glfw_wrapper::glfw_wrapper(std::shared_ptr<camera> camera, std::shared_ptr<window> window,
	                           std::shared_ptr<scene> scene, std::shared_ptr<gui> gui)
	    : mouse_buttons_map_(mapped_mouse_buttons()),
		  key_buttons_map_(mapped_keys()),
		  camera_(std::move(camera)),
	      window_(std::move(window)),
	      scene_(std::move(scene)),
		  gui_(std::move(gui)),
		  input_(static_cast<GLFWwindow*>(window_->internal()))
	{
		prepare_input_system();
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

			if (gui_->handle_click(ctx, b, action)) {
				return;
			}

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

				auto it = std::find_if(scene_->model_begin(), scene_->model_end(), [ray](const auto& model) {
					if (!model->is_scene_model()) {
						return false;
					}

					const std::shared_ptr<const scene_model> sm = std::static_pointer_cast<const scene_model>(model);
					return ray.is_intersects(sm->aabb_min(), sm->aabb_max());
				});
				if (it != scene_->model_end()) {
					(*it)->select_toggle();
				}
			}
		};
	}

	std::function<void(void*)> glfw_wrapper::keyboard_evt_callback() {
		return [this](void*) {
			if (!gui_->blocked_keyboard()) {
				input_.handle_input();
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

	std::function<void(void*, int)> glfw_wrapper::focus_evt_callback() {
		return [](void*, int) {
		};
	}

	std::function<void(void*, int)> glfw_wrapper::cursor_enter_evt_callback() {
		return [](void*, int) {
		};
	}

	std::function<void(void*, uint)> glfw_wrapper::char_input_callback() {
		return [this](void*, uint c) {
			gui_->handle_char(c);
		};
	}

	bool glfw_wrapper::is_key_pressed(void* ctx, enum mouse_button key) {
		return glfwGetMouseButton(static_cast<GLFWwindow*>(ctx), mouse_buttons_map_[key]) == GLFW_PRESS;
	}

	bool glfw_wrapper::is_key_pressed(void* ctx, enum key_button key) {
		return glfwGetKey(static_cast<GLFWwindow*>(ctx), key_buttons_map_[key]) == GLFW_PRESS;
	}

	std::unique_ptr<event_wrapper> event_wrapper_factory::make_event_wrapper(const std::shared_ptr<camera>& camera,
																			 const std::shared_ptr<window>& window,
																			 const std::shared_ptr<scene>&  scene,
																			 const std::shared_ptr<gui>&    gui)
	{
#ifdef GLFWAPI
		return std::make_unique<glfw_wrapper>(camera, window, scene, gui);
#else
		throw exception::not_implemented_exception("Unknown window API");
#endif
	}

} // namespace kanso
