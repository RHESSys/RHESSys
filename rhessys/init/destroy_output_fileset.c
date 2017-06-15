/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_output_fileset						*/
/*																*/
/*	destroy_output_fileset - closes output files objects for 	*/
/*					one spatial level and all temporal levels.	*/
/*																*/
/*	NAME														*/
/*	destroy_output_fileset - closes output files objects for 	*/
/*					one spatial level and all temporal levels.	*/
/*																*/
/*	SYNOPSIS													*/
/*	void	destroy_output_fileset(								*/
/*								struct output_files_object *fileset)					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
void	destroy_output_fileset(	 struct	output_files_object *fileset)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	close the basin output files.							*/
	/*--------------------------------------------------------------*/
	fclose(  fileset[0].yearly );
	fclose(  fileset[0].monthly );
	fclose(  fileset[0].daily );
	fclose(  fileset[0].hourly );
	free( fileset );
	return;
} /*end destroy_output_fileset*/
