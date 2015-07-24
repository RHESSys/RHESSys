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
		*assign_base_station_xy(
					 float		x,
					 float		y,
					 int		num_base_stations,
					 int		*notfound,
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
	if (num_base_stations < 1) {
		*notfound = 1;
		return 0;
	}
	else {
		//printf("\n      Assign: Starting while loop: %lf %lf",(*(base_stations[i])).x,(*(base_stations[i])).y);
		while ( (x != (*(base_stations[i])).x) || (y != (*(base_stations[i])).y) ) {
			i++;
			/*--------------------------------------------------------------*/
			/*	Report an error if no match was found.  Otherwise assign	*/
			/*	the base_station_pointer to point to this base_station.		*/
			/*--------------------------------------------------------------*/
			if ( i >= num_base_stations ){
				//fprintf(stderr,"\n      Assign: NOT FOUND. Adding new base station for %lf %lf",x,y);
				*notfound = 1;
				return 0;
			}
		}  /* end-while */
	base_station = base_stations[i];
	return(base_station);
	}
} /*end assign_base_station*/
