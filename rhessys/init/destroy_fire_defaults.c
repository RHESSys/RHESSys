/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_fire_defaults				*/
/*																*/
/*	destroy_fire_defaults.c - destroys fire default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	destroy_fire_defaults.c - makes fire default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	destroy_fire_defaults( 	num_default_files, 			*/
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

void destroy_fire_defaults(int num_default_files,
							int grow_flag,
							struct fire_default *default_object_list )
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
} /*end destroy_fire_defaults*/
