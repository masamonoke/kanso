#include "app.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "keys.h"

#include <log.h>

#include <stdlib.h>
#include <assert.h>

/*! @brief Reads key input
 *  @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
static void input(app_state_t* state);

/*! @brief Set default scene image to white fill
*/
static void clear(void);

/*! @brief Draws scene with data from context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
static void draw(app_state_t* ctx);

/*! @brief Updates all states in context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
static void update(app_state_t* ctx);

/*! @brief Updates all states in context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
int32_t app_new(app_state_t** ctx) {
	*ctx = malloc(sizeof(app_state_t));

	if (window_new(&(*ctx)->window)) {
		log_error("Failed to create window");
		return -1;
	}

	if (scene_new(&(*ctx)->scene)) {
		log_error("Failed to create scene");
		return -1;
	}

	(*ctx)->update = update;
	(*ctx)->input = input;

	camera_init_callbacks((*ctx)->window);

	window_set_capture_cursor((*ctx)->window);

	(*ctx)->close = false;

	return 0;
}

void app_free(app_state_t** ctx) {
	window_free(&(*ctx)->window);
	scene_free(&(*ctx)->scene);
	free(*ctx);
	*ctx = NULL;
	log_info("Freed application");
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

static void input(app_state_t* state) {
	if (window_is_key_pressed(state->window, ESCAPE_KEY)) {
		state->close = true;
	}
}

/*! @brief Set default scene image to white fill
*/
static void clear(void) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*! @brief Draws scene with data from context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
static void draw(app_state_t* ctx) {
	camera_update(ctx->window);
	ctx->scene->draw(ctx->scene);
	window_update(ctx->window);
}

/*! @brief Updates all states in context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
static void update(app_state_t* ctx) {
	clear();
	draw(ctx);
}

