#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <cglm/types.h>

struct model_common {
	uint32_t render_shader;
	uint32_t outline_shader;
	vec3 position;
	vec3 center;
	vec3 scale;
	vec3 rotation;
	bool selected;
	void (*draw) (void*);
};

typedef struct model {
	struct model_common common;
	void* model_data;
} model_t;

__attribute__((nonnull(1, 5)))
bool model_new(model_t** model, /*enum model_type type,*/ vec3* init_position, vec3* init_scale, vec3* init_rotation, const void* payload);

__attribute__((nonnull(1, 2, 3), warn_unused_result))
bool models_from_json(const char* path, model_t** models_ptrs, size_t* length, size_t max_models);

__attribute__((nonnull(1)))
void model_free(model_t** model);
