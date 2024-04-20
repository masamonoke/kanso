#include "shader.h"
#include "file.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <log.h>

#include <stdio.h>
#include <stdlib.h>

uint32_t shader_create_program(const char* vertex_file, const char* frag_file) { // NOLINT
	char* vertex_shader_str;
	char* frag_shader_str;
	uint32_t vertex_shader;
	uint32_t frag_shader;
	uint32_t shader_program;
	int32_t status;
	char info[512];

	vertex_shader_str = NULL;
	if (file_read(vertex_file, &vertex_shader_str) && !vertex_shader_str) {
		log_error("Failed to read %s", vertex_file);
		return -1;
	}

	frag_shader_str = NULL;
	if (file_read(frag_file, &frag_shader_str) && !frag_shader_str) {
		log_error("Failed to read %s", frag_file);
		return -1;
	}

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char* const*) &vertex_shader_str, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		log_error("Vertex shader %s compilation failed: %s", vertex_file, info);
		return -1;
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, (const char* const*) &frag_shader_str, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info);
		log_error("Fragment shader %s compilation failed: %s", frag_file, info);
		return -1;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(shader_program, 512, NULL, info);
		log_info("Shader program linking failed: %s", info);
		shader_program = -1;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	free(vertex_shader_str);
	free(frag_shader_str);

	return shader_program;
}

void shader_set_bool(uint32_t shader_program, const char* name, bool value) {
	glUniform1i(glGetUniformLocation(shader_program, name), (int32_t) value);
}

void shader_set_int(uint32_t shader_program, const char* name, int32_t value) {
	glUniform1i(glGetUniformLocation(shader_program, name), value);
}

void shader_set_float(uint32_t shader_program, const char* name, float value) {
	glUniform1f(glGetUniformLocation(shader_program, name), value);
}

void shader_set_mat4(uint32_t shader_program, const char* name, mat4 mat) {
	glUniformMatrix4fv(glGetUniformLocation(shader_program, name), 1, GL_FALSE, mat[0]);
}

void shader_set_transform(struct transform t, uint32_t shader_program) {
	shader_set_mat4(shader_program, "model", t.model);
	shader_set_mat4(shader_program, "view", t.view);
	shader_set_mat4(shader_program, "proj", t.proj);
}

void shader_set_vec3(uint32_t shader_program, const char* name, float vec[3]) {
	glUniform3fv(glGetUniformLocation(shader_program, name), 1, vec);
}