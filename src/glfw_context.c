#include <stdio.h>          // for NULL

#include <json_object.h>    // for json_object_get_int, json_object_object_g...
#include <json_util.h>      // for json_object_from_file
#include <glad/glad.h>      // for gladLoadGLLoader, GLADloadproc, GL_DEPTH_...
#include <GLFW/glfw3.h>     // for glfwWindowHint, GLFWwindow, glfwCreateWindow

#include "custom_logger.h"  // for custom_log_error, custom_log_info, custom...
#include "glfw_context.h"

#define DEFAULT_WINDOW_SIZE 800, 600
#define DEFAULT_TITLE "Kanso Engine"

int32_t glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int)) {
	int32_t status;
	int32_t framebuffer_width;
	int32_t framebuffer_height;
	struct json_object* j;

	status = 0;

	// valgrind and address sanitizer may show gpu memory leak after this function
	// and it not freed after glfwTerminate() call
	// but that is not glfw problem or anything else but PU drivers (libraries outside glfw control)
	// like to allocate some global memory and keep it allocated even when GL/Vulkan context is destroyed.
	// They have some globals/statics that are never released.
	// So if valgrind shows memory coming from them, this is expected.
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

		custom_log_info("Reading window config from file");
		json_object_object_get_ex(j, "width", &width_json);
		json_object_object_get_ex(j, "height", &height_json);

		if (width_json && height_json) {
			*window = glfwCreateWindow(json_object_get_int(width_json), json_object_get_int(height_json),DEFAULT_TITLE, NULL, NULL);
		} else {
			custom_log_error("Failed to read json height or width value");
			json_object_put(j);
			goto L_DEFAULT_WINDOW_INIT;
		}

		if (json_object_put(j)) {
			custom_log_debug("Freed json object");
		} else {
			custom_log_error("Failed to free json object");
		}

	} else {
		custom_log_info("Window config is not found. Using default values");
L_DEFAULT_WINDOW_INIT:
		*window = glfwCreateWindow(DEFAULT_WINDOW_SIZE, DEFAULT_TITLE, NULL, NULL);
	}

	if (window == NULL) {
		custom_log_error("Failed to create GLFW window\n");
		glfwTerminate();
		status = -1;
		goto L_RETURN;
	}
	glfwMakeContextCurrent(*window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		custom_log_error("Failed to initialize GLAD\n");
		status = -1;
		goto L_RETURN;
	}

	glEnable(GL_DEPTH_TEST);

	glfwGetFramebufferSize(*window, &framebuffer_width, &framebuffer_height);
	glViewport(0, 0, framebuffer_width, framebuffer_height);

	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

	custom_log_info("Created GLFW window");

L_RETURN:

	return status;
}

float glfw_context_time(void) {
	return (float) glfwGetTime();
}

void glfw_context_capture_cursor(GLFWwindow* window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void glfw_context_set_cursor_pos_callback(GLFWwindow* window, void (*mouse_callback)(struct GLFWwindow*, double, double)) {
	glfwSetCursorPosCallback(window, mouse_callback);
}

void glfw_context_set_scroll_callback(GLFWwindow* window, void (*scroll_callback)(struct GLFWwindow*, double, double)) {
	glfwSetScrollCallback(window, scroll_callback);
}

void glfw_context_update_window(GLFWwindow** window) {
	glfwSwapBuffers(*window);
	glfwPollEvents();
}
