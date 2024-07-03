#pragma once

#include <cglm/types.h>

#define MAX_FOV 150.0f
#define MIN_FOV 90.0f

struct camera {
	vec3 pos;
	vec3 front;
	vec3 up;
	float fov;
};

typedef struct camera camera_t;

__attribute__((warn_unused_result))
const camera_t* camera_get(void);

__attribute__((nonnull(1)))
void camera_view(mat4 view);

__attribute__((nonnull(1)))
void camera_projection(mat4 projection);

void camera_set_fov(float fov);

__attribute__((nonnull(1)))
void camera_set_front(float* front);

__attribute__((warn_unused_result))
float camera_fov(void);

__attribute__((warn_unused_result))
const float* camera_pos(void);

__attribute__((warn_unused_result))
const float* camera_front(void);

void camera_move_front(float camera_speed);

void camera_move_back(float camera_speed);

void camera_move_left(float camera_speed);

void camera_move_right(float camera_speed);

void camera_move_up(float camera_speed);

void camera_move_down(float camera_speed);

void camera_change_fov(float offset);

void camera_change_view(float x, float y);
