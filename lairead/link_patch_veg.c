/*--------------------------------------------------------------*/
/*                                                              */
/*		link_patch_veg									    */
/*                                                              */
/*  NAME                                                        */
/*		 link_patch_veg										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 link_patch_veg( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*  revision:  6.0 29 April, 2005                               */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"

void link_patch_veg(flow_table, allom_table, num_patches, nvegtype )
	struct flow_struct *flow_table;
	struct allom_struct *allom_table;
	int num_patches, nvegtype;

    {

 	/* local variable declarations */
	int i, fnd;
	int pch;

	/* compute mean pch values */
	for (pch = 1; pch <= num_patches; pch++) {
		fnd = 0;
		i = 0;
		while ( (i < nvegtype) && (fnd == 0) ) {
			if (flow_table[pch].vegid == allom_table[i].vegid) {
				fnd = 1;
				flow_table[pch].veglink = &(allom_table[i]);
				}
			i += 1;
		} /* end while */
		if (flow_table[pch].veglink == NULL)
			printf("\n Couldn't find vegtype %d for patch %d", flow_table[pch].vegid,
					flow_table[pch].patchID);
	}
		


	return;


    }



