/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_hillslope_defaults				*/
/*																*/
/*	destroy_hillslope_defaults.c - makes hillslope default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	destroy_hillslope_defaults.c - makes hillslope default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	destroy_hillslope_defaults( 	num_default_files, 			*/
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

void destroy_hillslope_defaults(int num_default_files,
								int grow_flag,
								struct hillslope_default *default_object_list )
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	int 	i;
	/*--------------------------------------------------------------*/
	/*	Delete the default records (all at once since they were		*/
	/*	allocated in a contiguous array).							*/
	/*--------------------------------------------------------------*/
	free( default_object_list );
	return;
} /*end destroy_hillslope_defaults*/
