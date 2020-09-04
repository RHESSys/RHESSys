#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_format_csv.h"

#define BUFFER_SIZE 4096

#define FMT_STR_TIMESTAMP_FIELD "%ld%s"
#define FMT_STR_ID_FIELD "%d%s"

#define FMT_STR_BOOL "%s%h"
#define FMT_STR_CHAR "%s%c"
#define FMT_STR_CHAR_ARRAY "%s%s"
#define FMT_STR_INT "%s%d"
#define FMT_STR_LONG "%s%l"
#define FMT_STR_LONG_ARRAY "%s%p"
#define FMT_STR_FLOAT "%s%f"
#define FMT_STR_DOUBLE "%s%f"
#define FMT_STR_DOUBLE_ARRAY "%s%p"
#define FMT_STR_UNDEFINED NULL

#define HEADER_ID_PATCH "basinID,hillID,zoneID,patchID,"
#define HEADER_ID_CANOPY_STRATA "basinID,hillID,zoneID,patchID,stratumID,"


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
	// Output timestep headers
	switch (f->timestep) {
	case TIMESTEP_HOURLY:
		fprintf(fp, FMT_STR_CHAR_ARRAY, CSV_HEADER_HOUR, CSV_DELIM_DEFAULT);
	case TIMESTEP_DAILY:
		fprintf(fp, FMT_STR_CHAR_ARRAY, CSV_HEADER_DAY, CSV_DELIM_DEFAULT);
	case TIMESTEP_MONTHLY:
		fprintf(fp, FMT_STR_CHAR_ARRAY, CSV_HEADER_MONTH, CSV_DELIM_DEFAULT);
	case TIMESTEP_YEARLY:
		fprintf(fp, FMT_STR_CHAR_ARRAY, CSV_HEADER_YEAR, CSV_DELIM_DEFAULT);
		break;
	default:
		// Do not print headers for unknown time steps
		break;
	}

	// Output ID headers
	switch (f->type) {
	case OUTPUT_FILTER_PATCH:
		fprintf(fp, HEADER_ID_PATCH);
		break;
	case OUTPUT_FILTER_CANOPY_STRATA:
		fprintf(fp, HEADER_ID_CANOPY_STRATA);
		break;
	default:
		// Do not print ID headers for unknown output filter types
		break;
	}

	// Output header for first field
	fprintf(fp, "%s", f->variables->name);
	OutputFilterVariable *v = f->variables->next;
	// Output headers for remaining fields
	while (v != NULL) {
		fprintf(fp, FMT_STR_CHAR_ARRAY, CSV_DELIM_DEFAULT, v->name);
		v = v->next;
	}
	fprintf(fp, CSV_EOL);
	return !fflush(fp);
}

static bool output_variable_to_stream(char * const error, size_t error_len,
		FILE *fp, MaterializedVariable v, char *delim) {
	char *local_error;
	int rv;
	switch (v.data_type) {
	case DATA_TYPE_BOOL:
		rv = fprintf(fp, FMT_STR_BOOL, delim, v.u.bool_val);
		break;
	case DATA_TYPE_CHAR:
		rv = fprintf(fp, FMT_STR_CHAR, delim, v.u.char_val);
		break;
	case DATA_TYPE_CHAR_ARRAY:
		rv = fprintf(fp, FMT_STR_CHAR_ARRAY, delim, v.u.char_array);
		break;
	case DATA_TYPE_INT:
		rv = fprintf(fp, FMT_STR_INT, delim, v.u.int_val);
		break;
	case DATA_TYPE_LONG:
		rv = fprintf(fp, FMT_STR_LONG, delim, v.u.long_val);
		break;
	case DATA_TYPE_LONG_ARRAY:
		rv = fprintf(fp, FMT_STR_LONG_ARRAY, delim, v.u.long_array);
		break;
	case DATA_TYPE_FLOAT:
		rv = fprintf(fp, FMT_STR_FLOAT, delim, v.u.float_val);
		break;
	case DATA_TYPE_DOUBLE:
		rv = fprintf(fp, FMT_STR_DOUBLE, delim, v.u.double_val);
		break;
	case DATA_TYPE_DOUBLE_ARRAY:
		rv = fprintf(fp, FMT_STR_DOUBLE_ARRAY, delim, v.u.double_array);
		break;
	default:
		local_error = (char *)calloc(MAXSTR, sizeof(char));
		snprintf(local_error, MAXSTR, "output_format_csv_write_data: unknown variable type %d.",
				 v.data_type);
		return return_with_error(error, error_len, local_error);
	}

	if (rv < 1) {
		perror("output_format_csv::output_variable_to_stream: error writing output");
		return false;
	}
	return true;
}

