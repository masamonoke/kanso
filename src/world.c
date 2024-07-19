#include "world.h"

#include <assert.h>
#include <stdlib.h>

#include "model.h"
#include "light.h"

#define MAX_MODELS_COUNT 1024
#define MAX_LIGHTS_COUNT 100

bool world_new(const char* scene_path, world_t** world) {
	bool status;
	size_t i;

	assert(scene_path != NULL);

	*world = malloc(sizeof(world_t));
	(*world)->models = malloc(sizeof(model_t) * MAX_MODELS_COUNT);
	(*world)->lights = malloc(sizeof(light_t) * MAX_LIGHTS_COUNT);

	(*world)->light_count = 0;
	(*world)->model_count = 0;

	for (i = 0; i < MAX_LIGHTS_COUNT; i++) {
		(*world)->lights[i] = NULL;
	}

	for (i = 0; i < MAX_MODELS_COUNT; i++) {
		(*world)->models[i] = NULL;
	}

	status = models_from_json(scene_path, (*world)->models, &(*world)->model_count, MAX_MODELS_COUNT);
	status = light_from_json(scene_path, (*world)->lights, &(*world)->light_count, MAX_LIGHTS_COUNT);

	return status;
}

void world_free(world_t** world) {
	size_t i;
	for (i = 0; i <  (*world)->model_count; i++) {
		if ((*world)->models[i] != NULL) {
			model_free(&(*world)->models[i]);
		}
	}

	for (i = 0; i < (*world)->light_count; i++) {
		if ((*world)->lights[i] != NULL) {
			light_free(&(*world)->lights[i]);
		}
	}

	(*world)->model_count = 0;
	(*world)->light_count = 0;
	free(*world);
	*world = NULL;
}

void world_models(const world_t* world, model_t *** models, size_t* models_count) {
	*models = world->models;
	*models_count = world->model_count;
}

void world_lights(const world_t* world, light_t*** lights, size_t* lights_count) {
	*lights = world->lights;
	*lights_count = world->light_count;
}
