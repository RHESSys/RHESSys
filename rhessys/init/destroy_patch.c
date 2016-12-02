/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_patch	 					*/
/*																*/
/*	destroy_patch.c - destroy patch object 		*/
/*																*/
/*	NAME														*/
/*	destroy_patch.c - destroy patch object 		*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMERS NOTES											*/
/*																*/
/*	Original code - MARCH 15, 1996								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void destroy_patch(
				   struct	command_line_object	*command_line,
				   struct	patch_object	**patch_list)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	destroy_canopy_stratum( struct command_line_object *,
		struct canopy_strata_object ** );
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	i;
	struct	patch_object	*patch;
	/*--------------------------------------------------------------*/
	/*	first destroy all the stratum in the patch.				*/
	/*--------------------------------------------------------------*/
	patch = *patch_list;
	for ( i=0 ; i< patch[0].num_canopy_strata; i++ )
		destroy_canopy_stratum(command_line,&(patch[0].canopy_strata[i]));
	/*--------------------------------------------------------------*/
	/*	destroy the list of canopy strata.							*/
	/*--------------------------------------------------------------*/
	free(patch[0].canopy_strata);
	/*--------------------------------------------------------------*/
	/*	destroy the patch grow extension if it exists.				*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag == 1)
		free(patch[0].grow);
	/*--------------------------------------------------------------*/
	/*	destroy the list of base stations.							*/
	/*--------------------------------------------------------------*/
	if ( patch[0].num_base_stations > 0 )
		free( patch[0].base_stations);
	
	
	/*--------------------------------------------------------------*/
	/*	destroy the routing list							*/
	/*--------------------------------------------------------------*/
	free(patch[0].innundation_list[0].neighbours);
	free(patch[0].innundation_list);
	free(patch[0].surface_innundation_list[0].neighbours);	
	free(patch[0].surface_innundation_list);	
	free(patch[0].transmissivity_profile);
	
	free(patch[0].hourly);
	free(patch[0].layers);
	/*--------------------------------------------------------------*/
	/*	destroy the main patch object.								*/
	/*--------------------------------------------------------------*/

	free(patch);
	return;
} /*end destroy_patch*/
