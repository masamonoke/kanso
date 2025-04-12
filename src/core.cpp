#include "core.hpp"

#include "GLFW/glfw3.h"

#include <string>
#include <random>
#include <sstream>

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
			{ KANSO_KEYBOARD_BUTTON_F1, GLFW_KEY_F1 },
			{ KANSO_KEYBOARD_BUTTON_F5, GLFW_KEY_F5 }
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

	namespace uuid {
		std::string generate_id() {
			std::random_device              rd;
			std::mt19937                    gen(rd());
			std::uniform_int_distribution<> dis(0, 15);
			std::uniform_int_distribution<> dis2(8, 11);

			std::stringstream ss;
			int i = 0;

			ss << std::hex;
			for (i = 0; i < 8; i++) {
				ss << dis(gen);
			}
			ss << "-";
			for (i = 0; i < 4; i++) {
				ss << dis(gen);
			}
			ss << "-4";
			for (i = 0; i < 3; i++) {
				ss << dis(gen);
			}
			ss << "-";
			ss << dis2(gen);
			for (i = 0; i < 3; i++) {
				ss << dis(gen);
			}
			ss << "-";
			for (i = 0; i < 12; i++) {
				ss << dis(gen);
			};
			return ss.str();
		}
	}
}
