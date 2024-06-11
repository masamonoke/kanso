#include <stdlib.h>     // for free, malloc, NULL

#include "glad/glad.h"  // for GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, glC...

#include "app.h"
#include "camera.h"     // for camera_init_callbacks, camera_update
#include "keys.h"       // for ESCAPE_KEY
#include "custom_logger.h"

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

static int32_t default_scene(scene_t* scene);

/*! @brief Updates all states in context
 * @param[in] app_state_t** ctx: An app state which will hold all data about current context
*/
int32_t app_new(app_state_t** ctx) {
	*ctx = malloc(sizeof(app_state_t));

	if (window_new(&(*ctx)->window)) {
		custom_log_error("Failed to create window");
		return -1;
	}

	if (scene_new(&(*ctx)->scene)) {
		custom_log_error("Failed to create scene");
		return -1;
	}

	custom_log_info("Loading scene");
	if (0 != default_scene((*ctx)->scene)) {
		custom_log_error("Failed to initialize default scene. Shutting down");
		app_free(ctx);
		return -1;
	}

	(*ctx)->update = update;
	(*ctx)->input = input;

	camera_init_callbacks((*ctx)->window);

	custom_log_debug("Initiated camera callbacks");

	window_set_capture_cursor((*ctx)->window);

	custom_log_debug("Set window cursor mode to capture");

	(*ctx)->close = false;

	return 0;
}

void app_free(app_state_t** ctx) {
	window_free(&(*ctx)->window);
	scene_free(&(*ctx)->scene);
	free(*ctx);
	*ctx = NULL;
	custom_log_info("All clear");
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

static void input(app_state_t* state) {
	if (window_is_key_pressed(state->window, ESCAPE_KEY)) {
		state->close = true;
	}
}

/*! @brief Set default scene image to white fill
*/

#define CLEAR_COLOR 0.1f, 0.1f, 0.1f, 1.0f // grey
static void clear(void) {
	glClearColor(CLEAR_COLOR);
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

static int32_t default_scene(scene_t* scene) {
	return scene_load_from_json(scene, "cfg/default_scene.json");
}
