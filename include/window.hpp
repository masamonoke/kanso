#pragma once

#include <memory>
#include <functional>
#include <map>

#include "GLFW/glfw3.h"
#include "core.hpp"

namespace kanso {

	class window {
		public:
			window()          = default;
			virtual ~window() = default;

			virtual void hide_cursor() = 0;
			// TODO: probably rename to show_cursor
			virtual void show_cursor() = 0;

			virtual void subsribe_on_mouse_pos_change(std::function<void(void*, double, double)> mouse_callback) = 0;
			virtual void subscribe_on_scroll_change(std::function<void(void*, double, double)> scroll_callback)  = 0;
			virtual void subscribe_on_mouse_click(
			    std::function<void(void*, enum mouse_button, enum button_status)> mouse_click_callback) = 0;
			virtual void subsribe_on_keyboard_click(std::function<void(void*)> keyboard_click_callback) = 0;

			virtual enum mouse_button  map_mouse_button(int button)  = 0;
			virtual enum key_button    map_key_button(int button)    = 0;
			virtual enum button_status map_button_action(int action) = 0;
			virtual int                map_button_mods(int mods)     = 0;

			virtual void update() = 0;

			[[nodiscard]] virtual float context_time() const          = 0;
			[[nodiscard]] virtual int   height() const                = 0;
			[[nodiscard]] virtual int   width() const                 = 0;
			[[nodiscard]] virtual bool  is_key_pressed(int key) const = 0;
			[[nodiscard]] virtual bool  should_close() const          = 0;
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

			enum mouse_button  map_mouse_button(int button) override;
			enum key_button    map_key_button(int button) override;
			enum button_status map_button_action(int action) override;
			int                map_button_mods(int mods) override;

			void update() override;

			[[nodiscard]] float context_time() const override;
			[[nodiscard]] int   height() const override;
			[[nodiscard]] int   width() const override;
			[[nodiscard]] bool  is_key_pressed(int key) const override;
			[[nodiscard]] bool  should_close() const override;

		private:
			GLFWwindow* window_ = nullptr;

			std::map<int, enum mouse_button>   mouse_buttons_map_;
			std::map<int, enum key_button>     key_buttons_map_;
			std::map<int, enum button_status>  button_status_map_;
			std::map<int, enum key_button_mod> key_mod_map_;

			std::function<void(void*)> keyboard_click_callback_;
	};
} // namespace kanso
