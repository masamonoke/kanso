#include <stdbool.h>

#include <GLFW/glfw3.h>

struct GLFWwindow;

__attribute__((nonnull(1, 2), warn_unused_result))
bool glfw_context_create_window(GLFWwindow** window, void (*framebuffer_size_callback)(struct GLFWwindow*, int, int));

__attribute__((warn_unused_result))
float glfw_context_time(void);

__attribute__((nonnull(1)))
void glfw_context_capture_cursor(GLFWwindow* window);

__attribute__((nonnull(1, 2)))
void glfw_context_set_cursor_pos_callback(GLFWwindow* window, void (*mouse_callback)(struct GLFWwindow*, double, double));

__attribute__((nonnull(1, 2)))
void glfw_context_set_scroll_callback(GLFWwindow* window, void (*scroll_callback)(struct GLFWwindow*, double, double));

__attribute__((nonnull(1, 2)))
void glfw_context_set_mouse_click_callback(GLFWwindow* window, void (*mouse_click_callback)(struct GLFWwindow*, int, int, int));

__attribute__((nonnull(1)))
void glfw_context_update_window(GLFWwindow* window);
