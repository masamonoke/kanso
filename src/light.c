#include "light.h"

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include <cglm/types.h>
#include <arraylist.h>
#include <json_object.h>
#include <json_util.h>
#include <cglm/util.h>
#include <c_log.h>

#include "point_light.h"
#include "spot_light.h"
#include "directional_light.h"
#include "camera.h"

struct json_object;

void light_new(light_t** light, enum light_type type, vec3* ambient, vec3* diffuse, vec3* specular, void* specific_data) {
	assert(light != NULL);
	assert(ambient != NULL);
	assert(diffuse != NULL);
	assert(specular != NULL);
	assert(specific_data != NULL);

	switch (type) {
		case LIGHT_SPOT:
			spot_light_new(light, specific_data);
			log_debug("Created spot light");
			break;
		case LIGHT_DIRECTIONAL:
			directional_light_new(light, specific_data);
			log_debug("Created directional light");
			break;
		case LIGHT_POINT:
			point_light_new(light, specific_data);
			log_debug("Created point light");
			break;
		default:
			log_error("Failed to define type: %d", type);
			return;
	}

	(*light)->common.type = type;
	memcpy((*light)->common.ambient, ambient, sizeof(vec3));
	memcpy((*light)->common.diffuse, diffuse, sizeof(vec3));
	memcpy((*light)->common.specular, specular, sizeof(vec3));
}

void light_free(light_t** light) {
	assert(light != NULL);

	switch ((*light)->common.type) {
		case LIGHT_POINT:
			point_light_free(light);
			break;
		case LIGHT_DIRECTIONAL:
			directional_light_free(light);
			break;
		case LIGHT_SPOT:
			spot_light_free(light);
			break;
		default:
			log_error("Undefined light type");
			break;
	}
}

__attribute__((warn_unused_result))
static bool get_type(struct json_object* light_jso, struct json_object** jso, const char* path, const char** ret_type);

static void get_common(struct json_object* light_jso, vec3 ambient, vec3 diffuse, vec3 specular);

__attribute__((warn_unused_result))
static bool create_spot_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular);

__attribute__((warn_unused_result))
static bool create_point_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular);

static void create_dir_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular);

