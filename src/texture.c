#include "texture.h"

#include <glad/glad.h>
#include <c_log.h>
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif


int32_t texture_create_gl_texture(const char* texname) {
	uint32_t texture;
	int32_t width;
	int32_t height;
	int32_t nr_channels;
	uint8_t* texture_bytes;
	int32_t status;

	assert(texname != NULL);

	status = 0;

	/* stbi_set_flip_vertically_on_load(true); */

	texture_bytes = stbi_load(texname, &width, &height, &nr_channels, 0);
	if (texture_bytes) {
		GLenum format;
		switch (nr_channels) {
			case 1:
				format = GL_RED;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			default:
				log_error("Unknown texture %s format", texname);
				status = -1;
				goto L_FREE;
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, (int32_t) format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);

	} else {
		log_error("Failed to load texture %s", texname);
		status = -1;
	}

L_FREE:
	stbi_image_free(texture_bytes);

	if (status) {
		return status;
	}

	return (int32_t) texture;
}
