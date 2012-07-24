/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_surface_energy_defaults				*/
/*																*/
/*	destroy_surface_energy_defaults.c - destroys surface_energy default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	destroy_surface_energy_defaults.c - makes surface_energy default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	destroy_surface_energy_defaults( 	num_default_files, 			*/
/*								  grow_flag,					*/
/*								  default_object_list )			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void destroy_surface_energy_defaults(int num_default_files,
							int grow_flag,
							struct surface_energy_default *default_object_list )
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Delete the default records (all at once since they were		*/
	/*	allocated in a contiguous array).							*/
	/*--------------------------------------------------------------*/
	free( default_object_list );
	return;
} /*end destroy_surface_energy_defaults*/