bool light_from_json(const char* path, light_t** lights_ptrs, size_t* length, size_t max_lights) {
	struct json_object* file_jso;
	bool status;

	assert(path != NULL);
	assert(lights_ptrs != NULL);
	assert(length != NULL);

	status = true;
	file_jso = json_object_from_file(path);

	if (file_jso) {
		struct json_object* lights_jso;
		size_t array_len;
		size_t i;

		log_debug("Reading lights from %s", path);

		json_object_object_get_ex(file_jso, "lights", &lights_jso);
		if (!lights_jso) {
			log_error("Failed to read lights from %s", path);
			status = false;
			goto L_FREE_JSO;
		}

		// better not use struct arraylist* from json-c directly
		array_len = json_object_get_array(lights_jso)->length;
		if (!array_len) {
			log_error("Failed to parse lights from %s", path);
			status = false;
			goto L_FREE_JSO;
		}

		for (i = 0, *length = 0; i < array_len && i < max_lights; i++) {
			struct json_object* light_jso;
			struct json_object* jso;
			const char* type;
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;

			light_jso = json_object_array_get_idx(lights_jso, i);

			if (!get_type(light_jso, &jso, path, &type)) {
				goto L_FREE_JSO;
			}

			get_common(light_jso, ambient, diffuse, specular);

			if (0 == strcmp(type, "SPOT_LIGHT")) {
				if (create_spot_light(&lights_ptrs[i], light_jso, &ambient, &diffuse, &specular)) {
					(*length)++;
				}
			} else if (0 == strcmp(type, "DIRECTIONAL_LIGHT")) {
				create_dir_light(&lights_ptrs[i], light_jso, &ambient, &diffuse, &specular);
				(*length)++;
			} else if (0 == strcmp(type, "POINT_LIGHT")) {
				if (create_point_light(&lights_ptrs[i], light_jso, &ambient, &diffuse, &specular)) {
					(*length)++;
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

	log_debug("Loaded lights: %d", *length);
	return status;
}


__attribute__((warn_unused_result))
static bool get_type(struct json_object* light_jso, struct json_object** jso, const char* path, const char** ret_type) {
	if (!json_object_object_get_ex(light_jso, "light_type", jso)) {
		log_error("Failed to get type of light from %s", path);
		return false;
	}
	*ret_type = json_object_get_string(*jso);
	log_debug("Loading light %s", *ret_type);

	return true;
}

static void get_common(struct json_object* light_jso, vec3 ambient, vec3 diffuse, vec3 specular) { // NOLINT
	struct json_object* jso;

	if (!json_object_object_get_ex(light_jso, "ambient", &jso)) {
		log_error("Failed to get ambient");
	}

	ambient[0] = (float) json_object_get_double(json_object_array_get_idx(jso, 0));
	ambient[1] = (float) json_object_get_double(json_object_array_get_idx(jso, 1));
	ambient[2] = (float) json_object_get_double(json_object_array_get_idx(jso, 2));

	if (!json_object_object_get_ex(light_jso, "diffuse", &jso)) {
		log_error("Failed to get diffuse");
	}

	diffuse[0] = (float) json_object_get_double(json_object_array_get_idx(jso, 0));
	diffuse[1] = (float) json_object_get_double(json_object_array_get_idx(jso, 1));
	diffuse[2] = (float) json_object_get_double(json_object_array_get_idx(jso, 2));

	if (!json_object_object_get_ex(light_jso, "specular", &jso)) {
		log_error("Failed to get specular");
	}

	specular[0] = (float) json_object_get_double(json_object_array_get_idx(jso, 0));
	specular[1] = (float) json_object_get_double(json_object_array_get_idx(jso, 1));
	specular[2] = (float) json_object_get_double(json_object_array_get_idx(jso, 2));
}

static bool create_spot_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular) {
	struct json_object* jso;
	float constant;
	float linear;
	float quadratic;
	float inner_cut_off;
	float outer_cut_off;

	if (!json_object_object_get_ex(light_jso, "constant", &jso)) {
		log_error("Failed to get constant");
		return false;
	}
	constant = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "linear", &jso)) {
		log_error("Failed to get linear");
		return false;
	}
	linear = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "quadratic", &jso)) {
		log_error("Failed to get quadratic");
		return false;
	}
	quadratic = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "inner_cut_off_angle", &jso)) {
		log_error("Failed to get inner cut off angle");
		return false;
	}
	inner_cut_off = cosf((glm_rad((float) json_object_get_double(jso))));

	if (!json_object_object_get_ex(light_jso, "outer_cut_off_angle", &jso)) {
		log_error("Failed to get outer cut off angle");
		return false;
	}
	outer_cut_off = cosf((glm_rad((float) json_object_get_double(jso))));

	struct spot_light_specific specific = {
		.constant = constant,
		.linear = linear,
		.quadratic = quadratic,
		.inner_cut_off = inner_cut_off,
		.outer_cut_off = outer_cut_off
	};

	memcpy(specific.position, camera_pos(), sizeof(vec3));
	memcpy(specific.direction, camera_front(), sizeof(vec3));

	light_new(light, LIGHT_SPOT, ambient, diffuse, specular, &specific);

	return true;
}

static bool create_point_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular) {
	struct json_object* jso;
	float constant;
	float linear;
	float quadratic;
	vec3 position;

	if (!json_object_object_get_ex(light_jso, "constant", &jso)) {
		log_error("Failed to get constant");
		return false;
	}
	constant = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "linear", &jso)) {
		log_error("Failed to get linear");
		return false;
	}
	linear = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "quadratic", &jso)) {
		log_error("Failed to get quadratic");
		return false;
	}
	quadratic = (float) json_object_get_double(jso);

	if (!json_object_object_get_ex(light_jso, "position", &jso)) {
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
	} else {
		position[0] = (float) json_object_get_double(json_object_array_get_idx(jso, 0));
		position[1] = (float) json_object_get_double(json_object_array_get_idx(jso, 1));
		position[2] = (float) json_object_get_double(json_object_array_get_idx(jso, 2));
	}

	struct point_light_specific specific = {
		.constant = constant,
		.linear = linear,
		.quadratic = quadratic
	};
	memcpy(specific.position, position, sizeof(vec3));

	light_new(light, LIGHT_POINT, ambient, diffuse, specular, &specific);

	return true;
}

static void create_dir_light(light_t** light, struct json_object* light_jso, vec3* ambient, vec3* diffuse, vec3* specular) {
	struct dir_light_specific specific;
	vec3 direction;
	struct json_object* jso;

	if (!json_object_object_get_ex(light_jso, "position", &jso)) {
		direction[0] = 0;
		direction[1] = 0;
		direction[2] = 0;
	} else {
		direction[0] = (float) json_object_get_double(json_object_array_get_idx(jso, 0));
		direction[1] = (float) json_object_get_double(json_object_array_get_idx(jso, 1));
		direction[2] = (float) json_object_get_double(json_object_array_get_idx(jso, 2));
	}

	memcpy(specific.direction, direction, sizeof(vec3));

	light_new(light, LIGHT_DIRECTIONAL, ambient, diffuse, specular, &specific);
}
