#include "app.h"
#include "glad/glad.h"
#include "glfw_context.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

#include <log.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include <stdlib.h>
#include <memory.h>

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

const size_t BOXES_NUM = 10;

const float CUBE_POSITIONS[][3] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 2.0f, 5.0f, -15.0f },
	{ -1.5f, -2.2f, -2.5f },
	{ -3.8f, -2.0f, -12.3f },
	{ 2.4f, -0.4f, -3.5f },
	{ -1.7f, 3.0f, -7.5f },
	{ 1.3f, -2.0f, -2.5f },
	{ 1.5f, 2.0f, -2.5f },
	{ 1.5f, 0.2f, -1.5f },
	{ -1.3f, 1.0f, -1.5f }
};

const float POINT_LIGHT_POSITIONS[][3] = {
	{ 0.7f, 0.2f, 2.0f },
	{ 2.3f, -3.3f, -4.0f },
	{ -4.0f, 2.0f, -12.0f },
	{ 0.0f, 0.0f, -3.0f }
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	(void) window;
    glViewport(0, 0, width, height);
}


static void input(app_state_t* state) {
	if (glfwGetKey(state->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		state->close = true;
	}
}

// TODO: make glm_vec3_float_mul func
static void update(app_state_t* state) {
	struct transform t = TRANSFORM_IDENTITY;
	size_t i;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state->texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, state->specular_map);

	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);

	// draw cube
	glUseProgram(state->shader_program);
	shader_set_vec3(state->shader_program, "dirLight.direction", (float[]) { -0.2f, -1.0f, -0.3f });
	shader_set_vec3(state->shader_program, "dirLight.ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(state->shader_program, "dirLight.specular", (float[]) { 0.5f, 0.5f, 0.5f });

	shader_set_vec3(state->shader_program, "pointLights[0].pos", (float*) POINT_LIGHT_POSITIONS[0]);
	shader_set_vec3(state->shader_program, "pointLights[0].ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(state->shader_program, "pointLights[0].diffuse", (float[]) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(state->shader_program, "pointLights[0].specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(state->shader_program, "pointLights[0].constant", 1.0f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[0].linear", 0.09f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[0].quadratic", 0.032f);

	shader_set_vec3(state->shader_program, "pointLights[1].pos", (float*) POINT_LIGHT_POSITIONS[1]);
	shader_set_vec3(state->shader_program, "pointLights[1].ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(state->shader_program, "pointLights[1].diffuse", (float[]) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(state->shader_program, "pointLights[1].specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(state->shader_program, "pointLights[1].constant", 1.0f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[1].linear", 0.09f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[1].quadratic", 0.032f);

	shader_set_vec3(state->shader_program, "pointLights[2].pos", (float*) POINT_LIGHT_POSITIONS[2]);
	shader_set_vec3(state->shader_program, "pointLights[2].ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(state->shader_program, "pointLights[2].diffuse", (float[]) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(state->shader_program, "pointLights[2].specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(state->shader_program, "pointLights[2].constant", 1.0f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[2].linear", 0.09f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[2].quadratic", 0.032f);

	shader_set_vec3(state->shader_program, "pointLights[3].pos", (float*) POINT_LIGHT_POSITIONS[3]);
	shader_set_vec3(state->shader_program, "pointLights[3].ambient", (float[]) { 0.05f, 0.05f, 0.05f });
	shader_set_vec3(state->shader_program, "pointLights[3].diffuse", (float[]) { 0.8f, 0.8f, 0.8f });
	shader_set_vec3(state->shader_program, "pointLights[3].specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(state->shader_program, "pointLights[3].constant", 1.0f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[3].linear", 0.09f);
	shader_set_uniform_primitive(state->shader_program, "pointLights[3].quadratic", 0.032f);

	shader_set_vec3(state->shader_program, "spotLight.pos", (float*) camera_pos());
	shader_set_vec3(state->shader_program, "spotLight.direction", (float*) camera_front());
	shader_set_vec3(state->shader_program, "spotLight.ambient", (float[]) { 0.0f, 0.0f, 0.0f });
	shader_set_vec3(state->shader_program, "spotLight.diffuse", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_vec3(state->shader_program, "spotLight.specular", (float[]) { 1.0f, 1.0f, 1.0f });
	shader_set_uniform_primitive(state->shader_program, "spotLight.constant", 1.0f);
	shader_set_uniform_primitive(state->shader_program, "spotLight.linear", 0.09f);
	shader_set_uniform_primitive(state->shader_program, "spotLight.quadratic", 0.000032f);
	shader_set_uniform_primitive(state->shader_program, "spotLight.innerCutOff", cosf(glm_rad(12.5f)));
	shader_set_uniform_primitive(state->shader_program, "spotLight.outerCutOff", cosf(glm_rad(15.0f)));

	shader_set_vec3(state->shader_program, "dirLight.direction", (float[]) { -0.2f, -1.0f, -0.3f });
	shader_set_vec3(state->shader_program, "dirLight.ambient", (float[]) { 0.5f, 0.5f, 0.5f });
	shader_set_vec3(state->shader_program, "dirLight.diffuse", (float[]) { 0.4f, 0.4f, 0.4f });
	shader_set_vec3(state->shader_program, "dirLight.specular", (float[]) { 0.5f, 0.5f, 0.5f });

	shader_set_vec3(state->shader_program, "viewPos", (float*) camera_pos());
	shader_set_uniform_primitive(state->shader_program, "material.shininess", 32.0f);

	shader_set_transform(t, state->shader_program);
	glBindVertexArray(state->vo.vao);
	for (i = 0; i < BOXES_NUM; i++) {
		float angle;

		glm_mat4_identity(t.model);
		glm_translate(t.model, (float*) CUBE_POSITIONS[i]);
		angle = 20.0f * (float) i;
		glm_rotate(t.model, glm_rad(angle), (float[]) { 1.0f, 0.3f, 0.5f });
		shader_set_mat4(state->shader_program, "model", t.model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	// draw light cube
	glUseProgram(state->light_shader_program);
	glBindVertexArray(state->light_vo.vao);
	for (i = 0; i < 4; i++) {
		glm_mat4_identity(t.model);
		glm_translate(t.model, (float*) POINT_LIGHT_POSITIONS[i]);
		glm_scale(t.model, (float[]) { 0.2f, 0.2f, 0.2f });
		shader_set_transform(t, state->light_shader_program);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	camera_update(state->window);
	glfw_context_update_window(&state->window);
}

static void delete(app_state_t* state) {
	glDeleteVertexArrays(1, &state->vo.vao);
	glDeleteBuffers(1, &state->vo.vbo);
	glDeleteBuffers(1, &state->vo.ebo);
	free(state);

	glfwTerminate();

	log_info("Freed application");
}

int32_t app_new(app_state_t** state) {
	app_state_t* tmp;

	*state = malloc(sizeof(app_state_t));

	tmp = *state;

	if (glfw_context_create_window(&tmp->window, framebuffer_size_callback) && !tmp->window) {
		return -1;
	}

	log_info("Created GLFW window");

	tmp->shader_program = shader_create_program("shaders/vertex.vert", "shaders/fragment.frag");
	tmp->light_shader_program = shader_create_program("shaders/light_cube.vert", "shaders/light_cube.frag");

	vertex_object_create(&tmp->vo);
	vertex_object_bind_vertices(&tmp->vo, (float*) VERTICES, sizeof(VERTICES));
	vertex_object_bind_attr(&tmp->vo, 0, 8 * sizeof(float), 0, 3);
	vertex_object_bind_attr(&tmp->vo, 1, 8 * sizeof(float), 3 * sizeof(float), 3);
	vertex_object_bind_attr(&tmp->vo, 2, 8 * sizeof(float), 6 * sizeof(float), 2);
	glUseProgram(tmp->shader_program);
	tmp->texture = texture_create("assets/container2.png");
	tmp->specular_map = texture_create("assets/container2_specular.png");
	shader_set_uniform_primitive(tmp->shader_program, "material.diffuseMap", 0);
	shader_set_uniform_primitive(tmp->shader_program, "material.specularMap", 1);

	vertex_object_create(&tmp->light_vo);
	vertex_object_bind_vertices(&tmp->light_vo, (float*) VERTICES, sizeof(VERTICES));
	vertex_object_bind_attr(&tmp->light_vo, 0, 8 * sizeof(float), 0, 3);

	tmp->update = update;
	tmp->input = input;
	tmp->free = delete;

	camera_init_glfw_callbacks(tmp->window);
	tmp->camera = camera_get();

	glfw_context_capture_cursor(tmp->window);

	tmp->close = false;

	*state = tmp;

	return 0;
}

