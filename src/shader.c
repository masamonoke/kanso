#include "shader.h"

#include <stdlib.h>
#include <assert.h>

#include <glad/glad.h>
#include <cglm/types.h>
#include <c_log.h>

#include "file.h"

bool shader_create_program(const char* vertex_file, const char* frag_file, uint32_t* shader_program) { // NOLINT
	char* vertex_shader_str;
	char* frag_shader_str;
	uint32_t vertex_shader;
	uint32_t frag_shader;
	int32_t status;
	char info[512];

	assert(vertex_file != NULL);
	assert(frag_file != NULL);
	assert(shader_program != NULL);

	vertex_shader_str = NULL;
	if (!file_read(vertex_file, &vertex_shader_str) && !vertex_shader_str) {
		log_error("Failed to read %s", vertex_file);
		status = -1;
		goto L_RETURN;
	}

	frag_shader_str = NULL;
	if (!file_read(frag_file, &frag_shader_str) && !frag_shader_str) {
		log_error("Failed to read %s", frag_file);
		status = -1;
		goto L_RETURN;
	}

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char* const*) &vertex_shader_str, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		log_error("Vertex shader %s compilation failed: %s", vertex_file, info);
		status = -1;
		goto L_RETURN;
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, (const char* const*) &frag_shader_str, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info);
		log_error("Fragment shader %s compilation failed: %s", frag_file, info);
		status = -1;
		goto L_RETURN;
	}

	*shader_program = glCreateProgram();
	glAttachShader(*shader_program, vertex_shader);
	glAttachShader(*shader_program, frag_shader);
	glLinkProgram(*shader_program);
	glGetProgramiv(*shader_program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(*shader_program, 512, NULL, info);
		log_error("Shader program linking failed: %s", info);
		status = -1;
		goto L_RETURN;
	}


	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	free(vertex_shader_str);
	free(frag_shader_str);
	status = 0;

L_RETURN:

	return status == 0 ? true : false;
}

void shader_set_bool(uint32_t shader_program, const char* name, bool value) {
	assert(name != NULL);
	glUniform1i(glGetUniformLocation(shader_program, name), (int32_t) value);
}

void shader_set_int(uint32_t shader_program, const char* name, int32_t value) {
	assert(name != NULL);
	glUniform1i(glGetUniformLocation(shader_program, name), value);
}

void shader_set_float(uint32_t shader_program, const char* name, float value) {
	assert(name != NULL);
	glUniform1f(glGetUniformLocation(shader_program, name), value);
}

void shader_set_mat4(uint32_t shader_program, const char* name, mat4 mat) {
	assert(name != NULL);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, name), 1, GL_FALSE, mat[0]);
}

void shader_set_transform(struct transform t, uint32_t shader_program) {
	shader_set_mat4(shader_program, "model", t.model);
	shader_set_mat4(shader_program, "view", t.view);
	shader_set_mat4(shader_program, "proj", t.proj);
}

void shader_set_vec3(uint32_t shader_program, const char* name, float vec[3]) {
	assert(name != NULL);
	assert(vec != NULL);

	glUniform3fv(glGetUniformLocation(shader_program, name), 1, vec);
}
