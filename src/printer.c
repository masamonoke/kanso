#include <stdio.h>         // for printf, size_t

#include "loaded_model.h"  // for loaded_model_data, loaded_model_t
#include "model.h"         // for model_common, model_type, model_print, mod...
#include "primitive.h"     // for primitive_model_data, primitive_model_t
#include "shader.h"        // for transform

void model_print(model_t* model) {
	size_t i;
	size_t j;

	printf("Model {\n");

	switch (model->common.type) {
		case LOADED_MODEL:
			printf("\ttype = LOADED_MODEL\n");
			printf("\tdirectory = %s\n", ((loaded_model_t*) &model)->model_data.directory);
			printf("\tmeshes_count = %ld\n", ((loaded_model_t*) &model)->model_data.meshes_count);
			printf("\tebo = %d\n", ((loaded_model_t*) &model)->model_data.model_vo.ebo);
			printf("\tvbo = %d\n", ((loaded_model_t*) &model)->model_data.model_vo.vbo);
			printf("\tvao = %d\n", ((loaded_model_t*) &model)->model_data.model_vo.vao);
			break;
		case PRIMITVE_MODEL:
			printf("\ttype = PRIMITVE_MODEL\n");
			printf("\tprimitive type = %d\n", ((primitive_model_t*) &model)->model_data.primitive_type);
			printf("\tebo = %d\n", ((primitive_model_t*) &model)->model_data.vo.ebo);
			printf("\tvbo = %d\n", ((primitive_model_t*) &model)->model_data.vo.vbo);
			printf("\tvao = %d\n", ((primitive_model_t*) &model)->model_data.vo.vao);
			break;
		default:
			printf("\ttype = UNDEFINED\n");
			break;
	}

	printf("\tposition = (%f, %f, %f)\n",
		(double) model->common.position[0], (double) model->common.position[1], (double) model->common.position[2]);
	printf("\tscale = (%f, %f, %f)\n",
		(double) model->common.scale[0], (double) model->common.scale[1], (double) model->common.scale[2]);
	printf("\tshader_program = %d\n", model->common.shader_program);

	printf("\ttransform = {\n");
	printf("\t\tmodel matrix = {\n");
	for (i = 0; i < 4; i++) {
		printf("\t\t\t");
		for (j = 0; j < 4; j++) {
			printf("%f", (double) model->common.transform.model[i][j]);
		}
		printf("\n");
	}
	printf("\t}\n");

	printf("\t\tview matrix = {\n");
	for (i = 0; i < 4; i++) {
		printf("\t\t\t");
		for (j = 0; j < 4; j++) {
			printf("%f ", (double) model->common.transform.view[i][j]);
		}
		printf("\n");
	}
	printf("\t\t}\n");

	printf("\t\tprojection matrix = {\n");
	for (i = 0; i < 4; i++) {
		printf("\t\t\t");
		for (j = 0; j < 4; j++) {
			printf("%f ", (double) model->common.transform.proj[i][j]);
		}
		printf("\n");
	}
	printf("\t\t}\n");
	printf("}\n");
}
