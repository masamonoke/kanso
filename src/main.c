#include <stdint.h>
#include <stddef.h>

#include "app.h"

int32_t main(int32_t argc, char** argv) {
	app_state_t* app_state;
	char* scene_path;

	scene_path = NULL;
	if (argc > 1) {
		scene_path = argv[1];
	}

	if (!app_new(&app_state, scene_path)) {
		return 1;
	}

	while(!app_state->close) {
		app_state->input(app_state);

		app_state->update(app_state);
	}

	app_free(&app_state);

	return 0;
}
