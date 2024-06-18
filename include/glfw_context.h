#include <stdint.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>

struct GLFWwindow;

__attribute__((warn_unused_result))
bool glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int));

float glfw_context_time(void);

void glfw_context_capture_cursor(GLFWwindow* window);

void glfw_context_set_cursor_pos_callback(GLFWwindow* window, void (*mouse_callback)(struct GLFWwindow*, double, double));

void glfw_context_set_scroll_callback(GLFWwindow* window, void (*scroll_callback)(struct GLFWwindow*, double, double));

void glfw_context_update_window(GLFWwindow** window);
