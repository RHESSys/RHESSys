#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_format_csv.h"

bool output_format_csv_init(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_CSV) {
		fprintf(stderr, "Cannot initialize CSV output for non CSV filter.");
		return false;
	}
	size_t abs_path_len = 2 * FILEPATH_LEN;
	char *abs_path = malloc(abs_path_len * sizeof(char *));
	snprintf(abs_path, abs_path_len, "%s%c%s%c%s",
			f->output->path, PATH_SEP,
			f->output->filename, FILE_EXT_SEP, OUTPUT_FORMAT_EXT_CSV);
	FILE *fp = fopen(abs_path, "w");
	if (fp == NULL) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to open file %s", abs_path);
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	f->output->fp = fp;

	return true;
}

bool output_format_csv_destroy(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_CSV) {
		fprintf(stderr, "Cannot destroy CSV output for non CSV filter.");
		return false;
	}
	return !fclose(f->output->fp);
}
