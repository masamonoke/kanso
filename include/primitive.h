#pragma once

#include <stdint.h> // for int32_t

#include "vertex_object.h"    // for vertex_object_t
#include "model.h"

enum primitive_type {
	CUBE,
	UNDEFINED_PRIMITIVE
};

struct primitive_model_data {
	enum primitive_type primitive_type;
	vertex_object_t vo;
};

typedef struct primitive_model {
	struct model_common common;
	struct primitive_model_data model_data;
} primitive_model_t;

__attribute__((warn_unused_result)) int32_t primitive_new(void** primitive, enum primitive_type type);

void primitive_free(void** primitive);
