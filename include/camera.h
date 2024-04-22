#pragma once

#include <cglm/types.h>

#include "glfw_context.h"

typedef struct camera camera_t;

void camera_init_glfw_callbacks(GLFWwindow* window);

camera_t* camera_get(void);

void camera_set_view(mat4 view);

void camera_update(GLFWwindow* window);

float camera_fov(void);

const float* camera_pos(void);

const float* camera_front(void);
