#include "glfw_context.h"

#include <stdio.h>

#include <log.h>

#define DEFAULT_WINDOW_SIZE 800, 600

int32_t glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int)) {
	int32_t status;

	status = 0;

	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	*window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, DEFAULT_WINDOW_SIZE);

	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

L_RETURN:
	return status;
}

