#pragma once

#include <functional>
#include <map>

#include "window.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "gui.hpp"
#include "input_system.hpp"

namespace kanso {

	class event_wrapper {
		public:
			virtual ~event_wrapper() = default;

			virtual std::function<void(void*, double, double)>                        mouse_pos_evt_callback()    = 0;
			virtual std::function<void(void*, enum mouse_button, enum button_status)> mouse_button_evt_callback() = 0;
			virtual std::function<void(void*)>                                        keyboard_evt_callback()     = 0;
			virtual std::function<void(void*, double, double)>                        scroll_evt_callback()       = 0;
			virtual std::function<void(void*, int)>                                   focus_evt_callback()        = 0;
	};

	class event_system {
		public:
			event_system(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera, const std::shared_ptr<scene>& scene, const std::shared_ptr<gui>& gui);

		private:
			std::unique_ptr<event_wrapper> event_wrapper_;
	};

	class glfw_wrapper : public event_wrapper {
		public:
			glfw_wrapper(std::shared_ptr<camera> camera, std::shared_ptr<window> window, std::shared_ptr<scene> scene, std::shared_ptr<gui> gui);

			std::function<void(void*, double, double)>                        mouse_pos_evt_callback() override;
			std::function<void(void*, enum mouse_button, enum button_status)> mouse_button_evt_callback() override;
			std::function<void(void*)>                                        keyboard_evt_callback() override;
			std::function<void(void*, double, double)>                        scroll_evt_callback() override;
			std::function<void(void*, int)>                                   focus_evt_callback() override;

		private:
			struct mouse_pos {
				float x;
				float y;
			};

			std::map<enum mouse_button, int> mouse_buttons_map_;
			std::map<enum key_button, int>   key_buttons_map_;
			bool                             cursor_set_ = false;
			mouse_pos                        camera_point_{ -1.0f, -1.0f };
			mouse_pos                        prev_point_{ -1.0f, -1.0f };
			std::shared_ptr<camera>          camera_;
			std::shared_ptr<window>          window_;
			std::shared_ptr<scene>           scene_;
			std::shared_ptr<gui>             gui_;
			glfw_input                       input_;

			template <typename KeyType>
			bool is_key_pressed(void* ctx, KeyType key) {
				(void)ctx, (void)key;
			}
			template <>
			bool is_key_pressed(void* ctx, enum mouse_button key);
			template <>
			bool is_key_pressed(void* ctx, enum key_button key);

			void prepare_input_system();
	};

	namespace event_wrapper_factory {
		std::unique_ptr<event_wrapper> make_event_wrapper(const std::shared_ptr<camera> &camera, const std::shared_ptr<window> &window,
														  const std::shared_ptr<scene> &scene, const std::shared_ptr<gui> &gui);
	};

} // namespace kanso
