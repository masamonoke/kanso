#include "camera.h"

#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include <cglm/cam.h>
#include <cglm/types.h>
#include <cglm/vec3.h>
#include "cglm/util.h"

#define CAMERA_INIT { .pos = { 0.0f, 0.0f, 4.0f }, .front = { 0.0f, 0.0f, -1.0f }, .up = { 0.0f, 1.0f, 0.0f }, .fov = MIN_FOV }

static struct camera camera = CAMERA_INIT;

void camera_move_front(float camera_speed) {
	vec3 tmp;
	memcpy(tmp, camera.front, sizeof(float) * 3);
	tmp[0] *= camera_speed;
	tmp[1] *= camera_speed;
	tmp[2] *= camera_speed;
	glm_vec3_add(camera.pos, tmp, camera.pos);
}

void camera_move_back(float camera_speed) {
	vec3 tmp;
	memcpy(tmp, camera.front, sizeof(float) * 3);
	tmp[0] *= camera_speed;
	tmp[1] *= camera_speed;
	tmp[2] *= camera_speed;
	glm_vec3_sub(camera.pos, tmp, camera.pos);
}

void camera_move_left(float camera_speed) {
	vec3 cross;

	glm_cross(camera.front, camera.up, cross);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_sub(camera.pos, cross, camera.pos);
}

void camera_move_right(float camera_speed) {
	vec3 cross;

	glm_cross(camera.front, camera.up, cross);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_add(camera.pos, cross, camera.pos);
}

void camera_move_up(float camera_speed) {
	vec3 cross;

	memcpy(cross, camera.up, sizeof(float) * 3);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_add(camera.pos, cross, camera.pos);
}

void camera_move_down(float camera_speed) {
	vec3 cross;

	memcpy(cross, camera.up, sizeof(float) * 3);
	glm_normalize(cross);

	cross[0] *= camera_speed;
	cross[1] *= camera_speed;
	cross[2] *= camera_speed;

	glm_vec3_sub(camera.pos, cross, camera.pos);
}

const float* camera_pos(void) {
	return camera.pos;
}

const float* camera_front(void) {
	return camera.front;
}

void camera_set_view(mat4 view) {
	vec3 direction;

	assert(view != NULL);

	glm_vec3_add(camera.pos, camera.front, direction);
	glm_lookat(camera.pos, direction, camera.up, view);
}

const camera_t* camera_get(void) {
	return &camera;
}

float camera_fov(void) {
	return camera.fov;
}

void camera_set_fov(float fov) {
	camera.fov = fov;
}

void camera_set_front(float* front) {
	assert(front != NULL);
	memcpy(camera.front, front, sizeof(float) * 3);
}

void camera_change_fov(float offset) {
	camera.fov -= offset;
	camera.fov = glm_clamp(camera.fov, MIN_FOV, MAX_FOV);
}

void camera_change_view(float x, float y) { // NOLINT(*bugprone-easily-swappable-parameters)
	float xoffset;
	float yoffset;
	vec3 front;
	const float SENSITIVITY = 0.1f;
	static bool first_time = true;
	static float last_x;
	static float last_y;
	static float yaw = -90.0f;
	static float pitch = 0.0f;

	if (first_time) {
		last_x = x;
		last_y = y;
		first_time = false;
	}

	xoffset = (x - last_x) * SENSITIVITY;
	yoffset = (last_y - y) * SENSITIVITY; // y coord goes from bottom to top

	last_x = x;
	last_y = y;

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
