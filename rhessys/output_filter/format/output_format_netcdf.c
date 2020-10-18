#include <netcdf.h>

#include "rhessys.h"
#include "isodate.h"
#include "output_filter/output_format_netcdf.h"


static void free_metadata(OutputFormatNetCDFMetadata *meta) {
	if (meta == NULL) return;
	if (meta->abs_path != NULL) {
		free(meta->abs_path);
	}
	free(meta);
}

static inline bool create_time_variable(char *abs_path, int ncid, int dimids[], char *name, nc_type type, int *time_id) {
	bool status = nc_def_var(ncid, name, type, OF_DIM_VECTOR, dimids, time_id);
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create time variable %s for dimension %s in output file %s, netCDF driver returned error: %s.\n",
				name, OF_DIMENSION_IDX, abs_path, nc_strerror(status));
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	return true;
}

static inline bool create_variable(OutputFilterVariable *v, int ncid, int dimids[]) {
	int nc_type = get_netcdf_data_type(v->data_type);
	if (nc_type == INVALID_TYPE) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create variable %s, output filter data type %d not supported by netCDF driver.\n",
				v->name,v->data_type);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	int varid;
	int status = nc_def_var(ncid, v->name, nc_type, 1, dimids,
			&varid);
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create variable %s, netCDF driver returned value of: %s.\n",
				v->name, nc_strerror(status));
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	// Save variable ID so that we can retrieve it when we need to write data for this variable.
	OutputFormatNetCDFVariableMetadata *var_meta = malloc(sizeof(OutputFormatNetCDFVariableMetadata));
	var_meta->varid = varid;
	v->meta = var_meta;

	return true;
}

