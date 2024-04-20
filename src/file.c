#include "file.h"

#include <stdio.h>
#include <stdlib.h>

int32_t file_read(const char* filename, char** buf) {
	FILE* fp;
	size_t len;
	int32_t status;
	char* tmp;

	tmp = *buf;
	status = 0;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	tmp = malloc(sizeof(char) * (len + 1));
	if (tmp) {
		fread(tmp, 1, len, fp);
	} else {
		status = 1;
	}

	tmp[len] = '\0';

	fclose(fp);

	*buf = tmp;

	return status;
}
