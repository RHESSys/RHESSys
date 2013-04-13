/*--------------------------------------------------------------*/
/*                                                              */
/*		find_max_flna											*/
/*                                                              */
/*  NAME                                                        */
/*		 find_max_flna											*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 find_max_flna( 								       	*/
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*                                                              */
/*																*/
/*	recursive routing to find the top of a pit					*/
/*  and to create a list of all patches in a pit  				*/
/*  the top of a pit is the patch with the minimum				*/
/* 	elevation which points to somewhere outside the				*/
/*  pit															*/
/*  revision: 6.0  29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"

void find_max_flna(struct flow_struct *flow_table, int curr, int *str_inx)

{

	struct adj_struct *aptr;
	int i, inx;
	double max_flna;

	aptr = flow_table[curr].adj_list;
	max_flna = flow_table[aptr->inx].flna;

	/* check to see if we are at the edge of the pit, find min elevation of */
	/*  apixel draining from the pit 										*/

	i = 1;
	while ((i <= flow_table[curr].num_adjacent)) {
		inx = aptr->inx;

		if ((flow_table[inx].flna >= max_flna) && (aptr->gamma != 0.0)) {
			*str_inx = inx;
			max_flna = flow_table[inx].flna;
		}

		aptr = aptr->next;
		i += 1;

	} /* while */

	return;

} /* end find_max_flna */

