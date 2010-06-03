/*--------------------------------------------------------------*/
/*                                                              */
/*		add_roads									            */
/*                                                              */
/*  NAME                                                        */
/*		 add_roads										        */
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 add_roads( 								            */
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
/*  revision 6.0:  29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "fileio.h"
#include "blender.h"
 

void add_roads(flow_table, num_patches, f1, cell)
	struct flow_struct *flow_table;
	int num_patches;
	double cell;
	FILE *f1;

    {


 	/* local variable declarations */
	int inx;
	int neigh;
	int pch;
	int fnd;



	struct adj_struct *aptr;
	struct ID_struct ; /*max_ID;*/


	/* calculate gamma for each neighbour */
	for (pch = 1; pch <= num_patches; pch++) 
		{
		aptr = flow_table[pch].adj_list;

		if (flow_table[pch].land == 2) {

			/*
			fnd = ( int )find_stream(flow_table, pch, &inx);
			
			if (fnd == 0) {
				printf("\n Cannot find a stream below this road %d %d", flow_table[pch].patchID, flow_table[pch].hillID);
				printf("\n so we will just link the road runoff with the patch itself");
				inx = flow_table[pch].adj_list->inx; 
				}
			*/

			/* so now we always route road/sewer network flow to basin outlet - a trick to avoid flat area issues */
			/* once we actually route in the stream this will no longer work and a better fix is needed */
				inx = num_patches;
				flow_table[pch].stream_ID.hill = flow_table[inx].hillID;
				flow_table[pch].stream_ID.zone = flow_table[inx].zoneID;
				flow_table[pch].stream_ID.patch = flow_table[inx].patchID;
				flow_table[pch].stream_inx = inx;

/* 				neigh = flow_table[pch].num_adjacent + 1; */

                   } /* end if road */

		}

	return;


    }



