#include <netcdf.h>

#include "rhessys.h"
#include "output_filter/output_format_netcdf.h"


bool output_format_netcdf_init(OutputFilter * const filter) {
	return false;
}

bool output_format_netcdf_destroy(OutputFilter * const filter) {
	return false;

}
bool output_format_netcdf_write_headers(OutputFilter * const filter) {
	return false;
}

bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const filter,
		EntityID id, MaterializedVariable * const vars, bool flush) {
	return false;
}
