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
		return output_format_csv_init(f);
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "init_output: output format type %d is unknown or not yet implemented.", f->output->format);
		return false;
	}
	return true;
}

static bool write_headers(OutputFilter *f) {
	switch(f->output->format) {
	case OUTPUT_TYPE_CSV:
		return output_format_csv_write_headers(f);
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "write_headers: output format type %d is unknown or not yet implemented.", f->output->format);
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

	StructIndex_t *idx = index_struct_fields();

	bool status;
	for (OutputFilter *f = filters; f != NULL; f = f->next) {
		// TODO: Search for patches/zones/hillslopes/basins

		// TODO: Validate variables and write offsets to filter

		// Initialize output for filters
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
		status = init_output(f);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to initialize output %s/%s for output filter.",
					f->output->path, f->output->filename);
			return returnWithError(error, error_len, init_error);
		}

		// Write header information for each output file
		status = write_headers(f);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to write headers for output %s/%s.",
					f->output->path, f->output->filename);
			return returnWithError(error, error_len, init_error);
		}
	}

	cmd->output_filter = filters;
	return true;
}
