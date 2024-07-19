#pragma once

#include <stddef.h>

#include "model.h"
#include "light.h"
#include "world.h"

typedef struct scene {
	size_t light_count;
	size_t model_count;
	size_t line_count;
	model_t** models;
	light_t** lights;
	void (*draw) (struct scene*);
} scene_t;

__attribute__((nonnull(1)))
void scene_new(scene_t** scene, const world_t* world);

__attribute__((nonnull(1)))
void scene_free(scene_t** scene);
