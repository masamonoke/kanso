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
#include <c_log.h>

#include "model_loader.h"
#include "shader.h"
#include "camera.h"
#include "window.h"

static void draw_model(loaded_model_t* model, uint32_t shader);

static void draw(void* model) {
	loaded_model_t* loaded_model;

	loaded_model = (loaded_model_t*) model;

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	draw_model(loaded_model, loaded_model->common.render_shader);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	draw_model(loaded_model, loaded_model->common.outline_shader);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
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

bool loaded_model_new(loaded_model_t** model, const char* path) {
	size_t i;
	volatile bool found;
	static bool initialized_cache = false;

	assert(model != NULL);
	assert(path != NULL);

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
		bool set;

		set = false;
		for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
			if (cache.models[i].model == NULL) {

				if (!model_loader_load_model(*model, path)) {
					free(*model);
					*model = NULL;
					return false;
				}

				(*model)->common.type = LOADED_MODEL;
				(*model)->common.draw = draw;
				(*model)->model_data._ref_count = malloc(sizeof(uint32_t));
				*(*model)->model_data._ref_count = 0;

				cache.models[i].model = *model;
				cache.models[i].path = path;
				cache.len++;
				set = true;
				break;
			}
		}

		if (!set) {
			return false;
		}
	}

	if (!shader_create_program("shaders/default.vert", "shaders/default.frag", &(*model)->common.render_shader)) {
		log_error("Failed to compile model %s render shader program", path);
	}
	if (!shader_create_program("shaders/outline.vert", "shaders/outline.frag", &(*model)->common.outline_shader)) {
		log_error("Failed to compile model %s shader program", path);
	}

	return true;
}

void loaded_model_free(loaded_model_t** model) {
	size_t i;

	assert(model != NULL);

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

static void draw_model(loaded_model_t* model, uint32_t shader) {
	float w_width;
	float w_height;

	glUseProgram(shader);

	glm_mat4_identity(model->common.transform.model);
	glm_mat4_identity(model->common.transform.proj);
	glm_mat4_identity(model->common.transform.view);

	glm_translate(model->common.transform.model, model->common.position);

	glm_scale(model->common.transform.model, model->common.scale);

	glm_rotate(model->common.transform.model, glm_rad(model->common.rotation[0]), (float[]) { 1, 0, 0 });
	glm_rotate(model->common.transform.model, glm_rad(model->common.rotation[1]), (float[]) { 0, 1, 0 });
	glm_rotate(model->common.transform.model, glm_rad(model->common.rotation[2]), (float[]) { 0, 0, 1 });

	camera_set_view(model->common.transform.view);
	w_height = (float) window_height();
	w_width = (float) window_width();
	glm_perspective(glm_rad(camera_fov()), w_width / w_height, 0.1f, 100.f, model->common.transform.proj);

	shader_set_mat4(shader, "model", model->common.transform.model);
	shader_set_mat4(shader, "view", model->common.transform.view);
	shader_set_mat4(shader, "proj", model->common.transform.proj);

	shader_set_vec3(shader, "viewPos", (float*) camera_pos());
	shader_set_uniform_primitive(shader, "material.shininess", 32.0f);

	for (size_t i = 0; i < model->model_data.meshes_count; i++) {
		assert(model->model_data.meshes[i] != NULL);
		model->model_data.meshes[i]->draw(model->model_data.meshes[i], shader);
	}
}
