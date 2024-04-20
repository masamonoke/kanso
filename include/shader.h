#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <cglm/types.h>

struct transform {
	mat4 model;
	mat4 view;
	mat4 proj;
};

#define TRANSFORM_IDENTITY { .model = GLM_MAT4_IDENTITY_INIT, .view = GLM_MAT4_IDENTITY_INIT, .proj = GLM_MAT4_IDENTITY_INIT }

uint32_t shader_create_program(const char* vertex_shader, const char* fragment_shader);

void shader_set_bool(uint32_t shader_program, const char* name, bool value);

void shader_set_int(uint32_t shader_program, const char* name, int32_t value);

void shader_set_float(uint32_t shader_program, const char* name, float value);

void shader_set_mat4(uint32_t shader_program, const char* name, mat4 mat);

void shader_set_transform(struct transform t, uint32_t shader_program);

void shader_set_vec3(uint32_t shader_program, const char* name, float vec[3]);

#define shader_set_uniform_primitive(shader_program, name, T) _Generic((T), \
	bool: shader_set_bool(shader_program, name, T), \
	int: shader_set_int(shader_program, name, T), \
	float: shader_set_float(shader_program, name, T) \
) \