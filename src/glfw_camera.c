#include "glfw_context.h"
#include "camera.h"

#include <cglm/cglm.h>

#include <math.h>

static void input(GLFWwindow* window);

void camera_update(window_t* window) {
	input((GLFWwindow*) window);
}

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d);

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset);

void camera_init_callbacks(window_t* window) {
	void (*abstract_mouse_callback) (window_t* window, double xpos_d, double ypos_d);
	void (*abstract_scroll_callback)(window_t*, double, double);

	abstract_mouse_callback = (void (*)(window_t*, double, double)) mouse_callback;
	window_set_cursor(window, abstract_mouse_callback);
	abstract_scroll_callback = (void (*)(window_t*, double, double)) change_fov_callback;
	window_set_scroll(window, abstract_scroll_callback);
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

static void input(GLFWwindow* window) {
	static float last_frame = 0;
	float cur_frame;
	float delta_time;
	float camera_speed;

	cur_frame = glfw_context_time();
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
}

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset) {
	const camera_t* camera;
	float fov;

	(void) window;
	(void) xoffset;

	camera = camera_get();

	fov = camera->fov;
	fov -= (float) yoffset;
	fov = glm_clamp(fov, MIN_FOV, MAX_FOV);
	camera_set_fov(fov);
}

static void mouse_callback(GLFWwindow* window, double xpos_d, double ypos_d) {
	float xpos;
	float ypos;
	float xoffset;
	float yoffset;
	float front[3];
	const float SENSITIVITY = 0.1f;
	static bool first_time = true;
	static float last_x;
	static float last_y;
	static float yaw = -90.0f;
	static float pitch = 0.0f;

	(void) window;

	xpos = (float) xpos_d;
	ypos = (float) ypos_d;

	if (first_time) {
		last_x = xpos;
		last_y = ypos;
		first_time = false;
	}

	xoffset = (xpos - last_x) * SENSITIVITY;
	yoffset = (last_y - ypos) * SENSITIVITY; // y coord goes from bottom to top

	last_x = xpos;
	last_y = ypos;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	front[0] = cosf(glm_rad(yaw)) * cosf(glm_rad(pitch)); // x
	front[1] = sinf(glm_rad(pitch));					  // y
	front[2] = sinf(glm_rad(yaw)) * cosf(glm_rad(pitch)); // z

	glm_normalize(front);
	camera_set_front(front);
}
