#pragma once

#include <stdbool.h>  // for bool
#include <stdint.h>   // for int32_t

#include "scene.h"    // for scene_t
#include "window.h"   // for window_t

typedef struct app_state app_state_t;

struct app_state {
	window_t* window;
	scene_t* scene;
	bool close;
	/*! @brief Updates all states in context
	 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
	*/
	void (*update) (app_state_t* state);
	/*! @brief Reads key input
	 *  @param[in] app_state_t** ctx: An app state which will hold all data about current context
	*/
	void (*input) (app_state_t* state);
};

/*! @brief Creating new app context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
 * @return int32_t status: On error will return negative non-zero value. If context successfuly initialized returns 0
 * @pointer_lifetime mallocs memory for state. Caller should free context with call app_free(ctx)
*/
__attribute__((warn_unused_result)) int32_t app_new(app_state_t** state);

/*! @brief Frees all fields in application context and context itself
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
 * @pointer_lifetime *ctx pointer becomes NULL after free
*/
void app_free(app_state_t** ctx);
