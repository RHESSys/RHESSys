#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_format_csv.h"

FILE *output_format_csv_init(char *path, char *filename) {
	size_t abs_path_len = 2 * FILEPATH_LEN;
	char *abs_path = malloc(abs_path_len * sizeof(char *));
	snprintf(abs_path, abs_path_len, "%s%c%s%c%s",
			path, PATH_SEP, filename, FILE_EXT_SEP, OUTPUT_FORMAT_EXT_CSV);
	FILE *fp = fopen(abs_path, "w");
	if (fp == NULL) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to open file %s", abs_path);
		perror(error_mesg);
		free(error_mesg);
		return NULL;
	}
	return fp;
}

void output_format_csv_destroy(FILE *fp) {
	fclose(fp);
}
