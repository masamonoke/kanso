#include "app.h"

#include <stdlib.h>
#include <assert.h>

#include <glad/glad.h>
#include <c_log.h>

#include "keys.h"
#include "control.h"
#include "world.h"

#define DEFAULT_SCENE_PATH "scene/default_scene.json"

static void input(app_state_t* state);

static void clear(void);

static void draw(app_state_t* ctx);

static void update(app_state_t* ctx);

bool app_new(app_state_t** ctx, const char* scene_path) {
	assert(ctx != NULL);

	*ctx = malloc(sizeof(app_state_t));

	if (!window_new(&(*ctx)->window)) {
		log_error("Failed to create window");
		return false;
	}

	if (!scene_path) {
		scene_path = DEFAULT_SCENE_PATH;
	}

	if (!world_new(scene_path, &(*ctx)->world)) {
		log_error("Failed to initialize scene %s. Shutting down", scene_path);
		app_free(ctx);
		return false;
	}

	scene_new(&(*ctx)->scene, (*ctx)->world);

	(*ctx)->update = update;
	(*ctx)->input = input;

	control_init((*ctx)->window, (*ctx)->world);

	(*ctx)->close = false;

	return true;
}

void app_free(app_state_t** ctx) {
	assert(ctx != NULL);

	window_free(&(*ctx)->window);
	world_free(&(*ctx)->world);
	scene_free(&(*ctx)->scene);
	free(*ctx);
	*ctx = NULL;
	log_info("All clear");
}


static void input(app_state_t* state) {
	if (window_is_key_pressed(state->window, ESCAPE_KEY)) {
		state->close = true;
	}
}

#define CLEAR_COLOR 0.1f, 0.1f, 0.1f, 1.0f // dark gray
static void clear(void) {
	glClearColor(CLEAR_COLOR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void draw(app_state_t* ctx) {
	clear();
	control_update(ctx->window);
	ctx->scene->draw(ctx->scene);
	window_update(ctx->window);
}

static void update(app_state_t* ctx) {
	draw(ctx);
}
