#include "core.hpp"

#include "GLFW/glfw3.h"

namespace kanso {
	std::map<enum mouse_button, int> mapped_mouse_buttons() {
#ifdef GLFWAPI
		return {
			{ KANSO_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_LEFT },
			{ KANSO_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_RIGHT }
		};
#else
		return {};
#endif
	}

	std::map<enum key_button, int> mapped_keys() {
#ifdef GLFWAPI
		return {
			{ KANSO_KEYBOARD_BUTTON_W,  GLFW_KEY_W },
			{ KANSO_KEYBOARD_BUTTON_S,  GLFW_KEY_S },
			{ KANSO_KEYBOARD_BUTTON_D,  GLFW_KEY_D },
			{ KANSO_KEYBOARD_BUTTON_A,  GLFW_KEY_A },
			{ KANSO_KEYBOARD_BUTTON_Q,  GLFW_KEY_Q },
			{ KANSO_KEYBOARD_BUTTON_E,  GLFW_KEY_E },
			{ KANSO_KEYBOARD_BUTTON_F1, GLFW_KEY_F1 }
		};
#else
		return {};
#endif
	}

	std::map<enum button_status, int> mapped_actions() {
#ifdef GLFWAPI
		return {
			{ KANSO_STATUS_PRESS, GLFW_PRESS },
			{ KANSO_STATUS_RELEASE, GLFW_RELEASE },
		};
#else
		return {};
#endif

	}
}
