/*--------------------------------------------------------------*/
/* 																*/
/*					construct_output_files						*/
/*																*/
/*	construct_output_files - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	construct_output_files - creates output files objects.		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct	world_output_file_object *construct_output_files(	*/
/*   						command_line ) 						*/	
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates file pointers for output files for levels of 		*/
/*	object hierarchy which the user has designated output.		*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*	March 14, 1997 - RAF										*/
/*	The stratum fileset is constructed if either stratuim or	*/
/*	patch output is requested since a patch output may			*/
/*	include a moss stratum.										*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

struct	world_output_file_object *construct_output_files(
														 char 	*prefix,
														 struct	command_line_object *command_line )
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct	output_files_object	*construct_output_fileset(char *);
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	char	root[256];
	struct	world_output_file_object	*world_output_file = NULL;
	/*--------------------------------------------------------------*/
	/*	Allocate a world output file object if any output is        */
	/*	selected or print a warning.								*/
	/*--------------------------------------------------------------*/
	if ((command_line[0].b != NULL) || (command_line[0].h != NULL) ||
		(command_line[0].z != NULL) || (command_line[0].p != NULL) ||
		(command_line[0].c != NULL)|| (command_line[0].stro != NULL) ){
		world_output_file = (struct world_output_file_object *)
			alloc(1 * sizeof(struct world_output_file_object),
			"world_output_file","construct_output_files");
	}
	else{
		fprintf(stderr,
			"WARNING: in construct_output_file no output has been selected.\n");
		return(world_output_file);
	}
/*--------------------------------------------------------------*/
	/*	Construct the stream_routing output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].stro != NULL ){
		strcpy(root,prefix);
		strcat(root, "_streamrouting");
		world_output_file[0].stream_routing = construct_output_fileset(root);
	}
	/*--------------------------------------------------------------*/
	/*	Construct the basin output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].b != NULL ){
		strcpy(root,prefix);
		strcat(root, "_basin");
		world_output_file[0].basin = construct_output_fileset(root);
	}
	/*--------------------------------------------------------------*/
	/*	Construct the hillslope output files.						*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].h != NULL ){
		strcpy(root,prefix);
		strcat(root, "_hillslope");
		world_output_file[0].hillslope = construct_output_fileset(root);
	}
	/*--------------------------------------------------------------*/
	/*	Construct the zone output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].z != NULL ){
		strcpy(root, prefix);
		strcat(root, "_zone");
		world_output_file[0].zone = construct_output_fileset(root);
	}
	/*--------------------------------------------------------------*/
	/*	Construct the patch output files.							*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].p != NULL ){
		strcpy(root, prefix);
		strcat(root, "_patch");
		world_output_file[0].patch = construct_output_fileset(root);
	}
	/*--------------------------------------------------------------*/
	/*	Construct the canopy stratum output files.					*/
	/*	Always do this if a patch is requestedon the off chance we have moss.*/
	/*--------------------------------------------------------------*/
	if ( (command_line[0].c != NULL) || (command_line[0].p != NULL ) ){
		strcpy(root, prefix);
		strcat(root, "_stratum");
		world_output_file[0].canopy_stratum = construct_output_fileset(root);
  }
 if (command_line[0].vegspinup_flag > ZERO)  {
	  strcpy(root, prefix);
		strcat(root, "_shadow_stratum");
		world_output_file[0].shadow_strata = construct_output_fileset(root);
  }
	fprintf(stderr,"FINISHED CONSTRUCT OUTPUT FILES\n");
	return(world_output_file);
} /*end construct_output_files*/
