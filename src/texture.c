#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdbool.h>

#include <glad/glad.h>
#include <log.h>

int32_t texture_create(const char* texname) {
	uint32_t texture;
	int32_t width;
	int32_t height;
	int32_t nr_channels;
	uint8_t* texture_bytes;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	texture_bytes = stbi_load(texname, &width, &height, &nr_channels, 0);
	if (texture_bytes) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		log_error("Failed to load texture %s", texname);
		return -1;
	}

	stbi_image_free(texture_bytes);

	return (int32_t) texture;
}
