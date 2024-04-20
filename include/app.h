#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vertex_object.h"
#include "camera.h"

typedef struct app_state app_state_t;

struct app_state {
	GLFWwindow* window;
	vertex_object_t vo;
	vertex_object_t light_vo;
	uint32_t shader_program;
	uint32_t light_shader_program;
	uint32_t texture;
	camera_t* camera;
	bool close;
	void (*update) (app_state_t* state);
	void (*input) (app_state_t* state);
	void (*free) (app_state_t* state);
};

int32_t app_new(app_state_t** state);
