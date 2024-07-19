#include "window.h"

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <json_object.h>
#include <json_util.h>
#include <c_log.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define DEFAULT_WINDOW_SIZE 800, 600
#define DEFAULT_TITLE "Kanso Engine"

struct GLFWwindow;

struct window {
	// WARNING: do not use this field. Use (GLFWWindow*) window_t_ptr cast instead
	GLFWwindow* _glfw_ctx;
};

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);

static void err_callback(int32_t code, const char* err_str) {
	(void) code;
    fprintf(stderr, "GLFW Error: %s\n", err_str);
}

bool window_new(window_t** window) {
	bool status;
	int32_t framebuffer_width;
	int32_t framebuffer_height;
	struct json_object* j;

	assert(window != NULL);

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
			*window = (window_t*) glfwCreateWindow(json_object_get_int(width_json), json_object_get_int(height_json), DEFAULT_TITLE, NULL, NULL);
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
		*window = (window_t*) glfwCreateWindow(DEFAULT_WINDOW_SIZE, DEFAULT_TITLE, NULL, NULL);
	}

	if (*window == NULL) {
		log_error("Failed to create GLFW window\n");
		glfwTerminate();
		status = false;
		goto L_RETURN;
	}
	glfwMakeContextCurrent((GLFWwindow*) *window);

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

	glfwGetFramebufferSize((GLFWwindow*) *window, &framebuffer_width, &framebuffer_height);
	glViewport(0, 0, framebuffer_width, framebuffer_height);

	glfwSetFramebufferSizeCallback((GLFWwindow*) *window, framebuffer_size_callback);

	log_info("Created GLFW window");

L_RETURN:

	return status;
}

void window_free(window_t** window) {
	glfwDestroyWindow((GLFWwindow*) *window);
	glfwTerminate();
	*window = NULL;
}

float window_context_time(void) {
	return (float) glfwGetTime();
}

void window_hide_cursor(window_t* window) {
	assert(window != NULL);
	glfwSetInputMode((GLFWwindow*) window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void window_reset_cursor(window_t* window) {
	assert(window != NULL);
	glfwSetInputMode((GLFWwindow*) window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void window_set_cursor(window_t* window, void (*mouse_callback)(window_t*, double, double)) {
	void (*glfw_mouse_callback) (GLFWwindow*, double, double);
	glfw_mouse_callback = (void (*)(GLFWwindow*, double, double)) mouse_callback;
	assert(window != NULL);
	glfwSetCursorPosCallback((GLFWwindow*) window, glfw_mouse_callback);
}

void window_set_scroll(window_t* window, void (*scroll_callback)(window_t*, double, double)) {
	void (*glfw_scroll_callback)(struct GLFWwindow *, double, double);

	glfw_scroll_callback = (void (*)(struct GLFWwindow *, double, double)) scroll_callback;
	assert(window != NULL);
	assert(scroll_callback != NULL);
	glfwSetScrollCallback((GLFWwindow*) window, glfw_scroll_callback);
}

void window_set_mouse_click(window_t* window, void (*mouse_click) (window_t* window, int, int, int)) {
	void (*glfw_mouse_click_callback)(struct GLFWwindow *, int, int, int);

	glfw_mouse_click_callback = (void (*)(struct GLFWwindow *, int, int, int)) mouse_click;
	assert(window != NULL);
	assert(glfw_mouse_click_callback != NULL);
	glfwSetMouseButtonCallback((GLFWwindow*) window, glfw_mouse_click_callback);
}

void window_update(window_t* window) {
	assert(window != NULL);
	glfwSwapBuffers((GLFWwindow*) window);
	glfwPollEvents();
}

bool window_is_key_pressed(window_t* window, int32_t key) {
	return glfwGetKey((GLFWwindow*) window, key) == GLFW_PRESS;
}

int32_t window_height(void) {
	const GLFWvidmode* ret;
	ret = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return ret->height;
}

int32_t window_width(void) {
	const GLFWvidmode* ret;
	ret = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return ret->width;
}

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	(void) window;
    glViewport(0, 0, width, height);
}
