#pragma once

#include <functional>
#include <map>

#include "window.hpp"
#include "camera.hpp"

namespace kanso {

	class event_wrapper {
		public:
			virtual ~event_wrapper() = default;

			virtual std::function<void(void*, double, double)>
				mouse_pos_evt_callback(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera)       = 0;
			virtual std::function<void(void*, enum mouse_button, enum button_status)> mouse_button_evt_callback() = 0;
			virtual std::function<void(void*)> keyboard_evt_callback(const std::shared_ptr<window>& w,
			                                                         const std::shared_ptr<camera>& camera)       = 0;
			virtual std::function<void(void*, double, double)>
				scroll_evt_callback(const std::shared_ptr<camera>& camera) = 0;
	};

	class event_system {
		public:
			event_system(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera);

		private:
			std::unique_ptr<event_wrapper> event_wrapper_;
	};

	class glfw_wrapper : public event_wrapper {
		public:
			glfw_wrapper();

			std::function<void(void*, double, double)>
				mouse_pos_evt_callback(const std::shared_ptr<window>& w, const std::shared_ptr<camera>& camera) override;
			std::function<void(void*, enum mouse_button, enum button_status)> mouse_button_evt_callback() override;
			std::function<void(void*)> keyboard_evt_callback(const std::shared_ptr<window>& w,
			                                                 const std::shared_ptr<camera>& camera) override;
			std::function<void(void*, double, double)>
				scroll_evt_callback(const std::shared_ptr<camera>& camera) override;

		private:
			struct mouse_pos {
					float x;
					float y;
			};

			std::map<enum mouse_button, int> mouse_buttons_map_;
			std::map<enum key_button, int>   key_buttons_map_;
			bool                             cursor_set_ = false;
			struct mouse_pos                 camera_point_ {
				- 1.0f, -1.0f
			};
			struct mouse_pos prev_point_ {
					- 1.0f, -1.0f
			};

			template <typename KeyType>
			bool is_key_pressed(void* ctx, KeyType key) {
				(void)ctx, (void)key;
			}
			template <>
			bool is_key_pressed(void* ctx, enum mouse_button key);
			template <>
			bool is_key_pressed(void* ctx, enum key_button key);
	};
} // namespace kanso
