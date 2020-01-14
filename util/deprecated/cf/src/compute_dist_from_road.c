/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_dist_from_road									*/
/*                                                              */
/*  NAME                                                        */
/*		 compute_dist_from_road									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 compute_dist_from_road( 							    */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*	sorts pches by elevation									*/
/*	computes  dist_from_road for neighbours of each pch			*/
/*  revision: 6.0  29 April, 2005                               */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"

int compute_dist_from_road(flow_table, num_patches, f1, cell)
	struct flow_struct *flow_table;int num_patches;float cell;FILE *f1;

{

	/* local variable declarations */
	int inx;
	int neigh;
	int pch;
	double dist, xrun, yrun;

	struct adj_struct *aptr;

	/* send area to  each upslope neighbour for each patch in the sort list  */

	for (pch = 1; pch <= num_patches; pch++) {
		aptr = flow_table[pch].adj_list;

		if (flow_table[pch].land == 2) {
			flow_table[pch].road_dist = (float) (sqrt(cell));
			flow_table[pch].inflow_cnt = 1;
		}

		/* process roads as all accumulated area going to stream (or receiving patch) */
		if (flow_table[pch].road_dist > 0) {

			for (neigh = 1; neigh <= flow_table[pch].num_adjacent; neigh++) {

				inx = aptr->inx;

				xrun = pow((flow_table[pch].x - flow_table[inx].x), 2.0);
				yrun = pow((flow_table[pch].y - flow_table[inx].y), 2.0);

				dist = sqrt(xrun + yrun);

				if (flow_table[inx].land == 1)
					flow_table[inx].road_dist = 0.0;
				else {
					if (aptr->gamma > 0.0) {
						flow_table[inx].road_dist +=
								(float) (flow_table[pch].road_dist
										/ flow_table[pch].inflow_cnt + dist);
						flow_table[inx].inflow_cnt += 1;
					}
				}

				aptr = aptr->next;
			}
		}

	}

	for (pch = 1; pch <= num_patches; pch++) {

		if (flow_table[pch].inflow_cnt == 0)
			fprintf(f1, "%d 0.0\n", flow_table[pch].patchID);
		else
			fprintf(f1, "%d %f\n", flow_table[pch].patchID,
					flow_table[pch].road_dist / flow_table[pch].inflow_cnt);
	}

	return (1);
}

