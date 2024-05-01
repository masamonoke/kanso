#include <log.h>
#include <glad/glad.h>

#include "glfw_context.h"

#include <stdio.h>


// TODO: get this from config
#define DEFAULT_WINDOW_SIZE 800, 600
#define DEFAULT_TITLE "Kanso Engine"

int32_t glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int)) {
	int32_t status;
	int32_t framebuffer_width;
	int32_t framebuffer_height;

	status = 0;

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
	*window = glfwCreateWindow(DEFAULT_WINDOW_SIZE, DEFAULT_TITLE, NULL, NULL);
	if (window == NULL) {
		log_error("Failed to create GLFW window\n");
		glfwTerminate();
		status = -1;
		goto L_RETURN;
	}
	glfwMakeContextCurrent(*window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		log_error("Failed to initialize GLAD\n");
		status = -1;
		goto L_RETURN;
	}

	glEnable(GL_DEPTH_TEST);

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
