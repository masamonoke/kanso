#include "texture.h"

#include <assert.h>
#ifdef DEBUG
#include <stddef.h>
#endif

#include <glad/glad.h>
#include <c_log.h>

int32_t texture_create(const uint8_t* texture_bytes, int32_t width, int32_t height, int32_t nr_channels) { // NOLINT
	uint32_t texture;
	int32_t status;

	assert(texture_bytes != NULL);

	status = 0;

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
			log_error("Unknown texture format");
			status = -1;
			goto L_RET;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, (int32_t) format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

L_RET:
	return status < 0 ? status : (int32_t) texture;
}
