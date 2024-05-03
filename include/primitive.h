#pragma once

#include <stdint.h> // for int32_t
#include "shader.h" // shader_create_program, struct transform, shader_set_transform

typedef struct cube cube_t;

enum primitive_type {
	CUBE
};

int32_t primitive_new(enum primitive_type type, void** primitive);

void primitive_draw(enum primitive_type type, void* primitive, struct transform t);

void primitive_free(enum primitive_type type, void** primitive);
