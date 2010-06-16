/*--------------------------------------------------------------*/
/*                                                              */
/*		find_patch									            */
/*                                                              */
/*  NAME                                                        */
/*		 find_patch										        */
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 find_patch( 								            */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*		- locates a patch based on ID value						*/
/*  revision:  6.0  29 April, 2005                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"

int	find_patch(num_patches, flow_table,  patchID,  zoneID,  hillID)
	struct flow_struct *flow_table;  
	int patchID, hillID, zoneID;     

    {

	int fnd,inx;

	fnd = 0;
	inx = 1;

	while ( (fnd == 0) && (inx <= num_patches) ) {
		 if ( (flow_table[inx].patchID == patchID) &&
		 	(flow_table[inx].hillID == hillID) &&
		 	(flow_table[inx].zoneID == zoneID) )
				fnd = inx;
		 else	inx += 1;
		 }


	return(fnd);

    }



