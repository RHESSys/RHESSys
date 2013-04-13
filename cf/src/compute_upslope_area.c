/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_upslope_area									*/
/*                                                              */
/*  NAME                                                        */
/*		 compute_upslope_area									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 compute_upslope_area( 								    */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*	sorts pches by elevation									*/
/*	computes  upslope_area for neighbours of each pch			*/
/*                                                              */
/*  revision:  6.0 29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"

int compute_upslope_area(flow_table, num_patches, f1, rflag)
	struct flow_struct *flow_table;int num_patches;int rflag;FILE *f1;

{

	/* local fuction declarations */
	struct ID_struct sort_flow_table();

	// Not called
	//int find_patch();

	/* local variable declarations */

	int inx;
	int neigh;
	int pch;
	double cell;

	struct adj_struct *aptr;

	/* max_ID = sort_flow_table(flow_table, num_patches); */

	/* send area to  each upslope neighbour for each patch in the sort list  */

	for (pch = 1; pch <= num_patches; pch++) {
		aptr = flow_table[pch].adj_list;

		flow_table[pch].acc_area += flow_table[pch].area;

		/* process roads as all accumulated area going to stream (or receiving patch) */
		if ((flow_table[pch].land == 2) && (rflag == 1)) {
			inx = flow_table[pch].stream_inx;
			flow_table[inx].acc_area += flow_table[pch].acc_area;
		}

		/* otherwise distribute accumulated area to downstream neighbours */
		else {

			for (neigh = 1; neigh <= flow_table[pch].num_adjacent; neigh++) {
				inx = aptr->inx;

				flow_table[inx].acc_area += (flow_table[pch].acc_area)
						* aptr->gamma;

				aptr = aptr->next;
			}
		}

	}

	return (1);

}

