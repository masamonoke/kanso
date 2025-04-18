#pragma once

#include <map>

namespace kanso {
	constexpr char const* DEFAULT_SCENE_PATH    = "scene/default_scene.json";
	constexpr int         DEFAULT_WINDOW_WIDTH  = 800;
	constexpr int         DEFAULT_WINDOW_HEIGHT = 600;
	constexpr char const* DEFAULT_WINDOW_TITLE  = "Kanso Engine";
	constexpr char const* DEFAULT_CONFIG_PATH   = "cfg/window.json";

	using uint = unsigned int;

	enum key_button {
		KANSO_KEYBOARD_BUTTON_W,
		KANSO_KEYBOARD_BUTTON_S,
		KANSO_KEYBOARD_BUTTON_A,
		KANSO_KEYBOARD_BUTTON_D,
		KANSO_KEYBOARD_BUTTON_Q,
		KANSO_KEYBOARD_BUTTON_E,
		KANSO_KEYBOARD_BUTTON_F1,
		KANSO_KEYBOARD_BUTTON_F5
	};

	enum key_button_mod {
		KANSO_KEYBOARD_MOD_NONE  = 0x0,
		KANSO_KEYBOARD_MOD_SHIFT = 0x0001,
		KANSO_KEYBOARD_MOD_CTRL  = 0x0002,
		KANSO_KEYBOARD_MOD_ALT   = 0x0004
	};

	enum press_type { PRESS, SINGLE_PRESS, DOUBLE_PRESS };

	enum mouse_button { KANSO_MOUSE_BUTTON_LEFT, KANSO_MOUSE_BUTTON_RIGHT };

	enum button_status { KANSO_STATUS_PRESS, KANSO_STATUS_RELEASE, KANSO_STATUS_UNDEFINED };

	std::map<enum mouse_button, int>  mapped_mouse_buttons();
	std::map<enum key_button, int>    mapped_keys();
	std::map<enum button_status, int> mapped_actions();
	std::map<enum press_type, int>    mapped_press();

	namespace uuid {
		std::string generate_id();
	}

} // namespace kanso
