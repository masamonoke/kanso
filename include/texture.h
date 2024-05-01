#pragma once

#include <stdint.h>

#include <stddef.h>

typedef struct texture {
	uint32_t id;
	char* type;
	char* path;
} texture_t;

int32_t texture_create_gl_texture(const char* texname);
