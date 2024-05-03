#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for int32_t, uint32_t

#include "mesh.h"    // for mesh_t
#include "vertex_object.h"  // for vertex_object_t

typedef struct model {
	mesh_t** meshes;
	size_t meshes_count;
	const char* directory;
	vertex_object_t model_vo;
	void (*draw) (const struct model*);
	uint32_t shader_program;
} model_t;

int32_t model_new(model_t** model, const char* path);

void model_free(model_t** model);
