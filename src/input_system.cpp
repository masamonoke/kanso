#include "input_system.hpp"
#include "spdlog/spdlog.h"

namespace kanso {

	void input_system::handle_input() {
		for (const auto& kv : funcs_) {
			kv.second();
		}
	}

	const int glfw_input::double_press_countdown_offset = 500;

	glfw_input::glfw_input(GLFWwindow* window, int double_press_timeout_ms)
		: input_system(),
		  window_(window),
		  keys_(mapped_keys()),
		  double_press_timeout_ms_(double_press_timeout_ms) {}

	void glfw_input::bind_key(enum key_button key, enum key_button_mod /*mod*/, enum press_type press,
							  const std::function<void(void)>& f) {
		assert(keys_.contains(key)); // NOLINT
		const int glfw_key = keys_[key];
		std::function<void(void)> handler;

		if (press == press_type::PRESS) {
			handler = make_press_func(glfw_key, f);
		} else if (press == press_type::SINGLE_PRESS) {
			handler = make_single_press_func(glfw_key, f);
		} else {
			handler = make_double_press_func(glfw_key, f);
		}

		key_press_map_[glfw_key] = KANSO_STATUS_UNDEFINED;

		funcs_[{key, press}] = handler;
	}

	std::function<void(void)> glfw_input::make_press_func(int glfw_key, const std::function<void(void)>& f) {
		return [this, f, glfw_key]() {
			if (glfwGetKey(window_, glfw_key) == GLFW_PRESS) {
				f();
			}
		};
	}

	std::function<void(void)> glfw_input::make_single_press_func(int glfw_key, const std::function<void(void)>& f) {
		return [this, f, glfw_key]() {
			if (!key_press_map_.contains(glfw_key)) {
				return;
			}

			auto glfw_status = glfwGetKey(window_, glfw_key);
			auto status = from_impl_to_kanso(glfw_status);

			if (status == KANSO_STATUS_PRESS) {
				key_press_map_[glfw_key] = KANSO_STATUS_PRESS;
			}

			if (status == KANSO_STATUS_RELEASE && key_press_map_[glfw_key] == KANSO_STATUS_PRESS) {
				key_press_map_[glfw_key] = KANSO_STATUS_RELEASE;
				f();
			}
		};
	}

	void glfw_input::countdown(int glfw_key, int ms) {
		auto start_time = std::chrono::steady_clock::now();

		for (int i = ms; i >= 0; --i) {
			auto next_tick = start_time + std::chrono::milliseconds(ms - i + 1);
			std::this_thread::sleep_until(next_tick);
		}

		const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		const auto passed = now - double_key_press_map_[glfw_key].unix_timestamp;
		if (passed > double_press_timeout_ms_ + double_press_countdown_offset) {
			spdlog::debug("countdown finished");
			double_key_press_map_[glfw_key].press_count = 0;
			double_key_press_map_[glfw_key].unix_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
		}
	}

	enum button_status glfw_input::from_impl_to_kanso(int status) {
		static std::map<int, enum button_status> map {
			{ GLFW_PRESS, KANSO_STATUS_PRESS },
			{ GLFW_RELEASE, KANSO_STATUS_RELEASE },
		};

		if (map.contains(status)) {
			return map[status];
		}

		return KANSO_STATUS_UNDEFINED;
	}

	int glfw_input::from_kanso_to_impl(enum button_status status) {
		static std::map<enum button_status, int> map {
			{ KANSO_STATUS_PRESS, GLFW_PRESS },
			{ KANSO_STATUS_RELEASE, GLFW_RELEASE }
		};

		if (map.contains(status)) {
			return map[status];
		}

		return -1;
	}


	std::function<void(void)> glfw_input::make_double_press_func(int glfw_key, const std::function<void(void)>& f) {
		auto double_press = [this, glfw_key, f]() {
			if (double_key_press_map_.contains(glfw_key)) {
				if (double_key_press_map_[glfw_key].press_count == 1) {
					const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					const auto passed = now - double_key_press_map_[glfw_key].unix_timestamp;
					// TODO: why no offset
					if (passed < double_press_timeout_ms_) {
						spdlog::debug("double press recognized");
						f();
					}
					double_key_press_map_[glfw_key].press_count = 0;
				}
				else {
					double_key_press_map_[glfw_key].press_count = 1;
					double_key_press_map_[glfw_key].unix_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					double_key_press_map_[glfw_key].future = std::async(std::launch::async, &glfw_input::countdown, this, glfw_key, double_press_timeout_ms_ + double_press_countdown_offset);
				}
			}
			else
			{
				double_key_press_map_[glfw_key] = {
					1,
					std::chrono::system_clock::now().time_since_epoch().count(),
					std::async(std::launch::async, &glfw_input::countdown, this, glfw_key, double_press_timeout_ms_ + double_press_countdown_offset)
				};
			}
		};

		return [this, glfw_key, double_press]() {
			static bool pressed = false;
			if (glfwGetKey(window_, glfw_key) == GLFW_PRESS) {
				pressed = true;
			}
			if (glfwGetKey(window_, glfw_key) == GLFW_RELEASE && pressed) {
				pressed = false;
				double_press();
			}
		};
	}
}
