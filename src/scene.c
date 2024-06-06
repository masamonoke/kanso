#include <math.h>           // for cosf
#include <stdlib.h>         // for NULL, size_t, free, malloc
#include <assert.h>         // for assert

#include <cglm/types.h>     // for vec3
#include <cglm/util.h>      // for glm_rad
#include <glad/glad.h>      // for glUseProgram

#include "camera.h"         // for camera_front, camera_pos
#include "custom_logger.h"  // for custom_log_debug, custom_log_error
#include "model.h"          // for model_common, model_free, model_t, models...
#include "shader.h"         // for shader_set_vec3, shader_set_uniform_primi...
#include "light.h"
#include "scene.h"

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
	size_t models_count;
	size_t lights_count;

	for (i = 0, models_count = 0; i < MAX_MODELS_COUNT && models_count != (*scene)->scn_ctx->model_count; i++) {
		if ((*scene)->scn_ctx->models[i] != NULL) {
			model_free(&(*scene)->scn_ctx->models[i]);
			models_count++;
		}
	}

	for (i = 0, lights_count = 0; i < MAX_LIGHTS_COUNT && lights_count != (*scene)->scn_ctx->light_count; i++) {
		if ((*scene)->scn_ctx->lights[i] != NULL) {
			light_free(&(*scene)->scn_ctx->lights[i]);
			lights_count++;
		}
	}

#ifdef DEBUG
	assert(models_count == (*scene)->scn_ctx->model_count);
	assert(lights_count == (*scene)->scn_ctx->light_count);

	for (i = 0; i < MAX_MODELS_COUNT; i++) {
		assert((*scene)->scn_ctx->models[i] == NULL);
	}

	for (i = 0; i < MAX_LIGHTS_COUNT; i++) {
		assert((*scene)->scn_ctx->lights[i] == NULL);
	}
#endif

	(*scene)->scn_ctx->model_count -= models_count;
	(*scene)->scn_ctx->light_count -= lights_count;

	free((*scene)->scn_ctx);
	(*scene)->scn_ctx = NULL;
	free(*scene);
	*scene = NULL;

	custom_log_debug("Freed scene");
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

/* #ifdef DEBUG */
/* 	size_t i; */
/* 	size_t model_count; */

/* 	for (i = 0, model_count = 0; i < MAX_MODELS_COUNT && model_count != scene->scn_ctx->model_count; i++) { */
/* 		if (scene->scn_ctx->models[i] != NULL) { */
/* 			model_print(scene->scn_ctx->models[i]); */
/* 			model_count++; */
/* 		} */
/* 	} */
/* #endif */

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
