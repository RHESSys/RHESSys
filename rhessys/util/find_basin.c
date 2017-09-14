/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_basin_in_world					*/
/*                                                              */
/*  NAME                                                        */
/*		find_basin_in_world					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_basin_in_world( struct world_object *world)			*/
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

struct basin_object *find_basin(
										  int basin_ID,
										  struct world_object *world)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct basin_object *basin;

	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	basin = NULL;
	
	while ( (fnd == 0) && (i >= 0) && (i < world[0].num_basin_files)) {
		if (world[0].basins[i][0].ID == basin_ID) {
			basin = world[0].basins[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
				"\nCould not find basin %d in world %d. Skipping basin...\n",
				basin_ID,
				world[0].ID);
		basin = NULL;
		/*exit(EXIT_FAILURE);*/
	}
	return(basin);
}/*end find_basin_in_world */