static inline bool output_byte_to_netcdf(char *abs_path, int ncid, size_t idx[],
		int varid, char value) {
	int retval = nc_put_var1_schar(ncid, varid, idx, &value);
	if (retval != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "output_format_netcdf::output_variable_to_netcdf: error writing output, NetCDF driver error %s encountered when writing variable ID %d to netCDF file %s.\n",
				nc_strerror(retval), varid, abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	return true;
}

static inline bool output_short_to_netcdf(char *abs_path, int ncid, size_t idx[],
		int varid, short value) {
	int retval = nc_put_var1_short(ncid, varid, idx, &value);
	if (retval != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "output_format_netcdf::output_variable_to_netcdf: error writing output, NetCDF driver error %s encountered when writing variable ID %d to netCDF file %s.\n",
				nc_strerror(retval), varid, abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	return true;
}

static bool output_materialized_variable_to_netcdf(char * const error, size_t error_len,
		int ncid, size_t idx[], MaterializedVariable *v) {
	char *local_error;
	int rv;

	OutputFormatNetCDFVariableMetadata *var_meta = (OutputFormatNetCDFVariableMetadata *)v->meta;
	int varid = var_meta->varid;

	switch (v->data_type) {
	case DATA_TYPE_BOOL:
		rv = nc_put_var1_short(ncid, varid, idx, &(v->u.bool_val));
		break;
	case DATA_TYPE_CHAR:
		rv = nc_put_var1_schar(ncid, varid, idx, &(v->u.char_val));
		break;
	case DATA_TYPE_STRING:
		rv = nc_put_var1_string(ncid, varid, idx, &(v->u.char_array));
		break;
	case DATA_TYPE_INT:
		rv = nc_put_var1_int(ncid, varid, idx, &(v->u.int_val));
		break;
	case DATA_TYPE_LONG:
		rv = nc_put_var1_long(ncid, varid, idx, &(v->u.long_val));
		break;
	case DATA_TYPE_FLOAT:
		rv = nc_put_var1_float(ncid, varid, idx, &(v->u.float_val));
		break;
	case DATA_TYPE_DOUBLE:
		rv = nc_put_var1_double(ncid, varid, idx, &(v->u.double_val));
		break;
	case DATA_TYPE_LONG_ARRAY:
	case DATA_TYPE_DOUBLE_ARRAY:
	default:
		local_error = (char *)calloc(MAXSTR, sizeof(char));
		snprintf(local_error, MAXSTR, "output_format_netcdf_write_data: unknown/unsupported variable type %d.",
				 v->data_type);
		return return_with_error(error, error_len, local_error);
	}

	if (rv != NC_NOERR) {
		local_error = (char *)calloc(MAXSTR, sizeof(char));
		snprintf(local_error, MAXSTR,
				"output_format_netcdf::output_materialized_variable_to_netcdf: error writing output, NetCDF driver returned error: %s.",
				nc_strerror(rv));
		return return_with_error(error, error_len, local_error);
	}
	return true;
}

inline int get_netcdf_data_type(DataType type) {
	switch (type) {
	case DATA_TYPE_BOOL:
		return NC_BYTE;
	case DATA_TYPE_CHAR:
		return NC_CHAR;
	case DATA_TYPE_STRING:
		return NC_STRING;
	case DATA_TYPE_INT:
		return NC_INT;
	case DATA_TYPE_LONG:
		return NC_LONG;
	case DATA_TYPE_FLOAT:
		return NC_FLOAT;
	case DATA_TYPE_DOUBLE:
		return NC_DOUBLE;
	default:
		return INVALID_TYPE;
	}
}

bool output_format_netcdf_init(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_NETCDF) {
		fprintf(stderr, "Cannot initialize netCDF output for non netCDF filter.\n");
		return false;
	}
	size_t abs_path_len = 2 * FILEPATH_LEN;
	char *abs_path = malloc(abs_path_len * sizeof(char *));
	snprintf(abs_path, abs_path_len, "%s%c%s%c%s",
			f->output->path, PATH_SEP,
			f->output->filename, FILE_EXT_SEP, OUTPUT_FORMAT_EXT_NETCDF);

	OutputFormatNetCDFMetadata *meta = calloc(1, sizeof(OutputFormatNetCDFMetadata));
	int status = nc_create(abs_path, NC_CLOBBER, &(meta->ncid));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to open file %s, netCDF driver returned error: %s.\n",
				abs_path, nc_strerror(status));
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	meta->abs_path = abs_path;

	// Define index dimension
	status = nc_def_dim(meta->ncid, OF_DIMENSION_IDX, NC_UNLIMITED, &(meta->dim_idx_id));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create dimension %s in output file %s, netCDF driver returned error: %s.\n",
				OF_DIMENSION_IDX, abs_path, nc_strerror(status));
		perror(error_mesg);
		free(error_mesg);
		return false;
	}

	// Save OutputFormatNetCDFMetadata to filter's output metadata so that the file can later be closed.
	f->output->meta = meta;

	return true;
}

