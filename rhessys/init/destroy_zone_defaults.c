/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_zone_defaults				*/
/*																*/
/*	destroy_zone_defaults.c - destroys zone default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	destroy_zone_defaults.c - makes zone default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	destroy_zone_defaults( 	num_default_files, 			*/
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

void destroy_zone_defaults(int num_default_files,
						   int grow_flag,
						   struct zone_default *default_object_list )
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int 	i;
	/*--------------------------------------------------------------*/
	/*	Delete grow extensions if present.							*/
	/*--------------------------------------------------------------*/
	if ( grow_flag ){
		/*--------------------------------------------------------------*/
		/*		Loop through all the default files and free grow extens */
		/*--------------------------------------------------------------*/
		for ( i=0 ; i<num_default_files; i++ )
			free( default_object_list[i].grow_defaults );
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Delete the default records (all at once since they were		*/
	/*	allocated in a contiguous array).							*/
	/*--------------------------------------------------------------*/
	free( default_object_list );
	return;
} /*end destroy_zone_defaults*/
