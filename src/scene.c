#include "scene.h"
#include "vertex_object.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <log.h>

#include <assert.h>

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


struct scene_ctx {
	vertex_object_t vo;
	vertex_object_t light_vo;
	uint32_t light_shader_program;
	uint32_t model_shader_program;
	uint32_t texture;
	uint32_t specular_map;
	model_t* model;
};

static void setup_lights(uint32_t shader_program);

static void draw_lights(struct transform t, uint32_t shader_program, uint32_t vao);

static void draw_models(struct transform t, uint32_t shader_program, model_t* model);

static void draw(scene_t* scene);


int32_t scene_new(scene_t** scene) {
	*scene = malloc(sizeof(scene_t));
	(*scene)->scn_ctx = malloc(sizeof(scene_ctx_t));

	(*scene)->scn_ctx->light_shader_program = shader_create_program("shaders/light_cube.vert", "shaders/light_cube.frag");
	(*scene)->scn_ctx->model_shader_program = shader_create_program("shaders/backpack.vert", "shaders/backpack.frag");

	vertex_object_create(&(*scene)->scn_ctx->vo);
	vertex_object_bind_vertices(&(*scene)->scn_ctx->vo, (float*) VERTICES, sizeof(VERTICES));
	vertex_object_bind_attr(&(*scene)->scn_ctx->vo, 0, 8 * sizeof(float), 0, 3);
	vertex_object_bind_attr(&(*scene)->scn_ctx->vo, 1, 8 * sizeof(float), 3 * sizeof(float), 3);
	vertex_object_bind_attr(&(*scene)->scn_ctx->vo, 2, 8 * sizeof(float), 6 * sizeof(float), 2);

	vertex_object_create(&(*scene)->scn_ctx->light_vo);
	vertex_object_bind_vertices(&(*scene)->scn_ctx->light_vo, (float*) VERTICES, sizeof(VERTICES));
	vertex_object_bind_attr(&(*scene)->scn_ctx->light_vo, 0, 8 * sizeof(float), 0, 3);

	model_new(&(*scene)->scn_ctx->model, "assets/backpack/backpack.obj");

	(*scene)->draw = draw;

	return 0;
}

void scene_free(scene_t** scene) {
	model_free(&(*scene)->scn_ctx->model);
	glDeleteVertexArrays(1, &(*scene)->scn_ctx->vo.vao);
	glDeleteBuffers(1, &(*scene)->scn_ctx->vo.vbo);
	glDeleteBuffers(1, &(*scene)->scn_ctx->vo.ebo);
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
	shader_set_vec3(shader_program, "spotLight.ambient", (float[]) { 0.0f, 0.0f, 0.0f });
	shader_set_vec3(shader_program, "spotLight.diffuse", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_vec3(shader_program, "spotLight.specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(shader_program, "spotLight.constant", 1.0f);
	shader_set_uniform_primitive(shader_program, "spotLight.linear", 0.09f);
	shader_set_uniform_primitive(shader_program, "spotLight.quadratic", 0.000032f);
	shader_set_uniform_primitive(shader_program, "spotLight.innerCutOff", cosf(glm_rad(12.5f)));
	shader_set_uniform_primitive(shader_program, "spotLight.outerCutOff", cosf(glm_rad(15.0f)));

	shader_set_vec3(shader_program, "dirLight.direction", (float[]) { -0.2f, -1.0f, -0.3f });
	shader_set_vec3(shader_program, "dirLight.ambient", (float[]) { 0.5f, 0.5f, 0.5f });
	shader_set_vec3(shader_program, "dirLight.diffuse", (float[]) { 0.4f, 0.4f, 0.4f });
	shader_set_vec3(shader_program, "dirLight.specular", (float[]) { 0.5f, 0.5f, 0.5f });

	shader_set_vec3(shader_program, "pointLight.pos", (float[]) { 0.7f, 0.2f, 2.0f });
	shader_set_vec3(shader_program, "pointLight.ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(shader_program, "pointLight.diffuse", (float[]) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(shader_program, "pointLight.specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(shader_program, "pointLight.constant", 1.0f);
	shader_set_uniform_primitive(shader_program, "pointLight.linear", 0.09f);
	shader_set_uniform_primitive(shader_program, "pointLight.quadratic", 0.032f);

	shader_set_vec3(shader_program, "viewPos", (float*) camera_pos());
	shader_set_uniform_primitive(shader_program, "material.shininess", 32.0f);
}

static void draw_lights(struct transform t, uint32_t shader_program, uint32_t vao) { // NOLINT
	glUseProgram(shader_program);
	glBindVertexArray(vao);
	glm_mat4_identity(t.model);
	glm_translate(t.model, (float[]) { 0.7f, 0.2f, 2.0f });
	glm_scale(t.model, (float[]) { 0.2f, 0.2f, 0.2f });
	shader_set_transform(t, shader_program);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void draw_models(struct transform t, uint32_t shader_program, model_t* model) {
	glUseProgram(shader_program);
	glm_mat4_identity(t.model);
	glm_mat4_identity(t.proj);
	glm_mat4_identity(t.view);
	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);
	glm_translate(t.model, (float[]) { 0.0f, 3.0f, 0.0f });
	glm_scale(t.model, (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_mat4(shader_program, "model", t.model);
	shader_set_mat4(shader_program, "view", t.view);
	shader_set_mat4(shader_program, "proj", t.proj);
	model->draw(model, shader_program);
}

static void draw (scene_t* scene) {
	struct transform t = TRANSFORM_IDENTITY;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene->scn_ctx->texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene->scn_ctx->specular_map);

	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);

	draw_lights(t, scene->scn_ctx->light_shader_program, scene->scn_ctx->light_vo.vao);

	setup_lights(scene->scn_ctx->model_shader_program);

	draw_models(t, scene->scn_ctx->model_shader_program, scene->scn_ctx->model);
}
