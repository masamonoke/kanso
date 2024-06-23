#include "directional_light.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glad/glad.h>
#include <cglm/types.h>

#include "shader.h"

static void bind_shader(void* light, uint32_t shader_program);

void directional_light_new(light_t** light, void* specific_data) {
	struct dir_light** dir_light;
	struct dir_light_specific* dir_specific;

	assert(light != NULL);
	assert(specific_data != NULL);

	dir_light = (dir_light_t**) light;
	dir_specific = (struct dir_light_specific*) specific_data;

	*dir_light = malloc(sizeof(dir_light_t));
	memcpy((*dir_light)->specific_data.direction, dir_specific->direction, sizeof(vec3));
	(*dir_light)->common.bind_shader = bind_shader;
}

void directional_light_free(light_t** light) {
	assert(light != NULL);
	free(*light);
	*light = NULL;
}


static void bind_shader(void* light, uint32_t shader_program) {
	dir_light_t* dir_light;

	dir_light = (dir_light_t*) light;

	glUseProgram(shader_program);

	shader_set_vec3(shader_program, "dirLight.ambient", dir_light->common.ambient);
	shader_set_vec3(shader_program, "dirLight.diffuse", dir_light->common.diffuse);
	shader_set_vec3(shader_program, "dirLight.specular", dir_light->common.specular);

	shader_set_vec3(shader_program, "dirLight.direction", dir_light->specific_data.direction);
}
