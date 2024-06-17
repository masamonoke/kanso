#include "primitive.h"

#include <stdlib.h>

#include <glad/glad.h>
#include <cglm/affine.h>
#include <cglm/affine-pre.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <c_log.h>

#include "shader.h"
#include "camera.h"

struct cube {
	uint32_t shader_program;
	vertex_object_t vo;
};

static int32_t create_cube(void** primitive);

int32_t primitive_new(void** primitive, enum primitive_type type) {
	switch (type) {
		case CUBE:
			return create_cube(primitive);
		default:
			log_error("Unknown primitive type: %d", type);
	}

	return 0;
}

static void draw_cube(void* primitive);

void primitive_free(void** primitive) {
	primitive_model_t** model;

	model = (primitive_model_t**) primitive;
	switch ((*model)->model_data.primitive_type) {
		case CUBE:
			{
				glDeleteVertexArrays(1, &(*model)->model_data.vo.vao);
				glDeleteBuffers(1, &(*model)->model_data.vo.vbo);
				glDeleteBuffers(1, &(*model)->model_data.vo.ebo);

				free(*model);
			}
			break;
		default:
			log_error("Unknown primitive type: %d", (*model)->model_data.primitive_type);
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
	primitive_model_t** cube;

	cube = (primitive_model_t**) primitive;
	*cube = malloc(sizeof(primitive_model_t));

	if (shader_create_program("shaders/light_cube.vert", "shaders/light_cube.frag", &(*cube)->common.shader_program)) {
		log_error("Failed to compile cube shader program");
	}

	glGenVertexArrays(1, &(*cube)->model_data.vo.vao);
	glGenBuffers(1, &(*cube)->model_data.vo.vbo);
	glGenBuffers(1, &(*cube)->model_data.vo.ebo);

	glBindVertexArray((*cube)->model_data.vo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, (*cube)->model_data.vo.vbo);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) sizeof(VERTICES), (float*) VERTICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, (int32_t) 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*) 0); // NOLINT
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	(*cube)->common.draw = draw_cube;

	(*cube)->model_data.primitive_type = CUBE;
	(*cube)->common.type = PRIMITVE_MODEL;

	return 0;
}

static void draw_cube(void* primitive) {
	primitive_model_t* cube;

	cube = (primitive_model_t*) primitive;

	camera_set_view(cube->common.transform.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, cube->common.transform.proj);

	glUseProgram(cube->common.shader_program);
	glBindVertexArray(cube->model_data.vo.vao);

	glm_mat4_identity(cube->common.transform.model);
	glm_translate(cube->common.transform.model, cube->common.position);
	glm_scale(cube->common.transform.model, cube->common.scale);

	shader_set_transform(cube->common.transform, cube->common.shader_program);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}
