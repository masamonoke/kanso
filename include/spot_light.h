#pragma once

#include <cglm/types.h>

#include "light.h"

struct spot_light_specific {
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float inner_cut_off;
	float outer_cut_off;
};

typedef struct spot_light {
	struct light_common common;
	struct spot_light_specific specific_data;
} spot_light_t;


__attribute__((nonnull(1, 2)))
void spot_light_new(light_t** light, void* specific_data);

__attribute__((nonnull(1)))
void spot_light_free(light_t** light);
