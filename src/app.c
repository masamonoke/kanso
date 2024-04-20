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

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

float light_vertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
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

static void update(app_state_t* state) {
	struct transform t = TRANSFORM_IDENTITY;
	const float light_pos[] = { 1.2f, 1.0f, 2.0f };

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state->texture);
	camera_set_view(t.view);
	glm_perspective(glm_rad(camera_fov()), 600.0f / 800, 0.1f, 100.f, t.proj);
	// draw cube
	glUseProgram(state->shader_program);
	shader_set_vec3(state->shader_program, "objectColor", (float[]) { 1.0f, 0.5f, 0.31f });
	shader_set_vec3(state->shader_program, "lightColor", (float[]) { 1.0f, 1.5f, 1.0f });
	shader_set_vec3(state->shader_program, "lightPos", (float*) light_pos);
	shader_set_vec3(state->shader_program, "viewPos", (float*) camera_pos());
	shader_set_transform(t, state->shader_program);
	glBindVertexArray(state->vo.vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glm_mat4_identity(t.model);

	glm_translate(t.model, (float*) light_pos);
	glm_scale(t.model, (float[]) { 0.2f, 0.2f, 0.2f });
	// draw light cube
	glUseProgram(state->light_shader_program);
	shader_set_transform(t, state->light_shader_program);
	glBindVertexArray(state->light_vo.vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
	vertex_object_bind_vertices(&tmp->vo, vertices, sizeof(vertices));
	vertex_object_bind_attr(&tmp->vo, 0, 6 * sizeof(float), 0);
	vertex_object_bind_attr(&tmp->vo, 1, 6 * sizeof(float), 3 * sizeof(float));

	vertex_object_create(&tmp->light_vo);
	vertex_object_bind_vertices(&tmp->light_vo, light_vertices, sizeof(light_vertices));
	vertex_object_bind_attr(&tmp->light_vo, 0, 3 * sizeof(float), 0);

	tmp->texture = texture_create("assets/container.jpg");
	shader_set_uniform_primitive(tmp->shader_program, "texture", 0);

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

