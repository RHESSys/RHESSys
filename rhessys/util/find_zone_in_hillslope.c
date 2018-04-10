/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_zone_in_hillslope					*/
/*                                                              */
/*  NAME                                                        */
/*		find_zone_in_hillslope					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_zone_in_hillslope( struct basin_object *basin)			*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct zone_object *find_zone_in_hillslope(
										  int zone_ID,
										  struct hillslope_object *hillslope)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct zone_object *zone;

	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	zone = NULL;
	
	while ( (fnd == 0) && (i >= 0) && (i < hillslope[0].num_zones)) {
		if (hillslope[0].zones[i][0].ID == zone_ID) {
			zone = hillslope[0].zones[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		/*fprintf(stderr,
			"FATAL ERROR: Could not find zone %d in  hillslope %d \n",
			zone_ID,
			hillslope[0].ID);*/
		zone = NULL;
		/*exit(EXIT_FAILURE);*/
	}
	return(zone);
}/*end find_zone_in_hillslope */
