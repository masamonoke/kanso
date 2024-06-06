#pragma once

#include <stdint.h>  // for int32_t

#include "model.h"   // for model_t

typedef struct scene_ctx scene_ctx_t;

typedef struct scene {
	scene_ctx_t* scn_ctx;
	void (*draw) (struct scene*);
} scene_t;

/*! @brief Creating new scene
 * @param[in] scene_t** scene: A scene context which holds all data about models, lights etc
 * @return int32_t status: On error will return non-zero value. If context successfuly initialized returns 0
 * @pointer_lifetime mallocs memory for state. Caller should free context with call ctx.free(ctx)
*/
__attribute__((warn_unused_result)) int32_t scene_new(scene_t** scene);

/*! @brief Deleting scene context and setting pointer to NULL
 * @param[in] scene_t** scene: A scene context which holds all data about models, lights etc
 * @return int32_t status: On error will return non-zero value. If context successfuly initialized returns 0
 * @pointer_lifetime Frees *scene and sets to NULL
 * */
void scene_free(scene_t** scene);

/*! @brief Adding new model to scene. If model cannot be added to scene returns -1 otherwise returns 0
 * @param[in] scene_t** scene: A scene context which holds all data about models, lights etc
 * @return int32_t status: On error will return non-zero value. If context successfuly initialized returns 0
 * */
__attribute__((warn_unused_result)) int32_t scene_add_model(scene_t* scene, model_t* model);

__attribute__((warn_unused_result)) int32_t scene_load_from_json(scene_t* scene, const char* path);
