#include "loaded_model.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glad/glad.h>
#include <cglm/affine.h>
#include <cglm/affine-pre.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <cglm/types.h>

#include "custom_logger.h"
#include "model_loader.h"
#include "shader.h"
#include "camera.h"

static void draw(void* model) {
	loaded_model_t* loaded_model;

	loaded_model = (loaded_model_t*) model;

	camera_set_view(loaded_model->common.transform.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, loaded_model->common.transform.proj);

	glUseProgram(loaded_model->common.shader_program);

	glm_mat4_identity(loaded_model->common.transform.model);
	glm_mat4_identity(loaded_model->common.transform.proj);
	glm_mat4_identity(loaded_model->common.transform.view);

	camera_set_view(loaded_model->common.transform.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, loaded_model->common.transform.proj);
	glm_translate(loaded_model->common.transform.model, loaded_model->common.position);
	glm_scale(loaded_model->common.transform.model, loaded_model->common.scale);

	shader_set_mat4(loaded_model->common.shader_program, "model", loaded_model->common.transform.model);
	shader_set_mat4(loaded_model->common.shader_program, "view", loaded_model->common.transform.view);
	shader_set_mat4(loaded_model->common.shader_program, "proj", loaded_model->common.transform.proj);

	shader_set_vec3(loaded_model->common.shader_program, "viewPos", (float*) camera_pos());
	shader_set_uniform_primitive(loaded_model->common.shader_program, "material.shininess", 32.0f);

	for (size_t i = 0; i < loaded_model->model_data.meshes_count; i++) {
		assert(loaded_model->model_data.meshes[i] != NULL);
		loaded_model->model_data.meshes[i]->draw(loaded_model->model_data.meshes[i], loaded_model->common.shader_program);
	}
}

#define MAX_CACHE_ENTRIES 1024

struct model_cache_entry {
	loaded_model_t* model;   // value
	const char* path; // key
};

struct model_cache {
	struct model_cache_entry models[MAX_CACHE_ENTRIES];
	size_t len;
};

static struct model_cache cache = {
	.len = 0,
};

static void share_ref(loaded_model_t** dest, loaded_model_t* ref);

int32_t loaded_model_new(loaded_model_t** model, const char* path) {
	size_t i;
	volatile bool found;
	static bool initialized_cache = false;

	if (!initialized_cache) {
		for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
			cache.models[i].model = NULL;
			cache.models[i].path = NULL;
		}
		initialized_cache = true;
	}

	*model = (loaded_model_t*) malloc(sizeof(loaded_model_t));

	found = false;
	for (i = 0; i < cache.len; i++) {
		if (cache.models[i].model != NULL) {
			if (0 == strcmp(cache.models[i].path, path)) {
				loaded_model_t* ref;

				ref = cache.models[i].model;
				found = true;

				share_ref(model, ref);
				break;
			}
		}
	}

	if (!found) {
		model_loader_load_model(*model, path);

		(*model)->common.type = LOADED_MODEL;
		(*model)->common.draw = draw;
		(*model)->model_data._ref_count = malloc(sizeof(uint32_t));
		*(*model)->model_data._ref_count = 0;

		for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
			if (cache.models[i].model == NULL) {
				cache.models[i].model = *model;
				cache.models[i].path = path;
				cache.len++;
				break;
			}
		}
	}

	if (shader_create_program("shaders/backpack.vert", "shaders/backpack.frag", &(*model)->common.shader_program)) {
		custom_log_error("Failed to compile model %s shader program", path);
	}

	return 0;
}

void loaded_model_free(loaded_model_t** model) {
	size_t i;

	if (!(*model)->model_data._ref_count) {
		return;
	}
	if (*(*model)->model_data._ref_count == 1) {

		free((*model)->model_data._ref_count);
		(*model)->model_data._ref_count = NULL;

		for (i = 0; i < (*model)->model_data.meshes_count; i++) {
			mesh_delete(&(*model)->model_data.meshes[i]);
		}

#ifdef DEBUG
		for (i = 0; i < (*model)->model_data.meshes_count; i++) {
			assert((*model)->model_data.meshes[i] == NULL);
		}
#endif

		free((*model)->model_data.meshes);
		(*model)->model_data.meshes = NULL;
		free((char*) ((*model)->model_data.directory));
		(*model)->model_data.directory = NULL;
		free(*model);
	} else {
		*(*model)->model_data._ref_count -= 1;
		// free shared ptr
		free(*model);
	}
}

static void share_ref(loaded_model_t** dest, loaded_model_t* ref) {
	struct transform t = TRANSFORM_IDENTITY;

	(*dest)->common.type = ref->common.type;
	(*dest)->common.draw = ref->common.draw;
	memcpy((*dest)->common.position, ref->common.position, sizeof(vec3));
	memcpy((*dest)->common.scale, ref->common.scale, sizeof(vec3));
	(*dest)->common.transform = t;

	ref->model_data._ref_count += 1;
	(*dest)->model_data._ref_count = ref->model_data._ref_count;

	(*dest)->model_data.directory = ref->model_data.directory;
	(*dest)->model_data.meshes = ref->model_data.meshes;
	(*dest)->model_data.meshes_count = ref->model_data.meshes_count;
}
