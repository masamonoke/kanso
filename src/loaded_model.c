#include <stdbool.h>          // for bool, false, true
#include <stdlib.h>           // for free, malloc
#include <string.h>           // for strcmp
#include <assert.h>           // for assert

#include <glad/glad.h>        // for glUseProgram

#include "cglm/affine.h"      // for glm_scale
#include "cglm/affine-pre.h"  // for glm_translate
#include "cglm/cam.h"         // for glm_perspective
#include "cglm/mat4.h"        // for glm_mat4_identity
#include "cglm/util.h"        // for glm_rad

#include "loaded_model.h"
#include "custom_logger.h"    // for custom_log_debug, custom_log_info, cust...
#include "model_loader.h"     // for model_loader_load_model
#include "shader.h"           // for transform, shader_set_mat4, shader_crea...
#include "camera.h"           // for camera_fov, camera_set_view, camera_pos

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

const uint32_t MAX_CACHE_ENTRIES = 1024;

struct model_cache_entry {
	loaded_model_t* model;   // value
	const char* path; // key
};

struct cache {
	struct model_cache_entry* models;
	size_t len;
};

static struct cache cache = {
	.len = 0,
	.models = NULL
};

int32_t loaded_model_new(loaded_model_t** model, const char* path) {
	size_t i;
	volatile bool found;

	if (cache.models == NULL) {
		custom_log_info("Models cache initiated");
		cache.models = malloc(sizeof(struct model_cache_entry) * MAX_CACHE_ENTRIES);
		for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
			cache.models[i].model = NULL;
			cache.models[i].path = NULL;
		}
	}

	*model = (loaded_model_t*) malloc(sizeof(loaded_model_t));

	for (i = 0; i < cache.len; i++) {
		if (cache.models[i].model != NULL) {
			if (0 == strcmp(cache.models[i].path, path)) {
				loaded_model_t* ref;

				custom_log_debug("%s model is already loaded", cache.models[i].path);
				ref = cache.models[i].model;
				found = true;
				model_share((model_t**) model, (model_t*) ref);

				(*model)->model_data.model_vo.ebo = ref->model_data.model_vo.ebo;
				(*model)->model_data.model_vo.vbo = ref->model_data.model_vo.vbo;
				(*model)->model_data.model_vo.vao = ref->model_data.model_vo.vao;
				(*model)->model_data.directory = ref->model_data.directory;
				(*model)->model_data.meshes = ref->model_data.meshes;
				(*model)->model_data.meshes_count = ref->model_data.meshes_count;

			}
		}
	}

	if (!found) {
		model_loader_load_model(*model, path);

		(*model)->common.type = LOADED_MODEL;
		(*model)->common.draw = draw;

		for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
			if (cache.models[i].model == NULL) {
				cache.models[i].model = *model;
				cache.models[i].path = path;
				cache.len++;
				break;
			}
		}
	} else {
		custom_log_info("Got model %s from shared memory", path);
	}

	if (shader_create_program("shaders/backpack.vert", "shaders/backpack.frag", &(*model)->common.shader_program)) {
		custom_log_error("Failed to compile model %s shader program", path);
	}

	return 0;
}

void loaded_model_free(loaded_model_t** model) {
	size_t i;
	static bool freed_cache = false;

	if (!freed_cache) {
		free(cache.models);
		cache.models = NULL;
	}

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
	custom_log_debug("Freed model");
}
