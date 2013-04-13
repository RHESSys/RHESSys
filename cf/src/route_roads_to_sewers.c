/*--------------------------------------------------------------*/
/*                                                              */
/*		route_roads_to_sewers									*/
/*                                                              */
/*  NAME                                                        */
/*		 route_roads_to_sewers									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 route_roads_to_sewers( 							    */
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

#include "fileio.h"
#include "blender.h"

void route_roads_to_sewers(flow_table, num_patches, f1, cell)
	struct flow_struct *flow_table;int num_patches;double cell;FILE *f1;

{

	/* local variable declarations */
	int inx;
	int neigh;
	int pch;
	int fnd;

	struct adj_struct *aptr;
	struct ID_struct;
	/*max_ID;*/

	/* calculate gamma for each neighbour */
	for (pch = 1; pch <= num_patches; pch++) {
		aptr = flow_table[pch].adj_list;

		if (flow_table[pch].land == 2) {
			fnd = (int) find_sewer(flow_table, pch, &inx);
			if (fnd == 0) {
				exit(EXIT_FAILURE);
			}
			if (flow_table[inx].land == 1) {
				flow_table[pch].stream_ID.hill = flow_table[inx].hillID;
				flow_table[pch].stream_ID.zone = flow_table[inx].zoneID;
				flow_table[pch].stream_ID.patch = flow_table[inx].patchID;
				flow_table[pch].stream_inx = inx;
				neigh = flow_table[pch].num_adjacent + 1;
			}

		} /* end if road */

	}

	return;

}

