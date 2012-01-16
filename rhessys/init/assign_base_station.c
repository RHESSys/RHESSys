/*--------------------------------------------------------------*/
/* 																*/
/*					assign_base_station.c 						*/
/*																*/
/*																*/
/*	NAME														*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct base_station_object
		*assign_base_station(
					 int		ID,
					 int		num_base_stations,
					 struct	base_station_object	**base_stations)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	i;
	struct	base_station_object *base_station;
	/*--------------------------------------------------------------*/
	/*	Loop through all of the basestations available.			*/
	/*	and find the record which holds the matching base station	*/
	/*--------------------------------------------------------------*/
	i = 0;
	while ( ID != (*(base_stations[i])).ID ) {
		i++;
		/*--------------------------------------------------------------*/
		/*	Report an error if no match was found.  Otherwise assign	*/
		/*	the base_station_pointer to point to this base_station.		*/
		/*--------------------------------------------------------------*/
		if ( i >= num_base_stations ){
			fprintf(stderr,
				"\nFATAL ERROR: in assign_base_stations, base station ID %d not found.\n",ID);
			exit(EXIT_FAILURE);
		}
	}  /* end-while */
	base_station = base_stations[i];
	return(base_station);
} /*end assign_base_station*/
