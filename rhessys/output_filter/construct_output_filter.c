#include <string.h>

#include "rhessys.h"
#include "output_filter.h"
#include "index_struct_fields.h"
#include "output_filter/output_format_csv.h"

OutputFilter *parse(const char* input);

static bool returnWithError(char * const error, size_t error_len, char *error_mesg) {
	strncpy(error, error_mesg, error_len);
	return false;
}

static bool init_output(OutputFilter *f) {
	switch(f->output->format) {
	case OUTPUT_TYPE_CSV:
		f->output->fp = output_format_csv_init(f->output->path, f->output->filename);
		return f->output->fp != NULL;
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "output format type %d is unknown or not yet implemented.", f->output->format);
		return false;
	}
	return true;
}

bool construct_output_filter(char * const error, size_t error_len,
		struct command_line_object * const cmd,
		struct world_object * const world) {
	if (!cmd->output_filter_flag) {
		return returnWithError(error, error_len, "output_filter_flag is fall, not constructing output filter.");
	}

	// Parse output filter
	OutputFilter *filters = parse(cmd->output_filter_filename);
	if (filters == NULL) {
		return returnWithError(error, error_len, "unable to parse output filter.");
	}
	if (filters->parse_error) {
		return returnWithError(error, error_len, "output_filter_parser returned with an error.");
	}

	for (OutputFilter *f = filters; f != NULL; f = f->next) {
		// Initialze output file for filter
		if (f->output == NULL) {
			return returnWithError(error, error_len,
					"Output filter did not specify an output section.");
		}
		if (f->output->path == NULL) {
			return returnWithError(error, error_len,
					"Output filter did not specify output path.");
		}
		if (f->output->filename == NULL) {
			return returnWithError(error, error_len,
					"Output filter did not specify output filename.");
		}
		bool status = init_output(f);
		if (!status) {
			return returnWithError(error, error_len, "unable to initialize output for output filter.");
		}
	}

	StructIndex_t *idx = index_struct_fields();

	cmd->output_filter = filters;
	return true;
}
