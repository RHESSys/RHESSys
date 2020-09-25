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
	int status = nc_create(abs_path, NC_CLOBBER, &(meta->ncid));
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to open file %s, netCDF driver returned value of: %d", abs_path, status);
		perror(error_mesg);
		free(error_mesg);
		return false;
	}
	meta->abs_path = abs_path;
	// Save OutputFormatNetCDFMetadata to filter's output metadata so that the file can later be closed.
	f->output->meta = (void *)meta;

	return true;
}

bool output_format_netcdf_destroy(OutputFilter * const f) {
	if (f->output->format != OUTPUT_TYPE_NETCDF) {
		fprintf(stderr, "Cannot destroy netCDF output for non netCDF filter.\n");
		return false;
	}
	if (f->output->meta == NULL) {
		fprintf(stderr, "Failed to close netCDF output because no output metadata were found.\n");
	}
	OutputFormatNetCDFMetadata *meta = (OutputFormatNetCDFMetadata *)f->output->meta;
	int status = nc_close(meta->ncid);
	if (status != NC_NOERR) {
		char *error_mesg = malloc(MAXSTR * sizeof(char *));
		snprintf(error_mesg, MAXSTR, "Unable to close netCDF file %s, netCDF driver returned value of: %d", meta->abs_path, status);
		perror(error_mesg);
		free(error_mesg);
		return false;
	}

	free_metadata(meta);
	f->output->meta = NULL;

	return true;
}
bool output_format_netcdf_write_headers(OutputFilter * const f) {
	return false;
}

bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const f,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	return false;
}
