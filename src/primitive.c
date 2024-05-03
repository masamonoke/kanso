#include "primitive.h"
#include "vertex_object.h"    // for vertex_object_t

#include <log.h>              // for log_error
#include <cglm/cglm.h>        // for glm_mat4_identity, glm_translate, glm_scale
#include <glad/glad.h>        //for glUseProgram, glBindVertexArray, glDrawArrays

#include <stdlib.h>           // for malloc

struct cube {
	uint32_t shader_program;
	vertex_object_t vo;
};

static int32_t create_cube(void** primitive);

int32_t primitive_new(enum primitive_type type, void** primitive) {
	switch (type) {
		case CUBE:
			return create_cube(primitive);
		default:
			log_error("Unknown primitive type: %d", type);
	}
}

static void draw_cube(void* primitive, struct transform t);

void primitive_draw(enum primitive_type type, void* primitive, struct transform t) {
	switch (type) {
		case CUBE:
			draw_cube(primitive, t);
			break;
		default:
			log_error("Unknown primitive type: %d", type);
	}
}

void primitive_free(enum primitive_type type, void** primitive) {
	switch (type) {
		case CUBE:
			{
				cube_t** cube;

				cube = (cube_t**) primitive;
				glDeleteVertexArrays(1, &(*cube)->vo.vao);
				glDeleteBuffers(1, &(*cube)->vo.vbo);
				glDeleteBuffers(1, &(*cube)->vo.ebo);

				free(*cube);
				log_debug("Freed cube primitive");
			}
			break;
		default:
			log_error("Unknown primitive type: %d", type);
	}
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

const float VERTICES[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static int32_t create_cube(void** primitive) {
	cube_t** cube;

	cube = (cube_t**) primitive;
	*cube = malloc(sizeof(cube_t));

	if (shader_create_program("shaders/light_cube.vert", "shaders/light_cube.frag", &(*cube)->shader_program)) {
		log_error("Failed to compile cube shader program");
	}

	glGenVertexArrays(1, &(*cube)->vo.vao);
	glGenBuffers(1, &(*cube)->vo.vbo);
	glGenBuffers(1, &(*cube)->vo.ebo);

	glBindVertexArray((*cube)->vo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, (*cube)->vo.vbo);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) sizeof(VERTICES), (float*) VERTICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, (int32_t) 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*) 0); // NOLINT
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;
}

static void draw_cube(void* primitive, struct transform t) {
	cube_t* cube;

	cube = (cube_t*) primitive;
	glUseProgram(cube->shader_program);
	glBindVertexArray(cube->vo.vao);
	glm_mat4_identity(t.model);
	glm_translate(t.model, (vec3) { 0.7f, 0.2f, 2.0f });
	glm_scale(t.model, (vec3) { 0.2f, 0.2f, 0.2f });
	shader_set_transform(t, cube->shader_program);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
