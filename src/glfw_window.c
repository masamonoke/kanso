#include "window.h"

#include <stdbool.h>
#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_context.h"

struct GLFWwindow;

struct window {
	// WARNING: do not use this field. Use (GLFWWindow*) window_t_ptr cast instead
	GLFWwindow* _glfw_ctx;
};

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);

bool window_new(window_t** window) {
	return glfw_context_create_window((GLFWwindow**) window, framebuffer_size_callback);
}

#include <stdlib.h>
void window_free(window_t** window) {
	glfwDestroyWindow((GLFWwindow*) *window);
	glfwTerminate();
	*window = NULL;
}

float window_context_time(void) {
	return glfw_context_time();
}

void window_set_capture_cursor(window_t* window) {
	glfw_context_capture_cursor((GLFWwindow*) window);
}

void window_set_cursor(window_t* window, void (*mouse_callback)(window_t*, double, double)) {
	void (*glfw_mouse_callback) (GLFWwindow*, double, double);
	glfw_mouse_callback = (void (*)(GLFWwindow*, double, double)) mouse_callback;
	glfw_context_set_cursor_pos_callback((GLFWwindow*) window, glfw_mouse_callback);
}

void window_set_scroll(window_t* window, void (*scroll_callback)(window_t*, double, double)) {
	void (*glfw_scroll_callback)(struct GLFWwindow *, double, double);

	glfw_scroll_callback = (void (*)(struct GLFWwindow *, double, double)) scroll_callback;
	glfw_context_set_scroll_callback((GLFWwindow*) window, glfw_scroll_callback);
}

void window_set_mouse_click(window_t* window, void (*mouse_click) (window_t* window, int, int, int)) {
	void (*glfw_mouse_click_callback)(struct GLFWwindow *, int, int, int);

	glfw_mouse_click_callback = (void (*)(struct GLFWwindow *, int, int, int)) mouse_click;
	glfw_context_set_mouse_click_callback((GLFWwindow*) window, glfw_mouse_click_callback);
}

void window_update(window_t* window) {
	glfw_context_update_window((GLFWwindow*) window);
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
