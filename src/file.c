#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

bool file_read(const char* filename, char** buf) {
	FILE* fp;
	size_t len;
	bool status;
	char* tmp;

	assert(filename != NULL);
	assert(buf != NULL);

	status = true;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		return false;
	}

	fseek(fp, 0, SEEK_END);
	len = (size_t) ftell(fp);
	fseek(fp, 0, SEEK_SET);
	tmp = malloc(sizeof(char) * (len + 1));
	if (tmp) {
		fread(tmp, 1, len, fp);
		tmp[len] = '\0';
		*buf = tmp;
	} else {
		status = false;
	}

	fclose(fp);

	return status;
}
