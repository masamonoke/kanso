#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct vertex_object {
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;
} vertex_object_t;

void vertex_object_create(vertex_object_t* vo);

void vertex_object_bind_vertices(vertex_object_t* vo, float* vertices, size_t verices_len);

void vertex_object_bind_attr(vertex_object_t* vo, size_t att_idx, int32_t stride, size_t offset, size_t num);

void vertex_object_bind_indices(vertex_object_t* vo, uint32_t* indices, size_t indices_len);
