#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "scene.h"
#include "window.h"

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
 * @return result_t status
 * @pointer_lifetime mallocs memory for state. Caller should free context with call app_free(ctx)
*/
__attribute__((nonnull(1), warn_unused_result))
bool app_new(app_state_t** state, const char* scene_path);

/*! @brief Frees all fields in application context and context itself
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
 * @pointer_lifetime *ctx pointer becomes NULL after free
*/
__attribute__((nonnull(1)))
void app_free(app_state_t** ctx);
