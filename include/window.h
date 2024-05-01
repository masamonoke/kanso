#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct window window_t;

/*! @brief Creates new window object. This function implementation derives from library used. Currently window_t is GLFWWindow
 *  @param[in] window_t** window: Pointer to pointer to abstract window
 *  @return int32_t status: 0 is success otherwise is failure
 *  @pointer_lifetime Mallocs memory and caller should free it with window_free() function
*/
int32_t window_new(window_t** window);

/*! @brief Frees window object
 *  @param[in] window_t* window: pointer to abstract window
 *  @pointer_lifetime Pointer *window is freed and set to NULL
*/
void window_free(window_t** window);

/*! @brief Gives current value of window counter. Can be used to calculate delta time or fps counter
 *  @return int32_t status: 0 is success otherwise is failure
*/
float window_context_time(void);

/*! @brief Sets cursor capture and cursor becomes hidden and fixated around window
 *  @param[in] window_t* window: pointer to abstract window
*/
void window_set_capture_cursor(window_t* window);

/*! @brief Sets cursor callback. This function is defined in specified source file that depends on specific library
 *  @param[in] window_t* window: pointer to abstract window
 *  @param[in] void (*mouse_callback)(window_t*, double, double): pointer to function that returns nothing and passes x and y of cursor position
*/
void window_set_cursor(window_t* window, void (*mouse_callback)(window_t*, double, double));

/*! @brief Sets cursor callback. This function is defined in specified source file that depends on specific library
 *  @param[in] window_t* window: pointer to abstract window
 *  @param[in] void (*scroll_callback)(window_t*, double, double): pointer to function that returns nothing and passes up and down offsets
*/
void window_set_scroll(window_t* window, void (*scroll_callback)(window_t*, double, double));

/*! @brief Updates window state and polls for events
 *  @param[in] window_t* window: pointer to abstract window
*/
void window_update(window_t* window);

/*! @brief Checks if passed key is pressed
 *  @param[in] window_t* window: pointer to abstract window
 *  @param[in] int32_t key: key number
*/
bool window_is_key_pressed(window_t* window, int32_t key);
