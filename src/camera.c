#include <stdbool.h>
#include <memory.h>

#include <cglm/cglm.h>

#include "camera.h"

struct camera {
	float pos[3];
	float front[3];
	float up[3];
	float fov;
};

#define CAMERA_INIT { .pos = { 0.0f, 0.0f, 4.0f }, .front = { 0.0f, 0.0f, -1.0f }, .up = { 0.0f, 1.0f, 0.0f }, .fov = MIN_FOV }
#define MAX_FOV 150.0f
#define MIN_FOV 90.0f

static struct camera camera = CAMERA_INIT;

static void move_front(camera_t* camera, float camera_speed) {
	float tmp[3];
	memcpy(tmp, camera->front, sizeof(float) * 3);
	tmp[0] *= camera_speed;
	tmp[1] *= camera_speed;
	tmp[2] *= camera_speed;
	glm_vec3_add(camera->pos, tmp, camera->pos);
}

static void move_back(camera_t* camera, float camera_speed) {
	float tmp[3];
	memcpy(tmp, camera->front, sizeof(float) * 3);
	tmp[0] *= camera_speed;
	tmp[1] *= camera_speed;
	tmp[2] *= camera_speed;
	glm_vec3_sub(camera->pos, tmp, camera->pos);
}

static void move_left(camera_t* camera, float camera_speed) {
	float cross[3];

	glm_cross(camera->front, camera->up, cross);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_sub(camera->pos, cross, camera->pos);
}

static void move_right(camera_t* camera, float camera_speed) {
	float cross[3];

	glm_cross(camera->front, camera->up, cross);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_add(camera->pos, cross, camera->pos);
}

static void move_up(camera_t* camera, float camera_speed) {
	float cross[3];

	memcpy(cross, camera->up, sizeof(float) * 3);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_add(camera->pos, cross, camera->pos);
}

static void move_down(camera_t* camera, float camera_speed) {
	float cross[3];

	memcpy(cross, camera->up, sizeof(float) * 3);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_sub(camera->pos, cross, camera->pos);
}

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
		move_front(&camera, camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		move_back(&camera, camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		move_left(&camera, camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		move_right(&camera, camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		move_up(&camera, camera_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		move_down(&camera, camera_speed);
	}
}

void camera_update(GLFWwindow* window) {
	input(window);
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
	memcpy(camera.front, front, sizeof(float) * 3);
}

static void change_fov_callback(GLFWwindow* window, double xoffset, double yoffset) {
	(void) window;
	(void) xoffset;

	camera.fov -= (float) yoffset;
	camera.fov = glm_clamp(camera.fov, MIN_FOV, MAX_FOV);
}

void camera_set_view(mat4 view) {
	float direction[3];

	glm_vec3_add(camera.pos, camera.front, direction);
	glm_lookat(camera.pos, direction, camera.up, view);
}

camera_t* camera_get(void) {
	return &camera;
}

float camera_fov(void) {
	return camera.fov;
}

void camera_init_glfw_callbacks(GLFWwindow* window) {
	glfw_context_set_cursor_pos_callback(window, mouse_callback);
	glfw_context_set_scroll_callback(window, change_fov_callback);
}

const float* camera_pos(void) {
	return camera.pos;
}
