#pragma once

#include <stdbool.h>

#include <cglm/types.h>

#include "world.h"

bool raycast_viewport_model(world_t* world, float viewport_x, float viewport_y, mat4 proj, mat4 view);
