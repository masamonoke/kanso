#include "scene.h"

#include <stdlib.h>
#include <assert.h>

#include "model.h"
#include "light.h"
#include "world.h"

static void draw(scene_t* scene);

void scene_new(scene_t** scene, const world_t* world) {
	assert(scene != NULL);

	*scene = malloc(sizeof(scene_t));

	world_models(world, &(*scene)->models, &(*scene)->model_count);
	world_lights(world, &(*scene)->lights, &(*scene)->light_count);

	(*scene)->draw = draw;
}

void scene_free(scene_t** scene) {
	assert(scene != NULL);

	free(*scene);
	*scene = NULL;
}

static void draw (scene_t* scene) {
	size_t i;
	size_t j;

	for (i = 0; i < scene->model_count; i++) {
		if (scene->models[i] != NULL) {

			for (j = 0; j < scene->light_count; j++) {
				if (scene->lights[j] != NULL) {
					scene->lights[j]->common.load_data_to_shader(scene->lights[j], scene->models[i]->common.render_shader);
				}
			}

			scene->models[i]->common.draw(scene->models[i]);
		}
	}
}