bool output_format_csv_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const f,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	if (f->num_named_variables < 1) return true;

	bool status;
	int curr_var = 0;

	if (f->output == NULL || f->output->fp == NULL) {
		fprintf(stderr, "Unable to write CSV headers for filter without initialized output.\n");
		return false;
	}
	FILE *fp = f->output->fp;

	// Output time step
	switch (f->timestep) {
	case TIMESTEP_HOURLY:
		fprintf(fp, FMT_STR_TIMESTAMP_FIELD, date.hour, CSV_DELIM_DEFAULT);
	case TIMESTEP_DAILY:
		fprintf(fp, FMT_STR_TIMESTAMP_FIELD, date.day, CSV_DELIM_DEFAULT);
	case TIMESTEP_MONTHLY:
		fprintf(fp, FMT_STR_TIMESTAMP_FIELD, date.month, CSV_DELIM_DEFAULT);
	case TIMESTEP_YEARLY:
		fprintf(fp, FMT_STR_TIMESTAMP_FIELD, date.year, CSV_DELIM_DEFAULT);
		break;
	default:
		// Do not print time step for unknown time steps
		break;
	}

	// Output entity ID fields
	if (id.basin_ID != OUTPUT_FILTER_ID_EMPTY) {
		fprintf(fp, FMT_STR_ID_FIELD, id.basin_ID, CSV_DELIM_DEFAULT);
	}
	if (id.hillslope_ID != OUTPUT_FILTER_ID_EMPTY) {
		fprintf(fp, FMT_STR_ID_FIELD, id.hillslope_ID, CSV_DELIM_DEFAULT);
	}
	if (id.zone_ID != OUTPUT_FILTER_ID_EMPTY) {
		fprintf(fp, FMT_STR_ID_FIELD, id.zone_ID, CSV_DELIM_DEFAULT);
	}
	if (id.patch_ID != OUTPUT_FILTER_ID_EMPTY) {
		fprintf(fp, FMT_STR_ID_FIELD, id.patch_ID, CSV_DELIM_DEFAULT);
	}
	if (id.canopy_strata_ID != OUTPUT_FILTER_ID_EMPTY) {
		fprintf(fp, FMT_STR_ID_FIELD, id.canopy_strata_ID, CSV_DELIM_DEFAULT);
	}

	// Output first value
	MaterializedVariable v = vars[curr_var++];
	status = output_variable_to_stream(error, error_len, fp, v, CSV_DELIM_NONE);
	if (!status) {
		return false;
	}
	// Output remaining values
	while (curr_var < f->num_named_variables) {
		MaterializedVariable v = vars[curr_var++];
		status = output_variable_to_stream(error, error_len, fp, v, CSV_DELIM_DEFAULT);
		if (!status) {
			return false;
		}
	}

	// Write end of line
	int rv = fprintf(fp, CSV_EOL);
	if (rv < 1) {
		perror("output_format_csv_write_data: failed to write end of line");
		return false;
	}
	// Flush stream (if requested)
	if (flush) {
		rv = fflush(fp);
		if (rv != 0) {
			perror("output_format_csv_write_data: error flushing stream");
			return false;
		}
	}

	return true;
}
