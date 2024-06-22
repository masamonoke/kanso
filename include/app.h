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
	void (*update) (app_state_t* state);
	void (*input) (app_state_t* state);
};

__attribute__((nonnull(1), warn_unused_result))
bool app_new(app_state_t** state, const char* scene_path);

__attribute__((nonnull(1)))
void app_free(app_state_t** ctx);
