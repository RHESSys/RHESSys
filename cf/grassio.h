#ifndef GRASSIO_H
#define GRASSIO_H

#include <grass/gis.h>


double* raster2array(const char* name, 
				  	 struct Cell_head* header,
					 int* rows,
					 int* cols);
				

void array2raster(const void* data, 
				  const char* name, 
				  const RASTER_MAP_TYPE type, 
				  const int maxr, 
				  const int maxc);

#endif // GRASSIO_H
