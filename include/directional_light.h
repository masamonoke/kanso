#pragma once

#include <cglm/types.h>

#include "light.h"


struct dir_light_specific {
	vec3 direction;
};

typedef struct dir_light {
	struct light_common common;
	struct dir_light_specific specific_data;
} dir_light_t;

void directional_light_new(light_t** light, void* specific_data);

void directional_light_free(light_t** light);
