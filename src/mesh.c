#include "mesh.h"
#include "shader.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glad/glad.h>
#include <log.h>

static void setup_vertex_objects(vertex_object_t* vo, vertex_vector_t vertices, int32_vector_t indices) {
	glGenVertexArrays(1, &vo->vao);
	glGenBuffers(1, &vo->vbo);
	glGenBuffers(1, &vo->ebo);

	glBindVertexArray(vo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vo->vbo);

	glBufferData(GL_ARRAY_BUFFER, (int32_t) (vertices.size * sizeof(struct vertex)), &vertices.vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vo->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int32_t) (indices.size * sizeof(int32_t)), &indices.array[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) offsetof(struct vertex, normal)); // NOLINT
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*) offsetof(struct vertex, tex_coords)); // NOLINT

	glBindVertexArray(0);
}

static void draw_mesh(mesh_t* mesh, uint32_t shader_program) {
	uint32_t diffuse_nr;
	uint32_t specular_nr;
	uint32_t height_nr;
	uint32_t normal_nr;
	size_t i;
	char name[64];
	char number[32];

	diffuse_nr = 1;
	specular_nr = 1;
	height_nr = 1;
	normal_nr = 1;

	for (i = 0; i < mesh->textures_vector.size; i++) {
		strcpy(name, "material.");
		glActiveTexture(GL_TEXTURE0 + i);
		strcpy(name, mesh->textures_vector.textures[i].type);
		if (0 == strcmp(name, "texture_diffuse")) {
			sprintf(number, "%d", diffuse_nr++);
		} else if (0 == strcmp(name, "texture_specular")) {
			sprintf(number, "%d", specular_nr++);
		} else if (0 == strcmp(name, "texture_height")) {
			sprintf(number, "%d", height_nr++);
		} else if (0 == strcmp(name, "texture_normal")) {
			sprintf(number, "%d", normal_nr++);
		} else {
			log_error("Unknown texture type: %s", name);
			return;
		}
		strcat(name, number);
		shader_set_uniform_primitive(shader_program, name, (int32_t) i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures_vector.textures[i].id);
	}

	glBindVertexArray(mesh->vo.vao);
	glDrawElements(GL_TRIANGLES, (int32_t) mesh->indices_vector.size, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

int32_t mesh_new(mesh_t** mesh, vertex_vector_t vertices, int32_vector_t indices, texture_vector_t textures) {
	*mesh = malloc(sizeof(mesh_t));
	(*mesh)->vertices_vector = vertices;
	(*mesh)->textures_vector = textures;
	(*mesh)->indices_vector = indices;
	(*mesh)->draw = draw_mesh;

	setup_vertex_objects(&(*mesh)->vo, (*mesh)->vertices_vector, (*mesh)->indices_vector);

	return 0;
}

void mesh_delete(mesh_t** mesh) {
	free((*mesh)->indices_vector.array);
	(*mesh)->indices_vector.array = NULL;
	free((*mesh)->vertices_vector.vertices);
	(*mesh)->vertices_vector.vertices = NULL;
	free((*mesh)->textures_vector.textures->path);
	(*mesh)->textures_vector.textures->path = NULL;
	free((*mesh)->textures_vector.textures->type);
	(*mesh)->textures_vector.textures->type = NULL;
	free((*mesh)->textures_vector.textures);
	(*mesh)->textures_vector.textures = NULL;
	free(*mesh);
	*mesh = NULL;
	log_debug("Freed mesh");
}

