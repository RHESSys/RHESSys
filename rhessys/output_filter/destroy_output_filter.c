#include <string.h>

#include "rhessys.h"
#include "output_filter.h"
#include "output_filter/output_format_csv.h"

static bool returnWithError(char * const error, size_t error_len, char *error_mesg) {
	strncpy(error, error_mesg, error_len);
	free(error_mesg);
	return false;
}

static bool destroy_output(OutputFilter *f) {
	switch(f->output->format) {
	case OUTPUT_TYPE_CSV:
		return output_format_csv_destroy(f);
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "output format type %d is unknown or not yet implemented.", f->output->format);
		return false;
	}
	return true;
}

bool destroy_output_filter(char * const error, size_t error_len,
		struct command_line_object * const cmd) {
	if (!cmd->output_filter_flag) {
		return returnWithError(error, error_len, "output_filter_flag is fall, not destroying output filter.");
	}

	for (OutputFilter *f = cmd->output_filter; f != NULL; f = f->next) {
		if (f->output == NULL) {
			return returnWithError(error, error_len,
					"No output section in output filter.");
		}
		bool status = destroy_output(f);
		if (!status) {
			char *destroy_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(destroy_error, MAXSTR, "unable to destroy output %s/%s for output filter.",
					f->output->path, f->output->filename);
			return returnWithError(error, error_len, destroy_error);
		}
	}

	return true;
}
