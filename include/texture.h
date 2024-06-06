#pragma once

#include <stdint.h>  // for int32_t, uint32_t

typedef struct texture {
	uint32_t id;
	char* type;
	char* path;
} texture_t;

__attribute__((warn_unused_result)) int32_t texture_create_gl_texture(const char* texname);
