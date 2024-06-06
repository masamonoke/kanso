#pragma once

#include <stddef.h>      // for size_t
#include <stdint.h>      // for uint32_t, int32_t

#include "cglm/types.h"  // for vec3

#include "shader.h"      // for transform

enum model_type {
	PRIMITVE_MODEL,
	LOADED_MODEL
};

// TODO: replace void* to model_t*
struct model_common {
	enum model_type type;
	uint32_t shader_program;
	struct transform transform;
	vec3 position;
	vec3 scale;
	uint32_t* _ref_count;
	void (*draw) (void*);
};

/*! @brief Parent for all models. Any model (LOADED_MODEL, PRIMITVE_MODEL) can be casted to from model_t
*/
typedef struct model {
	struct model_common common;
	void* model_data;
} model_t;

/*! @brief Creating new model from loaded_model_t or primitive_model_t pointer and is decided from type passed.
 * @param[in] model_t** model: Model abstraction object which will hold all data about model like vertices, meshes etc
 * @param[in] enum model_type type: Can be PRIMITVE_MODEL, LOADED_MODEL
 * @param[in] vec3* init_position: Initial position. Can be NULL and default position { 0.0f, 0.0f, 0.0f } will be used
 * @param[in] vec3* init_scale: Initial scale. Can be NULL and default scale { 1.0f, 1.0f, 1.0f } will be used
 * @param[in] const void* payload: Model specific data. For loaded_model_t this will be path to object data, for primitive this will be type of primitive like CUBE
 * @pointer_lifetime mallocs memory for model. Caller should free context with call model_free(ctx)
*/
void model_new(model_t** model, enum model_type type, vec3* init_position, vec3* init_scale, const void* payload);


// TODO: redo to model_share
void model_share(model_t** dest, model_t* ref);

__attribute__((warn_unused_result)) int32_t models_from_json(const char* path, model_t** models_ptrs, size_t* length, size_t max_models);

/*! @brief Frees model memory. The type of passed model defined from model.common.type
 * @param[in] model_t** model: Model abstraction object which will hold all data about model like vertices, meshes etc
 * @pointer_lifetime Model is freed and set to NULL
*/
void model_free(model_t** model);

void model_print(model_t* model);
