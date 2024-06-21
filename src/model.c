#include "model.h"

#include <string.h>

#include <arraylist.h>
#include <json_object.h>
#include <json_util.h>
#include <cglm/types.h>
#include <c_log.h>

#include "loaded_model.h"
#include "primitive.h"

struct json_object;

void model_new(model_t** model, enum model_type type, vec3* init_position, vec3* init_scale, const void* payload) { // NOLINT
	assert(model != NULL);
	assert(payload != NULL);

	switch (type) {
		case LOADED_MODEL:
			if (!loaded_model_new((loaded_model_t**) model, (const char*) payload)) {
				return;
			}
			break;
		case PRIMITVE_MODEL:
			if (!primitive_new((void**) model, *((enum primitive_type*) payload))) {
				return;
			} else {
				log_error("Null primitive model type passed");
			}
			break;
		default:
			log_error("Unknown model type: %d", (*model)->common.type);
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
	assert(model != NULL);
	switch ((*model)->common.type) {
		case LOADED_MODEL:
			loaded_model_free((loaded_model_t**) model);
			break;
		case PRIMITVE_MODEL:
			primitive_free((void**) model);
			break;
		default:
			log_error("Unknown model type: %d", (*model)->common.type);
			break;
	}

	*model = NULL;
}

__attribute__((warn_unused_result))
static bool get_type(struct json_object* model_jso, struct json_object** jso, const char* path, const char** ret_type);

static void get_position(struct json_object* model_jso, struct json_object** jso, const char* path, vec3 position);

static void get_scale(struct json_object* model_jso, struct json_object** jso, vec3 scale);

__attribute__((warn_unused_result))
static bool add_loaded_model(vec3* position, vec3* scale, model_t** models_ptrs, size_t max_models, size_t* length, const char* model_path);

__attribute__((warn_unused_result))
static bool add_primitive_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale,
	model_t** models_ptrs, size_t max_models, size_t* length);

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

		log_debug("Reading model from config %s", path);

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

			model_jso = json_object_array_get_idx(models_jso, i);

			if (!get_type(model_jso, &jso, path, &type)) {
				goto L_FREE_JSO;
			}

			get_position(model_jso, &jso, path, position);

			get_scale(model_jso, &jso, scale);

			if (0 == strcmp(type, "LOADED_MODEL")) {
				if (json_object_object_get_ex(model_jso, "path", &jso)) {
					const char* model_path;

					model_path = json_object_get_string(jso);

					log_debug("Path is %s", model_path);

					if (!add_loaded_model(&position, &scale, models_ptrs, max_models, length, model_path)) {
						log_warn("Failed to add model %s", model_path);
					}

					log_info("Loaded model %s", model_path);
				} else {
					log_warn("Failed to get model path field from json file");
				}
			} else if (0 == strcmp(type, "PRIMITIVE_MODEL")) {
				if (!add_primitive_model(model_jso, &jso, &position, &scale, models_ptrs, max_models, length)) {
					log_warn("Failed to load primitive model");
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
	log_debug("Loading model %s", *ret_type);

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

static bool add_loaded_model(vec3* position, vec3* scale, model_t** models_ptrs,
		size_t max_models, size_t* length, const char* model_path)
{
	bool set;
	model_t* model;

	model_new(&model, LOADED_MODEL, position, scale, model_path);
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

static bool add_primitive_model(struct json_object* model_jso, struct json_object** jso, vec3* position, vec3* scale, model_t** models_ptrs,
	size_t max_models, size_t* length)
{
	const char* primitive_type_str;
	enum primitive_type primitive_type;
	bool set;

	if (!json_object_object_get_ex(model_jso, "primitive_type", jso)) {
		log_error("Failed to get primitive type");
		return false;
	} else {
		model_t* model;

		primitive_type_str = json_object_get_string(*jso);
		log_debug("Primitive type is %s", primitive_type_str);
		if (0 == strcmp(primitive_type_str, "CUBE")) {
			primitive_type = CUBE;
		} else {
			log_error("Failed to define primitive type %s", primitive_type_str);
			primitive_type = UNDEFINED_PRIMITIVE;
		}

		set = false;
		if (primitive_type != UNDEFINED_PRIMITIVE) {
			model_new(&model, PRIMITVE_MODEL, position, scale, &primitive_type);
			for (size_t j = 0; j < max_models; j++) {
				if (models_ptrs[j] == NULL) {
					models_ptrs[j] = model;
					set = true;
					break;
				}
			}
			if (set) {
				(*length)++;
			} else {
				model_free(&model);
			}
		}
	}

	return set;
}
