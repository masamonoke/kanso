#pragma once

#include <stddef.h>
#include <stdint.h>

#include "mesh.h"
#include "model.h"
#include "vertex_object.h"

struct loaded_model_data {
	mesh_t** meshes;
	size_t meshes_count;
	const char* directory;
	vertex_object_t model_vo;
	uint32_t* _ref_count; // count of objects that use the same mesh ptr
};

typedef struct loaded_model {
	struct model_common common;
	struct loaded_model_data model_data;
} loaded_model_t;

__attribute__((nonnull(1, 2), warn_unused_result))
bool loaded_model_new(loaded_model_t** model, const char* path);

__attribute__((nonnull(1)))
void loaded_model_free(loaded_model_t** model);
