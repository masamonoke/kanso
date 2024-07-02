#include "control.h"

#include <stdlib.h>
#include <assert.h>

#include <GLFW/glfw3.h>
#include <c_log.h>

#include "window.h"
#include "camera.h"

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d);

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset);

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void control_init(void* ctx) {
	window_t* window;

	assert(ctx != NULL);

	window = (window_t*) ctx;

	void (*abstract_mouse_callback) (window_t* window, double xpos_d, double ypos_d);
	void (*abstract_scroll_callback) (window_t*, double, double);
	void (*abstract_mouse_click_callback) (window_t* window, int button, int action, int mods);

	abstract_mouse_callback = (void (*)(window_t*, double, double)) mouse_callback;
	window_set_cursor(window, abstract_mouse_callback);

	abstract_scroll_callback = (void (*)(window_t*, double, double)) change_fov_callback;
	window_set_scroll(window, abstract_scroll_callback);

	abstract_mouse_click_callback = (void (*)(window_t*, int, int, int)) mouse_button_callback;
	window_set_mouse_click(window, abstract_mouse_click_callback);
}

void control_update(void* ctx) {
#ifdef EDITOR_MODE
	// there should be game specific settings
#else
	static float last_frame = 0;
	float cur_frame;
	float delta_time;
	float camera_speed;
	GLFWwindow* window;

	assert(ctx != NULL);

	window = (GLFWwindow*) ctx;

	cur_frame = window_context_time();
	delta_time = cur_frame - last_frame;
	last_frame= cur_frame;
	camera_speed = delta_time * 2.5f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_move_front(camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_move_back(camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_move_left(camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_move_right(camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera_move_up(camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera_move_down(camera_speed);
	}
#endif
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
#ifdef EDITOR_MODE
	// there should be game specific settings
#else
	(void) mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		log_warn("Mouse pos: %f, %f", xpos, ypos);
	}
#endif
}

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset) {
#ifdef EDITOR_MODE
	// there should be game specific settings
#else
	(void) window;
	(void) xoffset;

	camera_change_fov((float) yoffset);
#endif
}

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d) {
#ifdef EDITOR_MODE
	// there should be game specific settings
#else
	(void) window;
	camera_change_view((float) xpos_d, (float) ypos_d);
#endif
}
