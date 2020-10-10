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

	OutputFormatNetCDFMetadata *meta = malloc(sizeof(OutputFormatNetCDFMetadata));
	int status = nc_create(abs_path, NC_CLOBBER|NC_NETCDF4, &(meta->ncid));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to open file %s, netCDF driver returned error: %s.\n",
				abs_path, nc_strerror(status));
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	meta->abs_path = abs_path;

	// Define time dimension
	status = nc_def_dim(meta->ncid, OF_DIMENSION_TIME, NC_UNLIMITED, &(meta->dim_time_id));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create dimension %s in output file %s, netCDF driver returned error: %s.\n",
				OF_DIMENSION_TIME, abs_path, nc_strerror(status));
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	int dimids[1];
	dimids[0] = meta->dim_time_id;
	status = nc_def_var(meta->ncid, OF_DIMENSION_TIME, NC_STRING, 1, dimids,
			&(meta->var_time_id));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create variable %s for dimension %s in output file %s, netCDF driver returned error: %s.\n",
				OF_DIMENSION_TIME, OF_DIMENSION_TIME, abs_path, nc_strerror(status));
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
	if (f->variables == NULL) {
		fprintf(stderr, "No variables specified for filter, so no CSV headers could be written.\n");
		return false;
	}
	if (f->output == NULL || f->output->meta == NULL) {
		fprintf(stderr, "Unable to define netCDF variables for filter without initialized output.\n");
		return false;
	}
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	int ncid = meta->ncid;
	int dimids[1];
	dimids[0] = meta->dim_time_id;

	// TODO: Create variables for ID fields

	// Create variable for first field
	bool status = create_variable(f->variables, ncid, dimids);
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

	int retval = nc_enddef(ncid);
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

static bool output_variable_to_netcdf(char * const error, size_t error_len,
		int ncid, int dimids[], MaterializedVariable v) {
	char *local_error;
	int rv;

	OutputFormatNetCDFVariableMetadata *var_meta = (OutputFormatNetCDFVariableMetadata *)v.meta;
	int varid = var_meta->varid;

	switch (v.data_type) {
	case DATA_TYPE_BOOL:
		rv = nc_put_var1_short(ncid, varid, dimids, &(v.u.bool_val));
		break;
	case DATA_TYPE_CHAR:
		rv = nc_put_var1_schar(ncid, varid, dimids, &(v.u.char_val));
		break;
	case DATA_TYPE_STRING:
		rv = nc_put_var1_string(ncid, varid, dimids, &(v.u.char_array));
		break;
	case DATA_TYPE_INT:
		rv = nc_put_var1_int(ncid, varid, dimids, &(v.u.int_val));
		break;
	case DATA_TYPE_LONG:
		rv = nc_put_var1_long(ncid, varid, dimids, &(v.u.long_val));
		break;
	case DATA_TYPE_FLOAT:
		rv = nc_put_var1_float(ncid, varid, dimids, &(v.u.float_val));
		break;
	case DATA_TYPE_DOUBLE:
		rv = nc_put_var1_double(ncid, varid, dimids, &(v.u.double_val));
		break;
	case DATA_TYPE_LONG_ARRAY:
	case DATA_TYPE_DOUBLE_ARRAY:
	default:
		local_error = (char *)calloc(MAXSTR, sizeof(char));
		snprintf(local_error, MAXSTR, "output_format_netcdf_write_data: unknown/unsupported variable type %d.",
				 v.data_type);
		return return_with_error(error, error_len, local_error);
	}

	if (rv != NC_NOERR) {
		local_error = (char *)calloc(MAXSTR, sizeof(char));
		snprintf(local_error, MAXSTR,
				"output_format_netcdf::output_variable_to_netcdf: error writing output, NetCDF driver returned error: %s.",
				nc_strerror(rv));
		return return_with_error(error, error_len, local_error);
	}
	return true;
}

bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const f,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	int ncid = meta->ncid;
	int dimids[1];
	dimids[0] = meta->dim_time_id;

	// First, write date to time variable
	char *isodate = get_iso_date(&date);
	int retval = nc_put_var1_string(ncid, meta->var_time_id, dimids, &isodate);
	if (retval != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "NetCDF driver error %s encountered when writing time variable value %s to netCDF file %s.\n",
				nc_strerror(retval), isodate, meta->abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	free(isodate);

	// TODO: Output entity ID fields

	// Output variables
	for (int i = 0; i < f->num_named_variables; i++) {
		MaterializedVariable v = vars[i];
		bool status = output_variable_to_netcdf(error, error_len, ncid, dimids, v);
		if (!status) {
			return false;
		}
	}

	if (flush) {
		retval = nc_sync(ncid);
		if (retval != NC_NOERR) {
			char *error_mesg = malloc(MAXSTR * sizeof(char *));
			snprintf(error_mesg, MAXSTR, "NetCDF driver error %s encountered when calling nc_sync() on netCDF file %s.\n",
					nc_strerror(retval), meta->abs_path);
			fprintf(stderr, error_mesg);
			free(error_mesg);
			return false;
		}
	}

	return true;
}
