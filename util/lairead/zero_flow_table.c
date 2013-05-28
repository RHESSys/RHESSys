/*--------------------------------------------------------------*/
/*                                                              */
/*		zero_flow_table									*/
/*                                                              */
/*  NAME                                                        */
/*		 zero_flow_table										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 zero_flow_table( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*		- locates a patch based on ID value						*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
 

void	zero_flow_table(flow_table, maxr, maxc)
	struct flow_struct *flow_table;
	int maxr, maxc;

    {


	int r, c;
	int inx;

 for (r=0; r< maxr; r++) 
	 {
	 for (c=0; c< maxc; c++)
		 {
		 inx = r*maxc+c;
		 flow_table[inx].patchID = -99999;
		 flow_table[inx].zoneID = -99999;
		 flow_table[inx].hillID = -99999;
		 flow_table[inx].ID_order = -99999;
		 flow_table[inx].area = 0;
		 flow_table[inx].vegid = 0;
		 flow_table[inx].lai = -99999.0;
		 flow_table[inx].veglink = NULL;
		 }
	  }


	return;

    }



