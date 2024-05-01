#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for int32_t, uint32_t

#include "mesh.h"    // for mesh_t

typedef struct model {
	mesh_t** meshes;
	size_t meshes_count;
	const char* directory;
	void (*draw) (const struct model*, uint32_t);
	void (*free) (struct model** model);
} model_t;

int32_t model_new(model_t** model, const char* path);
