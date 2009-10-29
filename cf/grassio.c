
#include <stdlib.h>

#include "grassio.h"


double* raster2array(const char* name, 
					 struct Cell_head* header, 
					 int* rows, 
					 int* cols) 
{
	// Open the raster map and load the dem
	// for simplicity sake, the dem will be an array of
	// doubles, converted from any possible GRASS CELL type.
	char* mapset = G_find_cell2(name, "");
	if (mapset == NULL)
		G_fatal_error("Raster map <%s> not found", name);

	// Find out the cell type of the DEM
	RASTER_MAP_TYPE type = G_raster_map_type(name, mapset);

	// Get a file descriptor for the DEM raster map
	int infd;
	if ((infd = G_open_cell_old(name, mapset)) < 0)
		G_fatal_error("Unable to open raster map <%s>", name);

	// Get header info for the DEM raster map
	struct Cell_head cellhd;
	if (G_get_cellhd(name, mapset, &cellhd) < 0)
		G_fatal_error("Unable to open raster map <%s>", name);

	// Create a GRASS buffer for the DEM raster
	void* inrast = G_allocate_raster_buf(type);

	// Get the max rows and max cols from the window information, since the 
	// header gives the values for the full raster
	const int maxr = G_window_rows();
	const int maxc = G_window_cols();

	// Read in the raster line by line, copying it into the double array
	// rast for return.
	double* rast = (double*)calloc(maxr * maxc, sizeof(double));

	if (rast == NULL) {
		G_fatal_error("Unable to allocate memory for raster map <%s>", name);
	}

	int row, col;
	for (row = 0; row < maxr; ++row) {
		if (G_get_raster_row(infd, inrast, row, type) < 0)
			G_fatal_error("Unable to read raster map <%s> row %d", name, row);

		for (col = 0; col < maxc; ++col) {
			int index= col + row*maxc;
			switch (type) {
				case CELL_TYPE:
					((double*)rast)[index] = (double)((int *) inrast)[col];
					break;
				case FCELL_TYPE:
					((double*)rast)[index] = (double)((float *) inrast)[col];
					break;
				case DCELL_TYPE:
					((double*)rast)[index] = ((double *) inrast)[col];
					break;
				default:
					G_fatal_error("Unknown cell type");
					break;
			}
		}
	}

	// Return cellhd, maxr, and maxc by pointer
	if (header != NULL)
		*header = cellhd;
	if (rows != NULL)
		*rows = maxr;
	if (cols != NULL)	
		*cols = maxc;

	return rast;
}


void array2raster(const void* data, 
				  const char* name, 
				  const RASTER_MAP_TYPE type, 
				  const int maxr, 
				  const int maxc)
{
	void* rast = G_allocate_raster_buf(type);
	int fd;
	if ((fd = G_open_raster_new(name, type)) < 0) {
		G_fatal_error("Unable to create raster map <%s>", name);
	}

	int row, col;
	for (row = 0; row < maxr; ++row) {
		for (col = 0; col < maxc; ++col) {
			int i = row*maxc + col;
			switch (type) {
				case CELL_TYPE:
					((int*)rast)[col] = ((int*)data)[i];	
					break;
				case FCELL_TYPE:
					((float*)rast)[col] = ((float*)data)[i];	
					break;
				case DCELL_TYPE:
					((double*)rast)[col] = ((double*)data)[i];	
					break;
			}
		}

		if (G_put_raster_row(fd, rast, type) < 0) {
			G_fatal_error("Failed writing raster map <%s>", name);
		}
	}

	G_free(rast);
	G_close_cell(fd);

	return;
}
