#pragma once

#include <cglm/types.h>

#include "light.h"

struct point_light_specific {
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

typedef struct point_light {
	struct light_common common;
	struct point_light_specific specific_data;
} point_light_t;

void point_light_new(light_t** light, void* specific_data);

void point_light_free(light_t** light);
