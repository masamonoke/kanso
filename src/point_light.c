#include "point_light.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <cglm/types.h>

#include "shader.h"

static void bind_shader(void* light, uint32_t shader_program);

void point_light_new(light_t** light, void* specific_data) {
	struct point_light** point_light;
	struct point_light_specific* point_specific;

	point_light = (point_light_t**) light;
	point_specific = (struct point_light_specific*) specific_data;

	*point_light = malloc(sizeof(point_light_t));
	(*point_light)->specific_data.constant = point_specific->constant;
	(*point_light)->specific_data.linear = point_specific->linear;
	(*point_light)->specific_data.quadratic = point_specific->quadratic;
	memcpy((*point_light)->specific_data.position, &point_specific->position, sizeof(vec3));
	(*point_light)->common.bind_shader = bind_shader;
}

void point_light_free(light_t** light) {
	free(*light);
	*light = NULL;
}

static void bind_shader(void* light, uint32_t shader_program) {
	point_light_t* point_light;

	point_light = (point_light_t*) light;

	glUseProgram(shader_program);

	shader_set_vec3(shader_program, "pointLight.ambient", point_light->common.ambient);
	shader_set_vec3(shader_program, "pointLight.diffuse", point_light->common.diffuse);
	shader_set_vec3(shader_program, "pointLight.specular", point_light->common.specular);

	shader_set_vec3(shader_program, "pointLight.pos", point_light->specific_data.position);
	shader_set_uniform_primitive(shader_program, "pointLight.constant",point_light->specific_data.constant);
	shader_set_uniform_primitive(shader_program, "pointLight.linear", point_light->specific_data.linear);
	shader_set_uniform_primitive(shader_program, "pointLight.quadratic", point_light->specific_data.quadratic);
}
