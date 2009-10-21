/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_hillslope	 					*/
/*																*/
/*	destroy_hillslope.c - destroy hillslope object 		*/
/*																*/
/*	NAME														*/
/*	destroy_hillslope.c - destroy hillslope object 		*/
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

void destroy_hillslope(
					   struct	command_line_object	*command_line,
					   struct	hillslope_object	**hillslope_list)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	destroy_zone(
		struct	command_line_object	*,
		struct	zone_object	**);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int		i;
	struct	hillslope_object	*hillslope;
	/*--------------------------------------------------------------*/
	/*	destroy all the zones in this hillslope.					*/
	/*--------------------------------------------------------------*/
	hillslope = *hillslope_list;
	for ( i=0 ; i< hillslope[0].num_zones; i++ )
		destroy_zone( command_line, &(hillslope[0].zones[i]));
	/*--------------------------------------------------------------*/
	/*	destroy the list of zones.									*/
	/*--------------------------------------------------------------*/
	free(hillslope[0].zones);
	/*--------------------------------------------------------------*/
	/*	destroy the hillslope's grow extension if it exists.		*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag == 1)
		free(hillslope[0].grow);
	/*--------------------------------------------------------------*/
	/*	destroy the list of pointers to base stations.				*/
	/*--------------------------------------------------------------*/
	if ( hillslope[0].num_base_stations > 0 )
		free( hillslope[0].base_stations);
	/*--------------------------------------------------------------*/
	/*	Destroy the main hillslope object.							*/
	/*--------------------------------------------------------------*/
	free(hillslope);
	return;
} /*end destroy_hillslope*/
