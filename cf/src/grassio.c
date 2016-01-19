#include <stdlib.h>

#include "grassio.h"

void* raster2array(const char* name, struct Cell_head* header, int* rows,
		int* cols, RASTER_MAP_TYPE out_type) {
	// Open the raster map and load the dem
	// for simplicity sake, the dem will be an array of
	// doubles, converted from any possible GRASS CELL type.
	const char* mapset = G_find_raster2(name, "");
	if (mapset == NULL)
		G_fatal_error("Raster map <%s> not found", name);

	// Find out the cell type of the DEM
	RASTER_MAP_TYPE type = Rast_map_type(name, mapset);

	// Get a file descriptor for the DEM raster map
	int infd;
	if ((infd = Rast_open_old(name, mapset)) < 0)
		G_fatal_error("Unable to open raster map <%s>", name);

	// Get header info for the DEM raster map
	struct Cell_head cellhd;
	Rast_get_cellhd(name, mapset, &cellhd);

	// Create a GRASS buffer for the DEM raster
	void* inrast = Rast_allocate_buf(type);

	// Get the max rows and max cols from the window information, since the 
	// header gives the values for the full raster
	const int maxr = Rast_window_rows();
	const int maxc = Rast_window_cols();

	// Read in the raster line by line, copying it into the double array
	// rast for return.
	void* rast;
	switch (out_type) {
	case CELL_TYPE:
		rast = (int*) calloc(maxr * maxc, sizeof(int));
		break;
	case FCELL_TYPE:
		rast = (float*) calloc(maxr * maxc, sizeof(float));
		break;
	case DCELL_TYPE:
		rast = (double*) calloc(maxr * maxc, sizeof(double));
		break;

	}

	if (rast == NULL) {
		G_fatal_error("Unable to allocate memory for raster map <%s>", name);
	}

	int row, col;
	for (row = 0; row < maxr; ++row) {
	      Rast_get_row(infd, inrast, row, type);

		for (col = 0; col < maxc; ++col) {
			int index = col + row * maxc;

			if (out_type == CELL_TYPE) {
				switch (type) {
				case CELL_TYPE:
					((int*) rast)[index] = ((int *) inrast)[col];
					break;
				case FCELL_TYPE:
					((int*) rast)[index] = (int) ((float *) inrast)[col];
					break;
				case DCELL_TYPE:
					((int*) rast)[index] = (int) ((double *) inrast)[col];
					break;
				default:
					G_fatal_error("Unknown cell type");
					break;
				}
			}

			if (out_type == FCELL_TYPE) {
				switch (type) {
				case CELL_TYPE:
					((float*) rast)[index] = (float) ((int *) inrast)[col];
					break;
				case FCELL_TYPE:
					((float*) rast)[index] = ((float *) inrast)[col];
					break;
				case DCELL_TYPE:
					((float*) rast)[index] = (float) ((double *) inrast)[col];
					break;
				default:
					G_fatal_error("Unknown cell type");
					break;
				}
			}

			if (out_type == DCELL_TYPE) {
				switch (type) {
				case CELL_TYPE:
					((double*) rast)[index] = (double) ((int *) inrast)[col];
					break;
				case FCELL_TYPE:
					((double*) rast)[index] = (double) ((float *) inrast)[col];
					break;
				case DCELL_TYPE:
					((double*) rast)[index] = ((double *) inrast)[col];
					break;
				default:
					G_fatal_error("Unknown cell type");
					break;
				}
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

void array2raster(const void* data, const char* name,
		const RASTER_MAP_TYPE type, const int maxr, const int maxc) {
	void* rast = Rast_allocate_buf(type);
	int fd;
	if ((fd = Rast_open_new(name, type)) < 0) {
		G_fatal_error("Unable to create raster map <%s>", name);
	}

	int row, col;
	for (row = 0; row < maxr; ++row) {
		for (col = 0; col < maxc; ++col) {
			int i = row * maxc + col;
			switch (type) {
			case CELL_TYPE:
				((int*) rast)[col] = ((int*) data)[i];
				break;
			case FCELL_TYPE:
				((float*) rast)[col] = ((float*) data)[i];
				break;
			case DCELL_TYPE:
				((double*) rast)[col] = ((double*) data)[i];
				break;
			}
		}

		if (Rast_put_row(fd, rast, type) < 0) {
			G_fatal_error("Failed writing raster map <%s>", name);
		}
	}

	G_free(rast);
	Rast_close(fd);

	return;
}
