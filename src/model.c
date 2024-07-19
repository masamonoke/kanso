#include "model.h"

#include <string.h>
#include <assert.h>

#include <arraylist.h>
#include <json_object.h>
#include <json_util.h>
#include <cglm/types.h>
#include <c_log.h>

#include "loaded_model.h"

struct json_object;

bool model_new(model_t** model, vec3* init_position, vec3* init_scale, vec3* init_rotation, const void* payload) { // NOLINT
	assert(model != NULL);
	assert(payload != NULL);

	if (!loaded_model_new((loaded_model_t**) model, (const char*) payload)) {
		return false;
	}

	if (init_position) {
		memcpy((*model)->common.position, *init_position, sizeof(vec3));
	} else {
		memcpy((*model)->common.position, (float[]) { 0.0f, 0.0f, 0.0f }, sizeof(vec3));
	}

	if (init_scale) {
		memcpy((*model)->common.scale, *init_scale, sizeof(vec3));
	} else {
		memcpy((*model)->common.scale, (float[]) { 1.0f, 1.0f, 1.0f }, sizeof(vec3));
	}

	if (init_rotation) {
		memcpy((*model)->common.rotation, *init_rotation, sizeof(vec3));
	} else {
		memcpy((*model)->common.rotation, (float[]) { 0.0f, 0.0f, 0.0f }, sizeof(vec3));
	}

	(*model)->common.selected = false;

	return true;
}

void model_free(model_t** model) {
	assert(model != NULL);
	loaded_model_free((loaded_model_t**) model);

	*model = NULL;
}

__attribute__((warn_unused_result))
static bool get_type(struct json_object* model_jso, struct json_object** jso, const char* path, const char** ret_type);

static void get_position(struct json_object* model_jso, struct json_object** jso, const char* path, vec3 position);

static void get_scale(struct json_object* model_jso, struct json_object** jso, vec3 scale);

static void get_rotation(struct json_object* model_jso, struct json_object** jso, vec3 rotation);

__attribute__((warn_unused_result))
static bool add_loaded_model(vec3* position, vec3* scale, vec3* rotation, model_t** models_ptrs, size_t max_models, size_t* length, const char* model_path);

bool models_from_json(const char* path, model_t** models_ptrs, size_t* length, size_t max_models) {
	struct json_object* file_jso;
	bool status;

	assert(path != NULL);
	assert(models_ptrs != NULL);
	assert(length != NULL);

	status = true;
	file_jso = json_object_from_file(path);

	if (file_jso) {
		struct json_object* models_jso;
		size_t array_len;
		size_t i;

		log_debug("Reading models from %s", path);

		json_object_object_get_ex(file_jso, "models", &models_jso);
		if (!models_jso) {
			log_error("Failed to read models from %s", path);
			status = false;
			goto L_FREE_JSO;
		}

		// better not use struct arraylist* from json-c directly
		array_len = json_object_get_array(models_jso)->length;
		if (!array_len) {
			log_error("Failed to parse models from %s", path);
			status = false;
			goto L_FREE_JSO;
		}

		for (i = 0, *length = 0; i < array_len && i < max_models; i++) {
			struct json_object* model_jso;
			struct json_object* jso;
			const char* type;
			vec3 position;
			vec3 scale;
			vec3 rotation;

			model_jso = json_object_array_get_idx(models_jso, i);

			if (!get_type(model_jso, &jso, path, &type)) {
				goto L_FREE_JSO;
			}

			get_position(model_jso, &jso, path, position);

			get_scale(model_jso, &jso, scale);

			get_rotation(model_jso, &jso, rotation);

			if (0 == strcmp(type, "LOADED_MODEL")) {
				if (json_object_object_get_ex(model_jso, "path", &jso)) {
					const char* model_path;

					model_path = json_object_get_string(jso);

					if (!add_loaded_model(&position, &scale, &rotation, models_ptrs, max_models, length, model_path)) {
						log_warn("Failed to add model %s", model_path);
					}

					log_info("Loaded model %s", model_path);
				} else {
					log_warn("Failed to get model path field from json file");
				}
			} else {
				log_error("Unknown model type: %s", type);
			}
		}

L_FREE_JSO:
		if (!json_object_put(file_jso)) {
			log_error("Failed to free json object");
			status = false;
		}
	} else {
		log_error("Failed to load model from config %s", path);
		status = false;
	}

	log_debug("Loaded models: %d", *length);
	return status;
}


static bool get_type(struct json_object* model_jso, struct json_object** jso, const char* path, const char** ret_type) {
	if (!json_object_object_get_ex(model_jso, "model_type", jso)) {
		log_error("Failed to get type of model from %s", path);
		return false;
	}
	*ret_type = json_object_get_string(*jso);

	return true;
}

static void get_position(struct json_object* model_jso, struct json_object** jso, const char* path, vec3 position) {
	if (!json_object_object_get_ex(model_jso, "position", jso)) {
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
		log_warn("Failed to get position of model from %s. Using default position (%d, %d, %d) instead",
			path, (double) position[0], (double) position[1], (double) position[2]);
	} else {
		position[0] = (float) json_object_get_double(json_object_array_get_idx(*jso, 0));
		position[1] = (float) json_object_get_double(json_object_array_get_idx(*jso, 1));
		position[2] = (float) json_object_get_double(json_object_array_get_idx(*jso, 2));
	}
}

static void get_scale(struct json_object* model_jso, struct json_object** jso, vec3 scale) {
	if (!json_object_object_get_ex(model_jso, "scale", jso)) {
		scale[0] = 1;
		scale[1] = 1;
		scale[2] = 1;
	} else {
		scale[0] = (float) json_object_get_double(json_object_array_get_idx(*jso, 0));
		scale[1] = (float) json_object_get_double(json_object_array_get_idx(*jso, 1));
		scale[2] = (float) json_object_get_double(json_object_array_get_idx(*jso, 2));
	}
}

static void get_rotation(struct json_object* model_jso, struct json_object** jso, vec3 rotation) {
	if (!json_object_object_get_ex(model_jso, "rotation", jso)) {
		rotation[0] = 0;
		rotation[1] = 0;
		rotation[2] = 0;
	} else {
		rotation[0] = (float) json_object_get_double(json_object_array_get_idx(*jso, 0));
		rotation[1] = (float) json_object_get_double(json_object_array_get_idx(*jso, 1));
		rotation[2] = (float) json_object_get_double(json_object_array_get_idx(*jso, 2));
	}
}

static bool add_loaded_model(vec3* position, vec3* scale, vec3* rotation, model_t** models_ptrs,
		size_t max_models, size_t* length, const char* model_path)
{
	bool set;
	model_t* model;

	if (!model_new(&model, position, scale, rotation, model_path)) {
		return false;
	}
	set = false;
	for (size_t j = 0; j < max_models; j++) {
		if (models_ptrs[j] == NULL) {
			models_ptrs[j] = model;
			set = true;
			break;
		}
	}
	if (!set) {
		model_free(&model);
	} else {
		(*length)++;
	}

	return set;
}
