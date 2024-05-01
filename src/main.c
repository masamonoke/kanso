#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <log.h>

#include "app.h"

int32_t main(void) {
	app_state_t* app_state;

	if (app_new(&app_state)) {
		return 1;
	}

	while(!app_state->close) {
		app_state->input(app_state);

		app_state->update(app_state);
	}

	app_state->free(app_state);

	return 0;
}
