#include <stdio.h>                 // for fseek, fclose, fopen, fread, ftell
#include <stdlib.h>                // for malloc

#include "file.h"

int32_t file_read(const char* filename, char** buf) {
	FILE* fp;
	size_t len;
	int32_t status;
	char* tmp;

	status = 0;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		return 1;
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
		status = 1;
	}

	fclose(fp);

	return status;
}
