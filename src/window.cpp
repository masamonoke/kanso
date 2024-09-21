#include "glad/glad.h"
#include "window.hpp"
#include "core.hpp"
#include "renderer.hpp"

#include <iostream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace kanso {

	void parse_json(int& framebuf_width, int& framebuf_height, std::string& title);

	glfw_window::glfw_window() {
		auto err_callback = [](int code, const char* err_str) {
			std::cerr << "GLFW error: (" << code << "): " << err_str << std::endl;
		};

		glfwSetErrorCallback(err_callback);

		// valgrind and address sanitizer may show gpu memory leak after this function
		// and it not freed after glfwTerminate() call
		// but that is not glfw problem or anything else but GPU drivers (libraries outside glfw control)
		// like to allocate some global memory and keep it allocated even when GL/Vulkan context is destroyed.
		// They have some globals/statics that are never released.
		// So if valgrind shows memory leaks coming from them, this is to be expected.
		if (glfwInit() == 0) {
			throw std::runtime_error("GLFW failed to initialize");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		int         framebuf_width  = 0;
		int         framebuf_height = 0;
		std::string title;

		parse_json(framebuf_width, framebuf_height, title);

		window_ = glfwCreateWindow(framebuf_width, framebuf_height, title.c_str(), nullptr, nullptr);

		if (window_ == nullptr) {
			glfwTerminate();
			throw std::runtime_error("GLFW failed to create window");
		}

		glfwMakeContextCurrent(window_);

		if (0 == gladLoadGLLoader(reinterpret_cast<GLADloadproc>( // NOLINT cppcoreguidelines-pro-type-reinterpret-cast
		             glfwGetProcAddress))) {
			glfwTerminate();
			throw std::runtime_error("Failed to initialize GLAD");
		}

		renderer::enable_depth();
		renderer::enable_stencil(0xff);

		glfwGetFramebufferSize(window_, &framebuf_width, &framebuf_height);
		renderer::set_viewport(framebuf_width, framebuf_height);

		auto framebuffer_size_callback = [](GLFWwindow* window, int width, int height) {
			(void)window;
			renderer::set_viewport(width, height);
		};

		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

		mouse_buttons_map_[GLFW_MOUSE_BUTTON_LEFT]  = KANSO_MOUSE_BUTTON_LEFT;
		mouse_buttons_map_[GLFW_MOUSE_BUTTON_RIGHT] = KANSO_MOUSE_BUTTON_RIGHT;

		key_buttons_map_[GLFW_KEY_W] = KANSO_KEYBOARD_BUTTON_W;
		key_buttons_map_[GLFW_KEY_S] = KANSO_KEYBOARD_BUTTON_S;
		key_buttons_map_[GLFW_KEY_D] = KANSO_KEYBOARD_BUTTON_D;
		key_buttons_map_[GLFW_KEY_A] = KANSO_KEYBOARD_BUTTON_A;
		key_buttons_map_[GLFW_KEY_Q] = KANSO_KEYBOARD_BUTTON_Q;
		key_buttons_map_[GLFW_KEY_E] = KANSO_KEYBOARD_BUTTON_E;

		button_status_map_[GLFW_PRESS]   = KANSO_STATUS_PRESS;
		button_status_map_[GLFW_RELEASE] = KANSO_STATUS_RELEASE;

		key_mod_map_[GLFW_MOD_SHIFT]   = KANSO_KEYBOARD_MOD_SHIFT;
		key_mod_map_[GLFW_MOD_ALT]     = KANSO_KEYBOARD_MOD_ALT;
		key_mod_map_[GLFW_MOD_CONTROL] = KANSO_KEYBOARD_MOD_CTRL;

		spdlog::info("Created GLFW window ({}x{}) '{}'", framebuf_width, framebuf_height, title);
	}

	glfw_window::~glfw_window() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void glfw_window::hide_cursor() {
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void glfw_window::show_cursor() {
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	enum mouse_button glfw_window::map_mouse_button(int button) {
		return mouse_buttons_map_[button];
	}

	enum key_button glfw_window::map_key_button(int button) {
		return key_buttons_map_[button];
	}

	enum button_status glfw_window::map_button_action(int action) {
		return button_status_map_[action];
	}

	int glfw_window::map_button_mods(int mods) {
		int res = 0;
		if ((mods & GLFW_MOD_SHIFT) != 0) {
			res |= KANSO_KEYBOARD_MOD_SHIFT;
		}
		if ((mods & GLFW_MOD_ALT) != 0) {
			res |= KANSO_KEYBOARD_MOD_ALT;
		}
		if ((mods & GLFW_MOD_CONTROL) != 0) {
			res |= KANSO_KEYBOARD_MOD_CTRL;
		}
		return res;
	}

	void glfw_window::subsribe_on_mouse_pos_change(std::function<void(void*, double, double)> mouse_callback) {
		static std::function<void(void*, double, double)> callback;

		callback = mouse_callback;
		auto foo = [](GLFWwindow* window, double xpos, double ypos) { callback(window, xpos, ypos); };
		glfwSetCursorPosCallback(window_, foo);
		spdlog::debug("Registered mouse pos callback");
	}

	void glfw_window::subscribe_on_scroll_change(std::function<void(void*, double, double)> scroll_callback) {
		static std::function<void(void*, double, double)> callback;

		callback = scroll_callback;
		auto foo = [](GLFWwindow* window, double xpos, double ypos) { callback(window, xpos, ypos); };
		glfwSetScrollCallback(window_, foo);
		spdlog::debug("Registered mouse scroll callback");
	}

	void glfw_window::subscribe_on_mouse_click(
	    std::function<void(void*, enum mouse_button, enum button_status)> mouse_click_callback) {
		static std::function<void(GLFWwindow*, int, int)> callback;
		auto foo = [this, mouse_click_callback](GLFWwindow* window, int button, int action) {
			auto b = map_mouse_button(button);
			auto a = map_button_action(action);
			mouse_click_callback(window, b, a);
		};

		callback           = foo;
		auto glfw_callback = [](GLFWwindow* window, int button, int action, int mods) {
			(void)mods;
			callback(window, button, action);
		};
		glfwSetMouseButtonCallback(window_, glfw_callback);
		spdlog::debug("Registered mouse click callback");
	}

	void glfw_window::subsribe_on_keyboard_click(std::function<void(void*)> keyboard_click_callback) {
		keyboard_click_callback_ = keyboard_click_callback;
		spdlog::debug("Registered mouse scroll callback");
	}

	void glfw_window::update() {
		glfwSwapBuffers(window_);
		glfwPollEvents();
		keyboard_click_callback_(window_);
	}

	float glfw_window::context_time() const {
		return static_cast<float>(glfwGetTime());
	}

	int glfw_window::height() const {
		const auto* ret = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return ret->height;
	}

	int glfw_window::width() const {
		const auto* ret = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return ret->width;
	}

	bool glfw_window::is_key_pressed(int key) const {
		return glfwGetKey(window_, key) == GLFW_PRESS;
	}

	void parse_json(int& framebuf_width, int& framebuf_height, std::string& title) {
		try {
			std::ifstream stream(DEFAULT_CONFIG_PATH);
			auto          js_config = nlohmann::json::parse(stream);

			try {
				framebuf_width = js_config["width"];
			} catch (nlohmann::json::type_error& e) {
				spdlog::error("Failed to read width value from config {}", DEFAULT_CONFIG_PATH);
				framebuf_width = DEFAULT_WINDOW_WIDTH;
			}

			try {
				framebuf_height = js_config["height"];
			} catch (nlohmann::json::type_error& e) {
				spdlog::error("Failed to read height value from config", DEFAULT_CONFIG_PATH);
				framebuf_height = DEFAULT_WINDOW_HEIGHT;
			}

			try {
				title = js_config["title"];
			} catch (nlohmann::json::type_error& e) {
				spdlog::error("Failed to read title value from config", DEFAULT_CONFIG_PATH);
				title = DEFAULT_WINDOW_TITLE;
			}

		} catch (nlohmann::json::parse_error& e) {
			spdlog::error("Failed to parse config file: {}", e.what());
			framebuf_height = DEFAULT_WINDOW_HEIGHT;
			framebuf_width  = DEFAULT_WINDOW_WIDTH;
			title           = DEFAULT_WINDOW_TITLE;
		}
	}

	bool glfw_window::should_close() const {
		return is_key_pressed(GLFW_KEY_ESCAPE);
	}
} // namespace kanso
