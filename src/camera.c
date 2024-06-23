#include "camera.h"

#include <string.h>
#include <assert.h>

#include <cglm/cam.h>
#include <cglm/types.h>
#include <cglm/vec3.h>

#define CAMERA_INIT { .pos = { 0.0f, 0.0f, 4.0f }, .front = { 0.0f, 0.0f, -1.0f }, .up = { 0.0f, 1.0f, 0.0f }, .fov = MIN_FOV }

static struct camera camera = CAMERA_INIT;

// window implementation specific functions
extern void camera_mouse_callback(window_t* window, double xpos_d, double ypos_d);
extern void camera_change_fov_callback(window_t* window, double xoffset, double yoffset);

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
