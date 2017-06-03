/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_basin	 					*/
/*																*/
/*	destroy_basin.c - destroy basin object 		*/
/*																*/
/*	NAME														*/
/*	destroy_basin.c - destroy basin object 		*/
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

void destroy_basin(
				   struct	command_line_object	*command_line,
				   struct	basin_object	**basin_list)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	destroy_hillslope(
		struct	command_line_object	*,
		struct	hillslope_object	**);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int		i;
	struct	basin_object	*basin;
	/*--------------------------------------------------------------*/
	/*	first destroy all of the hillslopes in this basin.			*/
	/*--------------------------------------------------------------*/
	basin = *basin_list;
	for ( i=0 ; i< basin[0].num_hillslopes; i++ )
		destroy_hillslope( command_line, &(basin[0].hillslopes[i]));
	/*--------------------------------------------------------------*/
	/*	destroy the list of hillslopes.								*/
	/*--------------------------------------------------------------*/
	free(basin[0].hillslopes);
	/*--------------------------------------------------------------*/
	/*	Destroy the basins grow extension if it exists.			*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag == 1)
		free(basin[0].grow);
	/*--------------------------------------------------------------*/
	/*	destroy the list of base stations	*/
	/*--------------------------------------------------------------*/
	if ( basin[0].num_base_stations > 0 )
		free( basin[0].base_stations);
	/*--------------------------------------------------------------*/
	/*	destroy the list of route_list: need further free	*/
	/*--------------------------------------------------------------*/
        if (command_line[0].routing_flag==1){
	    free(basin[0].route_list[0].list);
	    free(basin[0].route_list); 
	    free(basin[0].surface_route_list[0].list);
	    free(basin[0].surface_route_list);	    
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the main basin object.								*/
	/*--------------------------------------------------------------*/
	free(basin);
	return;
} /*end destroy_basin*/
