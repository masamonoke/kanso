#include "raycast.h"

#include <math.h>
#include <string.h>

#include "cglm/mat4.h"
#include "cglm/types.h"
#include <cglm/vec3.h>

#include "window.h"
#include "camera.h"
#include "world.h"
#include "model.h"

#define SPHERE_RADIUS 1.0f

static void viewport_to_world(float viewport_x, float viewport_y, mat4 proj, mat4 view, vec3 world_pos);

bool raycast_viewport_model(world_t* world, float viewport_x, float viewport_y, mat4 proj, mat4 view) {
	vec3 ray_origin;
	vec3 ray_direction;
	model_t** models;
	size_t models_count;
	size_t i;

	viewport_to_world(viewport_x, viewport_y, proj, view, ray_direction);
	glm_vec3_normalize(ray_direction);
	memcpy(ray_origin, camera_pos(), sizeof(vec3));

	world_models(world, &models, &models_count);
	for (i = 0; i < models_count; i++) {
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
		vec3 sphere_center;
		vec3 L;
		float t0, t1;
		float tca;
		float thc;
		float d2;

		memcpy(sphere_center, models[i]->common.position, sizeof(vec3));
		glm_vec3_sub(sphere_center, ray_origin, L);
		tca = glm_vec3_dot(L, ray_direction);
		if (tca < 0) {
			continue;
		}
		d2 = glm_vec3_dot(L, L) - (tca * tca);
		if (d2 > SPHERE_RADIUS * SPHERE_RADIUS) {
			continue;
		}
		thc = sqrtf(SPHERE_RADIUS * SPHERE_RADIUS - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		if (t0 > t1) {
			float tmp;
			tmp = t0;
			t0 = t1;
			t1 = tmp;
		}
		if (t0 < t1) {
			t0 = t1;
			if (t0 < 0) {
				continue;
			}
		}

		models[i]->common.selected = true;
		return true;
	}

	for (i = 0; i < models_count; i++) {
		models[i]->common.selected = false;
	}
	return false;
}

static void viewport_to_ndc(float viewport_x, float viewport_y, float* ndc_x, float* ndc_y);

static void ndc_to_clip(float ndc_x, float ndc_y, vec4 pos);

static void clip_to_view(vec4 pos, mat4 proj);

static void view_to_world(vec4 pos, mat4 view);

static void viewport_to_world(float viewport_x, float viewport_y, mat4 proj, mat4 view, vec3 world_pos) { // NOLINT (*bugprone-easily-swappable-parameters)
	float ndc_x;
	float ndc_y;
	vec4 pos;

	viewport_to_ndc(viewport_x, viewport_y, &ndc_x, &ndc_y);
	ndc_to_clip(ndc_x, ndc_y, pos);
	clip_to_view(pos, proj);
	view_to_world(pos, view);
	memcpy(world_pos, pos, sizeof(vec3));
}

static void viewport_to_ndc(float viewport_x, float viewport_y, float* ndc_x, float* ndc_y) {
    *ndc_x = (2.0f * viewport_x) / (float) window_width() - 1.0f;
    *ndc_y = 1.0f - (2.0f * viewport_y) / (float) window_height();
}

static void ndc_to_clip(float ndc_x, float ndc_y, vec4 pos) {
	memcpy(pos, (vec4) { ndc_x, ndc_y, -1.0f, 1.0f }, sizeof(vec4));
}

static void clip_to_view(vec4 pos, mat4 proj) {
	mat4 inverse_proj;

	glm_mat4_inv(proj, inverse_proj);
	glm_mat4_mulv(inverse_proj, pos, pos);
	pos[2] = -1.0f;
	pos[3] = 0.0f;
}

static void view_to_world(vec4 pos, mat4 view) {
	mat4 inverse_view;

	glm_mat4_inv(view, inverse_view);
	glm_mat4_mulv(inverse_view, pos, pos);
}
