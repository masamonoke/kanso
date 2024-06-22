#include "app.h"

#include <stdlib.h>
#include <assert.h>

#include <glad/glad.h>
#include <c_log.h>

#include "camera.h"
#include "keys.h"

static void input(app_state_t* state);

static void clear(void);

static void draw(app_state_t* ctx);

static void update(app_state_t* ctx);

static bool default_scene(scene_t* scene);

static bool custom_scene(scene_t* scene, const char* scene_path);

bool app_new(app_state_t** ctx, const char* scene_path) {
	assert(ctx != NULL);

	*ctx = malloc(sizeof(app_state_t));

	if (!window_new(&(*ctx)->window)) {
		log_error("Failed to create window");
		return false;
	}

	scene_new(&(*ctx)->scene);

	if (scene_path) {
		log_info("Loading scene %s", scene_path);
		if (!custom_scene((*ctx)->scene, scene_path)) {
			log_error("Failed to initialize scene %s. Shutting down", scene_path);
			app_free(ctx);
			return false;
		}
	} else {
		log_info("Loading default scene");
		if (!default_scene((*ctx)->scene)) {
			log_error("Failed to initialize default scene. Shutting down");
			app_free(ctx);
			return false;
		}
	}

	(*ctx)->update = update;
	(*ctx)->input = input;

	camera_init_callbacks((*ctx)->window);

	log_debug("Initiated camera callbacks");

	window_set_capture_cursor((*ctx)->window);

	log_debug("Set window cursor mode to capture");

	(*ctx)->close = false;

	return true;
}

void app_free(app_state_t** ctx) {
	assert(ctx != NULL);

	window_free(&(*ctx)->window);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void draw(app_state_t* ctx) {
	camera_update(ctx->window);
	ctx->scene->draw(ctx->scene);
	window_update(ctx->window);
}

static void update(app_state_t* ctx) {
	clear();
	draw(ctx);
}

static bool custom_scene(scene_t* scene, const char* scene_path) {
	return scene_load_from_json(scene, scene_path);
}

static bool default_scene(scene_t* scene) {
	return scene_load_from_json(scene, "scene/default_scene.json");
}
