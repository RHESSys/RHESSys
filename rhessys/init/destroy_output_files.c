/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_output_files						*/
/*																*/
/*	destroy_output_files - closes output files objects.			*/
/*																*/
/*	NAME														*/
/*	destroy_output_files - closes output files objects.			*/
/*																*/
/*	SYNOPSIS													*/
/*	struct	world_output_file_object *destroy_output_files(		*/
/*   						command_line, output ) 				*/	
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Closes open output files.									*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"


void	destroy_output_files(
							 struct	command_line_object *command_line,
							 struct	world_output_file_object *output)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*destroy_output_fileset( struct	output_files_object	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Destroy the basin output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].b != NULL ){
		destroy_output_fileset( output[0].basin );
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the hillslope output files.						*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].h != NULL ){
		destroy_output_fileset( output[0].hillslope );
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the zone output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].z != NULL ){
		destroy_output_fileset(output[0].zone );
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the patch output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].p != NULL ){
		destroy_output_fileset( output[0].patch);
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the canopy_stratum output files.		*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].c != NULL ){
		destroy_output_fileset( output[0].canopy_stratum);
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the world output file object.						*/
	/*--------------------------------------------------------------*/
	if ((command_line[0].b != NULL) || (command_line[0].h != NULL) ||
		(command_line[0].z != NULL) || (command_line[0].p != NULL) ||
		(command_line[0].c != NULL)){
		free( output );
	}
	return;
} /*end destroy_output_files*/
