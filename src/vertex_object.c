#include "vertex_object.h"

#include <glad/glad.h>

void vertex_object_create(vertex_object_t* vo) {
	glGenVertexArrays(1, &vo->vao);
	glGenBuffers(1, &vo->vbo);
	glGenBuffers(1, &vo->ebo);
}

void vertex_object_bind_vertices(vertex_object_t* vo, float* vertices, size_t verices_len) {
	glBindVertexArray(vo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vo->vbo);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) verices_len, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void vertex_object_bind_attr(vertex_object_t* vo, size_t att_idx, int32_t stride, size_t offset, size_t num) {
	glBindVertexArray(vo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vo->vbo);
	glVertexAttribPointer(att_idx, num, GL_FLOAT, GL_FALSE, stride, (const void*) offset); // NOLINT
	glEnableVertexAttribArray(att_idx);
}

void vertex_object_bind_indices(vertex_object_t* vo, uint32_t* indices, size_t indices_len) {
	glBindVertexArray(vo->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vo->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) indices_len, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

