/*--------------------------------------------------------------*/
/*                                                              */
/*		route_roads_to_patches									*/
/*                                                              */
/*  NAME                                                        */
/*		 route_roads_to_patches									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 route_roads_to_patches( 							    */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*	computes mean x,y,z for each pch							*/
/*	sorts pches by elevation									*/
/*	computes slope and gamma for neighbours of each pch		    */
/*		- gamma is 0 if slope is negative						*/
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h" 

int route_roads_to_patches(flow_table, num_patches, fl_flag)
	struct flow_struct *flow_table;
	int num_patches;
	int fl_flag;

    {

	int	find_min_flna();
	int	find_mmax_flna();

 	/* local variable declarations */
	int inx;
	int pch;
	float cell;


	struct adj_struct *aptr;


	cell = 30.0;
	/* calculate gamma for each neighbour */
	for (pch = 1; pch <= num_patches; pch++) 
		{
		aptr = flow_table[pch].adj_list;

		if (flow_table[pch].land == 2) {
			if (fl_flag)
				find_min_flna(flow_table, pch,&inx);
			else
				find_max_flna(flow_table, pch,&inx);

		  	flow_table[pch].stream_ID.hill = flow_table[inx].hillID;
		  	flow_table[pch].stream_ID.zone = flow_table[inx].zoneID;
		  	flow_table[pch].stream_ID.patch = flow_table[inx].patchID;
		  	flow_table[pch].stream_inx = inx;

			} /* end road */
		}

	return(1);


    }



