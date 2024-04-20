#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <log.h>

#include "app.h"

static void clear(void);

int32_t main(void) {
	app_state_t* app_state;

	if (app_new(&app_state)) {
		return 1;
	}

	while(!app_state->close) {
		clear();

		app_state->input(app_state);

		app_state->update(app_state);
	}

	app_state->free(app_state);

	return 0;
}

void clear(void) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
