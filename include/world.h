#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "model.h"
#include "light.h"

typedef struct world {
	size_t light_count;
	size_t model_count;
	size_t line_count;
	model_t** models;
	light_t** lights;
} world_t;

bool world_new(const char* scene_path, world_t** world);

void world_free(world_t** world);

void world_models(const world_t* world, model_t *** models, size_t* models_count);

void world_lights(const world_t* world, light_t*** lights, size_t* lights_count);
