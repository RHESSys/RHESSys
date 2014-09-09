/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_scm_defaults				*/
/*																*/
/*	destroy_scm_defaults.c - destroys scm default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	destroy_scm_defaults.c - destorys scm default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	destroy_scm_defaults( 	num_default_files, 			*/
/*								  grow_flag,					*/
/*								  default_object_list )			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	Copied from destroy_fire_deaults by Colin Bell															*/
/*	Original code, January 15, 1996.			/*
/*   Copied May 16, 2014				*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void destroy_scm_defaults(int num_default_files,
							int grow_flag,
							struct scm_default *default_object_list )
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
} /*end destroy_scm_defaults*/
