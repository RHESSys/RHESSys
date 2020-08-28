#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_format_csv.h"

#define BUFFER_SIZE 4096

bool output_format_csv_init(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_CSV) {
		fprintf(stderr, "Cannot initialize CSV output for non CSV filter.\n");
		return false;
	}
	size_t abs_path_len = 2 * FILEPATH_LEN;
	char *abs_path = malloc(abs_path_len * sizeof(char *));
	snprintf(abs_path, abs_path_len, "%s%c%s%c%s",
			f->output->path, PATH_SEP,
			f->output->filename, FILE_EXT_SEP, OUTPUT_FORMAT_EXT_CSV);
	// Use buffered output
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

bool output_format_csv_write_headers(OutputFilter * const f) {
	if (f->variables == NULL) {
		fprintf(stderr, "No variables specified for filter, so no CSV headers could be written.\n");
		return false;
	}
	if (f->output == NULL || f->output->fp == NULL) {
		fprintf(stderr, "Unable to write CSV headers for filter without initialized output.\n");
		return false;
	}
	FILE *fp = f->output->fp;
	fprintf(fp, "%s", f->variables->name);
	OutputFilterVariable *v = f->variables->next;
	while (v != NULL) {
		fprintf(fp, ",%s", v->name);
		v = v->next;
	}
	fprintf(fp, "\n");
	return !fflush(fp);
}
