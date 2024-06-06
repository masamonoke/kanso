#include <stdint.h>      // for uint32_t
#include <stdlib.h>      // for free, malloc, NULL
#include <string.h>      // for memcpy

#include <glad/glad.h>   // for glUseProgram
#include "cglm/types.h"  // for vec3

#include "spot_light.h"
#include "shader.h"      // for shader_set_vec3, shader_set_uniform_primitive
#include "camera.h"

static void bind_shader(void* light, uint32_t shader_program);

void spot_light_new(light_t** light, void* specific_data) {
	struct spot_light** spot_light;
	struct spot_light_specific* spot_specific;

	spot_light = (spot_light_t**) light;
	spot_specific = (struct spot_light_specific*) specific_data;

	*spot_light = malloc(sizeof(spot_light_t));
	(*spot_light)->specific_data.constant = spot_specific->constant;
	(*spot_light)->specific_data.linear = spot_specific->linear;
	(*spot_light)->specific_data.quadratic = spot_specific->quadratic;
	(*spot_light)->specific_data.inner_cut_off = spot_specific->inner_cut_off;
	(*spot_light)->specific_data.outer_cut_off = spot_specific->outer_cut_off;

	memcpy((*spot_light)->specific_data.position, spot_specific->position, sizeof(vec3));
	memcpy((*spot_light)->specific_data.direction, spot_specific->direction, sizeof(vec3));
	(*spot_light)->common.bind_shader = bind_shader;
}

void spot_light_free(light_t** light) {
	free(*light);
	*light = NULL;
}

static void bind_shader(void* light, uint32_t shader_program) {
	spot_light_t* spot_light;

	spot_light = (spot_light_t*) light;
	memcpy(spot_light->specific_data.position, camera_pos(), sizeof(vec3));
	memcpy(spot_light->specific_data.direction, camera_front(), sizeof(vec3));
	glUseProgram(shader_program);

	shader_set_vec3(shader_program, "spotLight.ambient", spot_light->common.ambient);
	shader_set_vec3(shader_program, "spotLight.diffuse", spot_light->common.diffuse);
	shader_set_vec3(shader_program, "spotLight.specular", spot_light->common.specular);
	shader_set_vec3(shader_program, "spotLight.direction", spot_light->specific_data.direction);
	shader_set_vec3(shader_program, "spotLight.pos", spot_light->specific_data.position);
	shader_set_uniform_primitive(shader_program, "spotLight.constant",spot_light->specific_data.constant);
	shader_set_uniform_primitive(shader_program, "spotLight.linear", spot_light->specific_data.linear);
	shader_set_uniform_primitive(shader_program, "spotLight.quadratic", spot_light->specific_data.quadratic);
	shader_set_uniform_primitive(shader_program, "spotLight.innerCutOff", spot_light->specific_data.inner_cut_off);
	shader_set_uniform_primitive(shader_program, "spotLight.outerCutOff", spot_light->specific_data.outer_cut_off);
}
