#include "scene.h"

#include <stdlib.h>
#include <assert.h>

#include "custom_logger.h"
#include "model.h"
#include "light.h"

#define MAX_MODELS_COUNT 1024
#define MAX_LIGHTS_COUNT 100

struct scene_ctx {
	size_t light_count;
	size_t model_count;
	model_t* models[MAX_MODELS_COUNT];
	light_t* lights[MAX_LIGHTS_COUNT];
};

static void draw(scene_t* scene);

int32_t scene_new(scene_t** scene) {
	size_t i;

	*scene = malloc(sizeof(scene_t));
	(*scene)->scn_ctx = malloc(sizeof(scene_ctx_t));

	for (i = 0; i < MAX_MODELS_COUNT; i++) {
		(*scene)->scn_ctx->models[i] = NULL;
	}
	(*scene)->scn_ctx->model_count = 0;

	for (i = 0; i < MAX_LIGHTS_COUNT; i++) {
		(*scene)->scn_ctx->lights[i] = NULL;
	}
	(*scene)->scn_ctx->light_count = 0;

	(*scene)->draw = draw;

	return 0;
}

void scene_free(scene_t** scene) {
	size_t i;
	size_t models_count_processed;
	size_t lights_count_processed;

	for (i = 0, models_count_processed = 0; i < MAX_MODELS_COUNT && models_count_processed != (*scene)->scn_ctx->model_count; i++) {
		if ((*scene)->scn_ctx->models[i] != NULL) {
			model_free(&(*scene)->scn_ctx->models[i]);
			models_count_processed++;
		}
	}

	for (i = 0, lights_count_processed = 0; i < MAX_LIGHTS_COUNT && lights_count_processed != (*scene)->scn_ctx->light_count; i++) {
		if ((*scene)->scn_ctx->lights[i] != NULL) {
			light_free(&(*scene)->scn_ctx->lights[i]);
			lights_count_processed++;
		}
	}

#ifdef DEBUG
	assert(models_count_processed == (*scene)->scn_ctx->model_count);
	assert(lights_count_processed == (*scene)->scn_ctx->light_count);

	for (i = 0; i < MAX_MODELS_COUNT; i++) {
		assert((*scene)->scn_ctx->models[i] == NULL);
	}

	for (i = 0; i < MAX_LIGHTS_COUNT; i++) {
		assert((*scene)->scn_ctx->lights[i] == NULL);
	}
#endif

	(*scene)->scn_ctx->model_count = 0;
	(*scene)->scn_ctx->light_count = 0;

	free((*scene)->scn_ctx);
	(*scene)->scn_ctx = NULL;
	free(*scene);
	*scene = NULL;
}

int32_t scene_add_model(scene_t* scene, model_t* model) {
	size_t i;

	for (i = 0; i < MAX_MODELS_COUNT; i++) {
		if (scene->scn_ctx->models[i] == NULL) {
			scene->scn_ctx->models[i] = model;
			return 0;
		}
	}

	custom_log_error("Can't add new model to scene");

	return -1;
}

int32_t scene_load_from_json(scene_t* scene, const char* path) {
	int32_t status;

	status = models_from_json(path, scene->scn_ctx->models, &scene->scn_ctx->model_count, MAX_MODELS_COUNT);
	status = light_from_json(path, scene->scn_ctx->lights, &scene->scn_ctx->light_count, MAX_LIGHTS_COUNT);

	return status;
}


static void draw (scene_t* scene) {
	size_t i;
	size_t j;
	size_t models_count;
	size_t lights_count;

	for (i = 0, models_count = 0; i < MAX_MODELS_COUNT; i++) {
		if (scene->scn_ctx->models[i] != NULL) {

			for (j = 0, lights_count = 0; j < MAX_LIGHTS_COUNT && lights_count < scene->scn_ctx->light_count; j++) {
				if (scene->scn_ctx->lights[j] != NULL) {
					scene->scn_ctx->lights[j]->common.bind_shader(scene->scn_ctx->lights[j], scene->scn_ctx->models[i]->common.shader_program);
					lights_count++;
				}
			}

			scene->scn_ctx->models[i]->common.draw(scene->scn_ctx->models[i]);
			models_count++;
			if (models_count == scene->scn_ctx->model_count) {
				break;
			}
		}
	}
}
