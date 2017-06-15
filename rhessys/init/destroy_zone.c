/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_zone	 					*/
/*																*/
/*	destroy_zone.c - destroy zone object 		*/
/*																*/
/*	NAME														*/
/*	destroy_zone.c - destroy zone object 		*/
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

void destroy_zone(
				  struct	command_line_object	*command_line,
				  struct	zone_object	**zone_list)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	destroy_patch(
		struct	command_line_object	*,
		struct	patch_object	**);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	i;
	struct	zone_object	*zone;
	/*--------------------------------------------------------------*/
	/*	first destroy all the patches in the zone.				*/
	/*--------------------------------------------------------------*/
	zone = *zone_list;
	for ( i=0 ; i< zone[0].num_patches; i++ )
		destroy_patch( command_line, &(zone[0].patches[i]) );
	/*--------------------------------------------------------------*/
	/*	destroy the list of patches.								*/
	/*--------------------------------------------------------------*/
	free(zone[0].patches);
	/*--------------------------------------------------------------*/
	/*	destroy the zone grow extension if it exists.				*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag == 1)
		free(zone[0].grow);
	/*--------------------------------------------------------------*/
	/*	destroy the list of pointers to the base stations 			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].num_base_stations > 0 ){
	       free(zone[0].base_stations);
	}
      
	/*--------------------------------------------------------------*/
	/*	destroy the hourly zone			*/
	/*--------------------------------------------------------------*/
	free(zone[0].hourly);
	/*--------------------------------------------------------------*/
	/*	Destroy the main zone object.								*/
	/*--------------------------------------------------------------*/
	free(zone);
	return;
} /*end destroy_zone*/
