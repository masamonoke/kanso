#include "model.h"

#include <string.h>

#include <arraylist.h>
#include <json_object.h>
#include <json_util.h>
#include <cglm/types.h>

#include "custom_logger.h"
#include "loaded_model.h"
#include "log.h"
#include "primitive.h"

struct json_object;

void model_new(model_t** model, enum model_type type, vec3* init_position, vec3* init_scale, const void* payload) { // NOLINT
	switch (type) {
		case LOADED_MODEL:
			if (0 != loaded_model_new((loaded_model_t**) model, (const char*) payload)) {
				return;
			}
			break;
		case PRIMITVE_MODEL:
			if (0 != primitive_new((void**) model, *((enum primitive_type*) payload))) {
				return;
			} else {
				custom_log_error("Null primitive model type passed");
			}
			break;
		default:
			custom_log_error("Unknown model type: %d", (*model)->common.type);
			break;
	}

	if (init_position) {
		memcpy((*model)->common.position, *init_position, sizeof(vec3));
	} else {
		(*model)->common.position[0] = 0.0f;
		(*model)->common.position[1] = 0.0f;
		(*model)->common.position[2] = 0.0f;
	}
	if (init_scale) {
		memcpy((*model)->common.scale, *init_scale, sizeof(vec3));
	} else {
		(*model)->common.scale[0] = 1.0f;
		(*model)->common.scale[1] = 1.0f;
		(*model)->common.scale[2] = 1.0f;
	}
}

void model_free(model_t** model) {
	switch ((*model)->common.type) {
		case LOADED_MODEL:
			loaded_model_free((loaded_model_t**) model);
			break;
		case PRIMITVE_MODEL:
			primitive_free((void**) model);
			break;
		default:
			custom_log_error("Unknown model type: %d", (*model)->common.type);
			break;
	}

	*model = NULL;
}

__attribute__((warn_unused_result))
static int32_t get_type(struct json_object* model_jso, struct json_object** jso, const char* path, const char** ret_type);

static void get_position(struct json_object* model_jso, struct json_object** jso, const char* path, vec3 position);

static void get_scale(struct json_object* model_jso, struct json_object** jso, vec3 scale);

__attribute__((warn_unused_result))
static int32_t add_loaded_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale, model_t** models_ptrs,
	size_t max_models, size_t* length);

__attribute__((warn_unused_result))
static int32_t add_primitive_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale,
		model_t** models_ptrs, size_t max_models, size_t* length);

int32_t models_from_json(const char* path, model_t** models_ptrs, size_t* length, size_t max_models) {
	struct json_object* file_jso;
	int32_t status;

	status = 0;
	file_jso = json_object_from_file(path);

	if (file_jso) {
		struct json_object* models_jso;
		size_t array_len;
		size_t i;

		custom_log_debug("Reading model from config %s", path);

		json_object_object_get_ex(file_jso, "models", &models_jso);
		if (!models_jso) {
			log_error("Failed to read models from %s", path);
			status = -1;
			goto L_FREE_JSO;
		}

		// better not use struct arraylist* from json-c directly
		if (!(array_len = json_object_get_array(models_jso)->length)) { // NOLINT
			custom_log_error("Failed to parse models from %s", path);
			status = -1;
			goto L_FREE_JSO;
		}

		for (i = 0, *length = 0; i < array_len; i++) {
			struct json_object* model_jso;
			struct json_object* jso;
			const char* type;
			vec3 position;
			vec3 scale;

			model_jso = json_object_array_get_idx(models_jso, i);

			if (0 != get_type(model_jso, &jso, path, &type)) {
				goto L_FREE_JSO;
			}

			get_position(model_jso, &jso, path, position);

			get_scale(model_jso, &jso, scale);

			if (0 == strcmp(type, "LOADED_MODEL")) {
				(void) add_loaded_model(model_jso, &jso, &position, &scale, models_ptrs, max_models, length);
			} else if (0 == strcmp(type, "PRIMITIVE_MODEL")) {
				(void) add_primitive_model(model_jso, &jso, &position, &scale, models_ptrs, max_models, length);
			} else {
				log_error("Unknown model type: %s", type);
			}
		}

L_FREE_JSO:
		if (!json_object_put(file_jso)) {
			custom_log_error("Failed to free json object");
			status = -1;
		}
	} else {
		custom_log_error("Failed to load model from config %s", path);
		status = -1;
	}

	custom_log_debug("Loaded models: %d", *length);
	return status;
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

static int32_t get_type(struct json_object* model_jso, struct json_object** jso, const char* path, const char** ret_type) {
	if (!json_object_object_get_ex(model_jso, "model_type", jso)) {
		custom_log_error("Failed to get type of model from %s", path);
		return -1;
	}
	*ret_type = json_object_get_string(*jso);
	custom_log_debug("Loading model %s", *ret_type);

	return 0;
}

static void get_position(struct json_object* model_jso, struct json_object** jso, const char* path, vec3 position) {
	if (!json_object_object_get_ex(model_jso, "position", jso)) {
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
		custom_log_warn("Failed to get position of model from %s. Using default position (%d, %d, %d) instead",
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

static int32_t add_loaded_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale, model_t** models_ptrs,
		size_t max_models, size_t* length)
{
	const char* model_path;

	if (!json_object_object_get_ex(model_jso, "path", jso)) {
		custom_log_error("Failed to get type of model from %s", model_path);
		return -1;
	} else {
		model_t* model;

		model_path = json_object_get_string(*jso);
		custom_log_debug("Path is %s", model_path);
		model_new(&model, LOADED_MODEL, position, scale, model_path);
		for (size_t j = 0; j < max_models; j++) {
			if (models_ptrs[j] == NULL) {
				models_ptrs[j] = model;
				break;
			}
		}
		(*length)++;
	}

	return 0;
}

static int32_t add_primitive_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale, model_t** models_ptrs,
	size_t max_models, size_t* length)
{
	const char* primitive_type_str;
	enum primitive_type primitive_type;
	if (!json_object_object_get_ex(model_jso, "primitive_type", jso)) {
		custom_log_error("Failed to get primitive type");
		return -1;
	} else {
		model_t* model;

		primitive_type_str = json_object_get_string(*jso);
		custom_log_debug("Primitive type is %s", primitive_type_str);
		if (0 == strcmp(primitive_type_str, "CUBE")) {
			primitive_type = CUBE;
		} else {
			custom_log_error("Failed to define type %s", primitive_type_str);
			primitive_type = UNDEFINED_PRIMITIVE;
		}

		if (primitive_type != UNDEFINED_PRIMITIVE) {
			model_new(&model, PRIMITVE_MODEL, position, scale, &primitive_type);
			for (size_t j = 0; j < max_models; j++) {
				if (models_ptrs[j] == NULL) {
					models_ptrs[j] = model;
					break;
				}
			}
			(*length)++;
		}
	}

	return 0;
}
