#include "control.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>
#include <cglm/types.h>

#include "window.h"
#include "camera.h"
#include "raycast.h"

static world_t* cur_world;

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d);

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset);

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void control_init(void* window_ctx, world_t* world) {
	window_t* window;

	assert(window_ctx != NULL);

	window = (window_t*) window_ctx;

	void (*abstract_mouse_callback) (window_t* window, double xpos_d, double ypos_d);
	void (*abstract_scroll_callback) (window_t*, double, double);
	void (*abstract_mouse_click_callback) (window_t* window, int button, int action, int mods);

	abstract_mouse_callback = (void (*)(window_t*, double, double)) mouse_callback;
	window_set_cursor(window, abstract_mouse_callback);

	abstract_scroll_callback = (void (*)(window_t*, double, double)) change_fov_callback;
	window_set_scroll(window, abstract_scroll_callback);

	abstract_mouse_click_callback = (void (*)(window_t*, int, int, int)) mouse_button_callback;
	window_set_mouse_click(window, abstract_mouse_click_callback);

	cur_world = world;
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
		mat4 proj;
		mat4 view;

		glfwGetCursorPos(window, &xpos, &ypos);
		camera_projection(proj);
		camera_view(view);
		raycast_viewport_model(cur_world, (float) xpos, (float) ypos, proj, view);
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

struct mouse_pos {
	float x;
	float y;
};

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d) {
#ifdef EDITOR_MODE
	// there should be game specific settings
#else
	static bool cursor_set = false;
	static struct mouse_pos camera_point = {
		.x = -1.0f,
		.y = -1.0f
	};
	static struct mouse_pos prev_point = {
		.x = -1.0f,
		.y = -1.0f
	};

	(void) window;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		float x_shift;
		float y_shift;

		if (!cursor_set) {
			window_hide_cursor((window_t*) window);
			cursor_set = true;
		}

		if (prev_point.x == -1.0f) {
			prev_point.x = (float) xpos_d;
			prev_point.y = (float) ypos_d;
		}

		x_shift = prev_point.x - (float) xpos_d;
		y_shift = prev_point.y - (float) ypos_d;

		camera_point.x -= x_shift;
		camera_point.y -= y_shift;
		camera_change_view(camera_point.x, camera_point.y);

		prev_point.x = (float) xpos_d;
		prev_point.y = (float) ypos_d;
		return;
	}

	if (cursor_set) {
		cursor_set = false;
		window_reset_cursor((window_t*) window);
	}

	prev_point.x = -1.0f;
	prev_point.y = -1.0f;

#endif
}
