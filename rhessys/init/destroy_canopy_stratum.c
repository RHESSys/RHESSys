/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_canopy_stratum	 					*/
/*																*/
/*	destroy_canopy_stratum.c - destroy strata objects 		*/
/*																*/
/*	NAME														*/
/*	destroy_canopy_stratum.c - destroy strata objects 		*/
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

void destroy_canopy_stratum(
							struct	command_line_object	*command_line,
							struct	canopy_strata_object	**stratum_list)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	struct	canopy_strata_object	*stratum;
	/*--------------------------------------------------------------*/
	/*	destroy the stratum grow extension if it exists.				*/
	/*--------------------------------------------------------------*/
	stratum = *stratum_list;
	/*--------------------------------------------------------------*/
	/*	destroy the list of base stations.	*/
	/*--------------------------------------------------------------*/
	if ( stratum[0].num_base_stations > 0 )
		free(stratum[0].base_stations);
	/*--------------------------------------------------------------*/
	/*	destroy the main stratum object		*/
	/*--------------------------------------------------------------*/
	free(stratum);
	return;
} /*end destroy_stratum*/
