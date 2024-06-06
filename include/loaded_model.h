#pragma once

#include <stddef.h>         // for size_t
#include <stdint.h>         // for int32_t

#include "mesh.h"           // for mesh_t
#include "model.h"          // for model_common
#include "vertex_object.h"  // for vertex_object_t

struct loaded_model_data {
	mesh_t** meshes;
	size_t meshes_count;
	const char* directory;
	vertex_object_t model_vo;
};

typedef struct loaded_model {
	struct model_common common;
	struct loaded_model_data model_data;
} loaded_model_t;

__attribute__((warn_unused_result)) int32_t loaded_model_new(loaded_model_t** model, const char* path);

void loaded_model_free(loaded_model_t** model);
