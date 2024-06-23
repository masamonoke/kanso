#pragma once

#include <stdbool.h>

#include "model.h"

typedef struct scene_ctx scene_ctx_t;

typedef struct scene {
	scene_ctx_t* scn_ctx;
	void (*draw) (struct scene*);
} scene_t;

__attribute__((nonnull(1)))
void scene_new(scene_t** scene);

__attribute__((nonnull(1)))
void scene_free(scene_t** scene);

__attribute__((nonnull(1, 2), warn_unused_result))
bool scene_add_model(scene_t* scene, model_t* model);

__attribute__((nonnull(1, 2), warn_unused_result))
bool scene_load_from_json(scene_t* scene, const char* path);
