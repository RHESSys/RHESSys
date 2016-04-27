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
                     struct	base_station_object	**base_stations
                     #ifdef LIU_NETCDF_READER
                     ,double dist_tol
                     #endif
                     #ifdef FIND_STATION_BASED_ON_ID
                     ,const int basestation_id
                     #endif
        )
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
        #ifdef FIND_STATION_BASED_ON_ID
        while ( (*(base_stations[i])).ID != basestation_id ) {
        #else
        #ifdef LIU_NETCDF_READER
        while ( !is_approximately(x,(*(base_stations[i])).x,dist_tol) || !is_approximately(y,(*(base_stations[i])).y,dist_tol) ) {
        #else
        while ( (x != (*(base_stations[i])).x) || (y != (*(base_stations[i])).y) ) {
        #endif
        #endif
            //printf("\n      Assign: Starting while loop: i:%d\tzone_baseid:%d\tbstation_id:%d\tbstation_x:%lf\tbstation_y:%lf\n",i,basestation_id,(*(base_stations[i])).ID,(*(base_stations[i])).x,(*(base_stations[i])).y);
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
