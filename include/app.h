#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "scene.h"

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
	void (*free) (app_state_t* state);
};

/*! @brief Creating new app context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
 * @return int32_t status: On error will return negative non-zero value. If context successfuly initialized returns 0
 * @pointer_lifetime mallocs memory for state. Caller should free context with call ctx.free(ctx)
*/
int32_t app_new(app_state_t** state);
