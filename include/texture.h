#pragma once

#include <stdint.h>

typedef struct texture {
	uint32_t id;
	char* type;
	char* path;
} texture_t;

__attribute__((nonnull(1), warn_unused_result))
int32_t texture_create(const uint8_t* texture_bytes, int32_t width, int32_t height, int32_t nr_channels);
