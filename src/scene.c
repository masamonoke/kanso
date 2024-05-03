#include "scene.h"
#include "vertex_object.h"
#include "model.h"
#include "shader.h"
#include "camera.h"
#include "primitive.h"

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <log.h>

#include <assert.h>

struct scene_ctx {
	model_t* model;
	cube_t* cube;
};

static void setup_lights(uint32_t shader_program);


static void draw_models(struct transform t, model_t* model);

static void draw(scene_t* scene);


int32_t scene_new(scene_t** scene) {
	*scene = malloc(sizeof(scene_t));
	(*scene)->scn_ctx = malloc(sizeof(scene_ctx_t));

	model_new(&(*scene)->scn_ctx->model, "assets/backpack/backpack.obj");
	primitive_new(CUBE, (void**) &(*scene)->scn_ctx->cube);

	(*scene)->draw = draw;

	return 0;
}

void scene_free(scene_t** scene) {
	model_free(&(*scene)->scn_ctx->model);
	primitive_free(CUBE, (void**) &(*scene)->scn_ctx->cube);
	free((*scene)->scn_ctx);
	(*scene)->scn_ctx = NULL;
	free(*scene);
	*scene = NULL;
	log_debug("Freed scene");
}


// ------------------------------------------------------[ static functions ]------------------------------------------------------

static void setup_lights(uint32_t shader_program) {
	glUseProgram(shader_program);

	shader_set_vec3(shader_program, "spotLight.pos", (float*) camera_pos());
	shader_set_vec3(shader_program, "spotLight.direction", (float*) camera_front());
	shader_set_vec3(shader_program, "spotLight.ambient", (vec3) { 0.0f, 0.0f, 0.0f });
	shader_set_vec3(shader_program, "spotLight.diffuse", (vec3) { 1.0f, 1.0f, 1.0f });
	shader_set_vec3(shader_program, "spotLight.specular", (vec3) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(shader_program, "spotLight.constant", 1.0f);
	shader_set_uniform_primitive(shader_program, "spotLight.linear", 0.09f);
	shader_set_uniform_primitive(shader_program, "spotLight.quadratic", 0.000032f);
	shader_set_uniform_primitive(shader_program, "spotLight.innerCutOff", cosf(glm_rad(12.5f)));
	shader_set_uniform_primitive(shader_program, "spotLight.outerCutOff", cosf(glm_rad(15.0f)));

	shader_set_vec3(shader_program, "dirLight.direction", (vec3) { -0.2f, -1.0f, -0.3f });
	shader_set_vec3(shader_program, "dirLight.ambient", (vec3) { 0.5f, 0.5f, 0.5f });
	shader_set_vec3(shader_program, "dirLight.diffuse", (vec3) { 0.4f, 0.4f, 0.4f });
	shader_set_vec3(shader_program, "dirLight.specular", (vec3) { 0.5f, 0.5f, 0.5f });

	shader_set_vec3(shader_program, "pointLight.pos", (vec3) { 0.7f, 0.2f, 2.0f });
	shader_set_vec3(shader_program, "pointLight.ambient", (vec3) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(shader_program, "pointLight.diffuse", (vec3) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(shader_program, "pointLight.specular", (vec3) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(shader_program, "pointLight.constant", 1.0f);
	shader_set_uniform_primitive(shader_program, "pointLight.linear", 0.09f);
	shader_set_uniform_primitive(shader_program, "pointLight.quadratic", 0.032f);

	shader_set_vec3(shader_program, "viewPos", (float*) camera_pos());
	shader_set_uniform_primitive(shader_program, "material.shininess", 32.0f);
}

static void draw_models(struct transform t, model_t* model) {
	glUseProgram(model->shader_program);
	glm_mat4_identity(t.model);
	glm_mat4_identity(t.proj);
	glm_mat4_identity(t.view);
	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);
	glm_translate(t.model, (vec3) { 0.0f, 3.0f, 0.0f });
	glm_scale(t.model, (vec3) { 1.0f, 1.0f, 1.0f });
	shader_set_mat4(model->shader_program, "model", t.model);
	shader_set_mat4(model->shader_program, "view", t.view);
	shader_set_mat4(model->shader_program, "proj", t.proj);
	model->draw(model);
}

static void draw (scene_t* scene) {
	struct transform t = TRANSFORM_IDENTITY;

	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);

	primitive_draw(CUBE, (void*) scene->scn_ctx->cube, t);

	setup_lights(scene->scn_ctx->model->shader_program);

	draw_models(t, scene->scn_ctx->model);
}
