#include <glad/glad.h>

#include <stdlib.h>

#include "window.h"
#include "glfw_context.h"

struct window {
	// WARNING: do not use this field. Use (GLFWWindow*) window_t_ptr cast instead
	GLFWwindow* _glfw_ctx;
};

/*! @brief Callback for GLFW to update viewport after window resized
 *  @param[in] GLFWwindow* window: current GLFW window pointer
 *  @param[in] int32_t width: current width of GLFW window
 *  @param[in] int32_t height: current height of GLFW window
*/
static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);

int32_t window_new(window_t** window) {
	return glfw_context_create_window((GLFWwindow**) window, framebuffer_size_callback);
}

void window_free(window_t **window) {
	// glfwTerminate call cleans all window context
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

void window_update(window_t* window) {
	glfw_context_update_window((GLFWwindow**) &window);
}

bool window_is_key_pressed(window_t* window, int32_t key) {
	return glfwGetKey((GLFWwindow*) window, key) == GLFW_PRESS;
}

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	(void) window;
    glViewport(0, 0, width, height);
}