#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct window window_t;

__attribute__((warn_unused_result))
bool window_new(window_t** window);

void window_free(window_t** window);

__attribute__((warn_unused_result))
float window_context_time(void);

void window_hide_cursor(window_t* window);

void window_reset_cursor(window_t* window);

void window_set_cursor(window_t* window, void (*mouse_callback)(window_t*, double, double));

void window_set_scroll(window_t* window, void (*scroll_callback)(window_t*, double, double));

void window_set_mouse_click(window_t* window, void (*mouse_click) (window_t* window, int, int, int));

void window_update(window_t* window);

int32_t window_height(void);

int32_t window_width(void);

__attribute__((warn_unused_result))
bool window_is_key_pressed(window_t* window, int32_t key);
