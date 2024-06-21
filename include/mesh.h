#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <cglm/types.h>

#include "texture.h"
#include "vertex_object.h"

typedef struct vertex {
	vec3 pos;
	vec3 normal;
	vec3 tex_coords;
} vertex_t;

typedef struct vertex_vector {
	struct vertex* vertices;
	size_t size;
} vertex_vector_t;

typedef struct int32_vector {
	int32_t* array;
	size_t size;
} int32_vector_t;

typedef struct texture_vector {
	texture_t* textures;
	size_t size;
} texture_vector_t;

typedef struct mesh {
	vertex_vector_t vertices_vector;
	int32_vector_t indices_vector;
	texture_vector_t textures_vector;
	vertex_object_t vo;
	void (*draw) (struct mesh*, uint32_t);
} mesh_t;

/*
	Creates new mesh object. Mallocs memory and to clean it you need to call mesh_delete()
*/
__attribute__((nonnull(1)))
void mesh_new(mesh_t** mesh, vertex_vector_t vertices, int32_vector_t indices, texture_vector_t textures);

/*
	Delete mesh object
*/
__attribute__((nonnull(1)))
void mesh_delete(mesh_t** mesh);
