#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <json_object.h>
#include <json_util.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <c_log.h>

#include "glfw_context.h"

#define DEFAULT_WINDOW_SIZE 800, 600
#define DEFAULT_TITLE "Kanso Engine"

static void err_callback(int32_t code, const char* err_str) {
	(void) code;
    fprintf(stderr, "GLFW Error: %s\n", err_str);
}

bool glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int)) {
	bool status;
	int32_t framebuffer_width;
	int32_t framebuffer_height;
	struct json_object* j;

	assert(window != NULL);
	assert(framebuffer_size_callback != NULL);

	status = true;

	glfwSetErrorCallback(err_callback);

	// valgrind and address sanitizer may show gpu memory leak after this function
	// and it not freed after glfwTerminate() call
	// but that is not glfw problem or anything else but GPU drivers (libraries outside glfw control)
	// like to allocate some global memory and keep it allocated even when GL/Vulkan context is destroyed.
	// They have some globals/statics that are never released.
	// So if valgrind shows memory leaks coming from them, this is to be expected.
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	j = json_object_from_file("cfg/window.json");
	if (j) {
		struct json_object* width_json;
		struct json_object* height_json;

		log_info("Reading window config from file");
		json_object_object_get_ex(j, "width", &width_json);
		json_object_object_get_ex(j, "height", &height_json);

		if (width_json && height_json) {
			*window = glfwCreateWindow(json_object_get_int(width_json), json_object_get_int(height_json), DEFAULT_TITLE, NULL, NULL);
		} else {
			log_error("Failed to read json height or width value");
			json_object_put(j);
			goto L_DEFAULT_WINDOW_INIT;
		}

		if (json_object_put(j)) {
			log_debug("Freed json object");
		} else {
			log_error("Failed to free json object");
		}

	} else {
		log_info("Window config is not found. Using default values");
L_DEFAULT_WINDOW_INIT:
		*window = glfwCreateWindow(DEFAULT_WINDOW_SIZE, DEFAULT_TITLE, NULL, NULL);
	}

	if (*window == NULL) {
		log_error("Failed to create GLFW window\n");
		glfwTerminate();
		status = false;
		goto L_RETURN;
	}
	glfwMakeContextCurrent(*window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		log_error("Failed to initialize GLAD\n");
		status = false;
		goto L_RETURN;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glfwGetFramebufferSize(*window, &framebuffer_width, &framebuffer_height);
	glViewport(0, 0, framebuffer_width, framebuffer_height);

	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

	log_info("Created GLFW window");

L_RETURN:

	return status;
}

float glfw_context_time(void) {
	return (float) glfwGetTime();
}

void glfw_context_capture_cursor(GLFWwindow* window) {
	assert(window != NULL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void glfw_context_set_cursor_pos_callback(GLFWwindow* window, void (*mouse_callback)(struct GLFWwindow*, double, double)) {
	assert(window != NULL);
	glfwSetCursorPosCallback(window, mouse_callback);
}

void glfw_context_set_scroll_callback(GLFWwindow* window, void (*scroll_callback)(struct GLFWwindow*, double, double)) {
	assert(window != NULL);
	assert(scroll_callback != NULL);
	glfwSetScrollCallback(window, scroll_callback);
}

void glfw_context_set_mouse_click_callback(GLFWwindow* window, void (*mouse_click_callback)(struct GLFWwindow*, int, int, int)) {
	assert(window != NULL);
	assert(mouse_click_callback != NULL);
	glfwSetMouseButtonCallback(window, mouse_click_callback);
}

void glfw_context_update_window(GLFWwindow* window) {
	assert(window != NULL);
	glfwSwapBuffers(window);
	glfwPollEvents();
}