bool output_format_netcdf_destroy(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_NETCDF) {
		fprintf(stderr, "Cannot destroy netCDF output for non netCDF filter.\n");
		return false;
	}
	if (f->output == NULL || f->output->meta == NULL) {
		fprintf(stderr, "Failed to close netCDF output because no output metadata were found.\n");
	}
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	int status = nc_close(meta->ncid);
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to close netCDF file %s, netCDF driver returned value of: %s.\n",
				meta->abs_path, nc_strerror(status));
		perror(error_mesg);
		free(error_mesg);
		return false;
	}

	free_metadata(meta);
	f->output->meta = NULL;

	return true;
}
bool output_format_netcdf_write_headers(OutputFilter * const f) {
	bool status = true;
	int retval;

	if (f->variables == NULL) {
		fprintf(stderr, "No variables specified for filter, so no CSV headers could be written.\n");
		return false;
	}
	if (f->output == NULL || f->output->meta == NULL) {
		fprintf(stderr, "Unable to define netCDF variables for filter without initialized output.\n");
		return false;
	}
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	meta->index = 0;
	char *abs_path = meta->abs_path;
	int ncid = meta->ncid;
	int dimids[1];
	dimids[0] = meta->dim_idx_id;

	// Define variables for timestep
	switch (f->timestep) {
	case TIMESTEP_HOURLY:
		status = create_time_variable(abs_path, ncid, dimids, OF_VAR_HOUR, NC_BYTE, &(meta->var_time_hour_id));
		if (!status) return false;
	case TIMESTEP_DAILY:
		status = create_time_variable(abs_path, ncid, dimids, OF_VAR_DAY, NC_BYTE, &(meta->var_time_day_id));
		if (!status) return false;
	case TIMESTEP_MONTHLY:
		status = create_time_variable(abs_path, ncid, dimids, OF_VAR_MONTH, NC_BYTE, &(meta->var_time_month_id));
		if (!status) return false;
	case TIMESTEP_YEARLY:
		status = create_time_variable(abs_path, ncid, dimids, OF_VAR_YEAR, NC_SHORT, &(meta->var_time_year_id));
		if (!status) return false;
		break;
	default:
		// Do not create variables for unknown time steps
		break;
	}

	// TODO: Create variables for ID fields

	// Create variable for first field
	status = create_variable(f->variables, ncid, dimids);
	if (!status) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Failed variable creation was in netCDF file %s.\n",
				meta->abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	OutputFilterVariable *v = f->variables->next;
	// Create variables for remaining fields
	while (v != NULL) {
		status = create_variable(v, ncid, dimids);
		if (!status) {
			char *error_mesg = malloc(MAXSTR * sizeof(char *));
			snprintf(error_mesg, MAXSTR, "Failed variable creation was in netCDF file %s.\n",
					meta->abs_path);
			fprintf(stderr, error_mesg);
			free(error_mesg);
			return false;
		}
		v = v->next;
	}

	retval = nc_enddef(ncid);
	if (retval != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "NetCDF driver error %s encountered when ending definition section of netCDF file %s.\n",
				nc_strerror(retval), meta->abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}

	return true;
}

bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const f,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	bool status = true;
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	int ncid = meta->ncid;
	int dimids[1];
	dimids[0] = meta->dim_idx_id;
	size_t curr_idx[] = {meta->index++};

	// Second, output time step variables
	short hour, day, month;
	short year;
	switch (f->timestep) {
	case TIMESTEP_HOURLY:
		//hour = (short)date.hour;
		status = output_byte_to_netcdf(meta->abs_path, ncid, curr_idx,
				meta->var_time_hour_id, (char)date.hour);
		if (!status) return false;
	case TIMESTEP_DAILY:
		//day = (short)date.day;
		status = output_byte_to_netcdf(meta->abs_path, ncid, curr_idx,
				meta->var_time_day_id, (char)date.day);
		if (!status) return false;
	case TIMESTEP_MONTHLY:
		//month = (short)date.month;
		status = output_byte_to_netcdf(meta->abs_path, ncid, curr_idx,
				meta->var_time_month_id, (char)date.month);
		if (!status) return false;
	case TIMESTEP_YEARLY:
		//year = (short)date.year;
		status = output_short_to_netcdf(meta->abs_path, ncid, curr_idx,
				meta->var_time_year_id, (short)date.year);
		if (!status) return false;
		break;
	default:
		// Do not write time step for unknown time steps
		break;
	}

	// TODO: Output entity ID fields

	// Output variables
	for (int i = 0; i < f->num_named_variables; i++) {
		MaterializedVariable *v = &vars[i];
		bool status = output_materialized_variable_to_netcdf(error, error_len, ncid, curr_idx, v);
		if (!status) {
			return false;
		}
	}

	if (flush) {
		int retval = nc_sync(ncid);
		if (retval != NC_NOERR) {
			char *error_mesg = malloc(MAXSTR * sizeof(char *));
			snprintf(error_mesg, MAXSTR, "NetCDF driver error %s encountered when calling nc_sync() on netCDF file %s.\n",
					nc_strerror(retval), meta->abs_path);
			fprintf(stderr, error_mesg);
			free(error_mesg);
			return false;
		}
	}

	return status;
}
