#include <stdint.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <log.h>

#include "glfw_context.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int32_t main(void) {
	GLFWwindow* window;
	int32_t status;

	status = 0;
	if (glfw_context_create_window(&window, framebuffer_size_callback) && !window) {
		goto L_RETURN;
	}

	log_info("Created GLFW window");

	while(!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

L_RETURN:
	return status;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	(void) window;
    glViewport(0, 0, width, height);
}
