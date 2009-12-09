#ifndef INIT_H
#define INIT_H

#include "rhessys.h"

void construct_ascii_grid(
	char*	base_station_filename,
	struct	date start_date,
	struct date	duration,
	struct base_station_object**);

#endif
