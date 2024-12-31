#pragma once

#include <memory>
#include <functional>
#include <map>

#include "GLFW/glfw3.h"
#include "core.hpp"
#include "renderer.hpp"

namespace kanso {

	class window {
		public:
			window()          = default;
			virtual ~window() = default;

			virtual void hide_cursor() = 0;
			virtual void show_cursor() = 0;

			virtual void subsribe_on_mouse_pos_change(std::function<void(void*, double, double)> mouse_callback) = 0;
			virtual void subscribe_on_scroll_change(std::function<void(void*, double, double)> scroll_callback)  = 0;
			virtual void
				subscribe_on_mouse_click(std::function<void(void*, enum mouse_button, enum button_status)> mouse_click_callback) = 0;
			virtual void subsribe_on_keyboard_click(std::function<void(void*)> keyboard_click_callback)                      = 0;
			virtual void subscribe_on_focus_changed(std::function<void(void*, int)> focus_callback)                          = 0;
			virtual void subscribe_on_char_input(std::function<void(void*, uint)> char_callback)                             = 0;

			virtual enum mouse_button  map_mouse_button(int button)  = 0;
			virtual enum key_button    map_key_button(int button)    = 0;
			virtual enum button_status map_button_action(int action) = 0;
			virtual int                map_button_mods(int mods)     = 0;

			virtual void update() = 0;

			virtual float context_time() const          = 0;
			virtual int   real_height() const           = 0;
			virtual int   real_width() const            = 0;
			virtual int   height() const                = 0;
			virtual int   width() const                 = 0;
			virtual bool  is_key_pressed(int key) const = 0;
			virtual bool  should_close() const          = 0;

			virtual void* internal() = 0;
			virtual void* internal() const = 0;
	};

	class glfw_window : public window {
		public:
			glfw_window();
			~glfw_window() override;

			void hide_cursor() override;
			void show_cursor() override;

			void subsribe_on_mouse_pos_change(std::function<void(void*, double, double)> mouse_callback) override;
			void subscribe_on_scroll_change(std::function<void(void*, double, double)> scroll_callback) override;
			void subscribe_on_mouse_click(
			    std::function<void(void*, enum mouse_button, enum button_status)> mouse_click_callback) override;
			void subsribe_on_keyboard_click(std::function<void(void*)> keyboard_click_callback) override;
			void subscribe_on_focus_changed(std::function<void(void*, int)> focus_callback) override;
			void subscribe_on_char_input(std::function<void(void*, uint)> char_callback) override;

			enum mouse_button  map_mouse_button(int button) override;
			enum key_button    map_key_button(int button) override;
			enum button_status map_button_action(int action) override;
			int                map_button_mods(int mods) override;

			void update() override;

			float context_time() const override;
			int   real_height() const override;
			int   real_width() const override;
			int   height() const override;
			int   width() const override;
			bool  is_key_pressed(int key) const override;
			bool  should_close() const override;

			void* internal() override { return window_; }
			void* internal() const override { return window_; }

		private:
			GLFWwindow*               window_ = nullptr;
			int                       width_{};
			int                       height_{};
			int                       real_width_{};
			int                       real_height_{};
			std::unique_ptr<renderer> renderer_;

			std::map<int, enum mouse_button>   mouse_buttons_map_;
			std::map<int, enum key_button>     key_buttons_map_;
			std::map<int, enum button_status>  button_status_map_;
			std::map<int, enum key_button_mod> key_mod_map_;

			std::function<void(void*)> keyboard_click_callback_;
	};
} // namespace kanso
