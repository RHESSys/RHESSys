/*--------------------------------------------------------------*/
/*                                                              */
/*		find_stream												*/
/*                                                              */
/*  NAME                                                        */
/*		 find_stream											*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 find_stream( 								       		*/
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*                                                              */
/*																*/
/*	recursive routing to find the nearest stream link			*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"

int find_stream(struct flow_struct *flow_table, int curr, int *str_inx) {
	struct adj_struct *aptr;
	int i, inx, fnd;

	fnd = 0;
	aptr = flow_table[curr].adj_list;

	/* check to see if we are at the edge of the pit, find min elevation of */
	/*  apixel draining from the pit 										*/

	if (flow_table[curr].land != 1) {

		i = 1;
		while ((i <= flow_table[curr].num_adjacent) && (fnd == 0)) {
			inx = aptr->inx;

			if (aptr->landtype != STREAM)
				fnd = (int) find_stream(flow_table, inx, str_inx);

			aptr = aptr->next;
			i += 1;

		} /* end first pass */

	} /* end if */

	else {
		fnd = 1;
		*str_inx = curr;
	}

	return (fnd);

} /* end find_stream */

