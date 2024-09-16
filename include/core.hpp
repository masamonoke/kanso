#pragma once

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
		KANSO_KEYBOARD_BUTTON_E
	};

	enum key_button_mod {
		KANSO_KEYBOARD_MOD_SHIFT = 0x0001,
		KANSO_KEYBOARD_MOD_CTRL  = 0x0002,
		KANSO_KEYBOARD_MOD_ALT   = 0x0004
	};

	enum mouse_button { KANSO_MOUSE_BUTTON_LEFT, KANSO_MOUSE_BUTTON_RIGHT };

	enum button_status { KANSO_STATUS_PRESS, KANSO_STATUS_RELEASE };
} // namespace kanso
