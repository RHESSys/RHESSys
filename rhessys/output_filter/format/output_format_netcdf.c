#include <netcdf.h>

#include "rhessys.h"
#include "output_filter/output_format_netcdf.h"


static void free_metadata(OutputFormatNetCDFMetadata *meta) {
	if (meta == NULL) return;
	if (meta->abs_path != NULL) {
		free(meta->abs_path);
	}
	free(meta);
}

static inline bool create_variable(OutputFilterVariable *v, int ncid, int *dimid_ptr) {
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
	int status = nc_def_var(ncid, v->name, nc_type, 1, dimid_ptr,
			&varid);
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create variable %s, netCDF driver returned value of: %d.\n",
				v->name, status);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}
	// Save variable ID so that we can retrieve it when we need to write data for this variable.
	OutputFormatNetCDFVariableMetadata *var_meta = malloc(sizeof(OutputFormatNetCDFVariableMetadata));
	var_meta = varid;
	v->meta = var_meta;

	return true;
}

inline int get_netcdf_data_type(DataType type) {
	switch (type) {
	case DATA_TYPE_BOOL:
		return NC_BYTE;
	case DATA_TYPE_CHAR:
		return NC_CHAR;
	case DATA_TYPE_CHAR_ARRAY:
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
		snprintf(error_mesg, MAXSTR, "Unable to open file %s, netCDF driver returned value of: %d.\n", abs_path, status);
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	meta->abs_path = abs_path;

	// Define time dimension
	status = nc_def_dim(meta->ncid, NC_DIMENSION_TIME, NC_UNLIMITED, &(meta->dim_time_id));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create dimension %s in output file %s, netCDF driver returned value of: %d.\n",
				NC_DIMENSION_TIME, abs_path, status);
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	status = nc_def_var(meta->ncid, NC_DIMENSION_TIME, NC_STRING, 1, &(meta->dim_time_id),
			&(meta->var_time_id));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to create variable %s for dimension %s in output file %s, netCDF driver returned value of: %d.\n",
				NC_DIMENSION_TIME, NC_DIMENSION_TIME, abs_path, status);
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
		snprintf(error_mesg, MAXSTR, "Unable to close netCDF file %s, netCDF driver returned value of: %d.\n", meta->abs_path, status);
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
	int *dimid_ptr = &(meta->dim_time_id);

	// Create variable for first field
	bool status = create_variable(f->variables, ncid, dimid_ptr);
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
		status = create_variable(v, ncid, dimid_ptr);
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
		snprintf(error_mesg, MAXSTR, "NetCDF driver error %d encountered when ending definition section of netCDF file %s.\n",
				retval, meta->abs_path);
		fprintf(stderr, error_mesg);
		free(error_mesg);
		return false;
	}

	return true;
}

bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const f,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	return false;
}
