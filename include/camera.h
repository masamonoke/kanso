#pragma once

#include <cglm/types.h>

#include "window.h"

#define MAX_FOV 150.0f
#define MIN_FOV 90.0f

/*! @brief Camera specific data. There is only one camera in this build.
*/
struct camera {
	vec3 pos;
	vec3 front;
	vec3 up;
	float fov;
};

typedef struct camera camera_t;

/*! @brief Initiates all callbacks that camera needs like cursor position or scroll input
 *  @param[in] window_t* window: pointer to abstract window
*/
void camera_init_callbacks(window_t* window);

/*! @brief Gives camera object to get data from it
 * @return const camera_t* camera: camera object
*/
const camera_t* camera_get(void);

/*! @brief Sets view 4x4 matrix from model-view-projection model
 * @param[in]: mat4 view: cglm/cglm.h mat4 object
*/
void camera_set_view(mat4 view);

/*! @brief Sets new fov
 *  @param[in] float fov
*/
void camera_set_fov(float fov);

/*! @brief Sets new front vector that consits of 3 components
 *  @param[in] float* front: float[3] array
*/
void camera_set_front(float* front);

/*! @brief Updates camera after user input and after window events
 *  @param[in] window_t* window: pointer to abstract window
*/
void camera_update(window_t* window);

/*! @brief Returns current camera's fov
 * @return float fov: field of view
*/
float camera_fov(void);

/*! @brief Returns current camera's position vector float[3]
 * @return float* pos: float[3] array
*/
const float* camera_pos(void);

/*! @brief Returns current camera's front vector float[3]
 * @return float* pos: float[3] array
*/
const float* camera_front(void);

/*! @brief Moves camera front with specified speed
 * @param[in] float camera_speed
*/
void camera_move_front(float camera_speed);

/*! @brief Moves camera back with specified speed
 * @param[in] float camera_speed
*/
void camera_move_back(float camera_speed);

/*! @brief Moves camera left with specified speed
 * @param[in] float camera_speed
*/
void camera_move_left(float camera_speed);

/*! @brief Moves camera right with specified speed
 * @param[in] float camera_speed
*/
void camera_move_right(float camera_speed);

/*! @brief Moves camera up with specified speed
 * @param[in] float camera_speed
*/
void camera_move_up(float camera_speed);

/*! @brief Moves camera down with specified speed
 * @param[in] float camera_speed
*/
void camera_move_down(float camera_speed);

