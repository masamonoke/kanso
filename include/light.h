#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <cglm/types.h>

enum light_type {
	LIGHT_POINT,
	LIGHT_DIRECTIONAL,
	LIGHT_SPOT
};

struct light_common {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	enum light_type type;
	void (*bind_shader) (void*, uint32_t);
};

typedef struct light {
	struct light_common common;
	void* light_specific;
} light_t;

__attribute__((nonnull(1, 3, 4, 5, 6)))
void light_new(light_t** light, enum light_type type, vec3* ambient, vec3* diffuse, vec3* specular, void* specific_data);

__attribute__((nonnull(1)))
void light_free(light_t** light);

__attribute__((nonnull(1, 2, 3)))
bool light_from_json(const char* path, light_t** models_ptrs, size_t* length, size_t max_models);
