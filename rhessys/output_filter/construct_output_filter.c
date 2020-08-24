#include <string.h>

#include "rhessys.h"
#include "output_filter.h"
#include "index_struct_fields.h"

OutputFilter *parse(const char* input);

static bool returnWithError(char * const error, size_t error_len, char *error_mesg) {
	strncpy(error, error_mesg, error_len);
	return false;
}


bool construct_output_filter(char * const error, size_t error_len,
		struct command_line_object * const cmd,
		struct world_object * const world) {
	if (!cmd->output_filter_flag) {
		return returnWithError(error, error_len, "output_filter_flag is fall, not constructing output filter.");
	}

	// Parse output filter
	OutputFilter *filters = parse(cmd->output_filter_filename);
	if (filters->parse_error) {
		return returnWithError(error, error_len, "output_filter_parser returned with an error.");
	}

	StructIndex_t *idx = index_struct_fields();

	return true;
}
