#pragma once

#include <stdint.h>

typedef struct scene_ctx scene_ctx_t;

typedef struct scene {
	scene_ctx_t* scn_ctx;
	void (*draw) (struct scene*);
} scene_t;

int32_t scene_new(scene_t** scene);

void scene_free(scene_t** scene);
