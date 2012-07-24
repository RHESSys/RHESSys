/*--------------------------------------------------------------*/
/* 																*/
/*					construct_fire_grid						*/
/*																*/
/*	construct_fire_grid.c - creates a zone object				*/
/*																*/
/*	NAME														*/
/*	construct_fire_grid.c - creates a zone object				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct fire_grid *construct_fire_grid(					*/
/*					base_station_file_name,						*/
/*					start_date,									*/
/*					duration									*/
/*					column_index);								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

struct fire_object **construct_fire_grid (struct world_object *world, struct command_line_object *command_line)

{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_object *patch;
	int  b,h,p, z;
	double maxx, tmp;

	for (b=0; b< world[0].num_basin_files; ++b) {
	 for (h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
	   for (z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
			tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
	    for (p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
			patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p];

			if(patch[0].x > maxx)
				maxx = patch[0].x;
			tmp =  command_line[0].fire_grid_res; /* grid resolution */

			tmp = patch[0].z; /* elevation */
			tmp = patch[0].slope; /* slope */


	 
	} /* end patches */
	}
	}
	}

	world[0].num_fire_grid_row = tmp;
	world[0].num_fire_grid_col = tmp;
	
	return(fire_grid);
	
}

