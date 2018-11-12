/*--------------------------------------------------------------*/
/* 																*/
/*					execute_state_output_event					*/
/*																*/
/*	execute_state_output_event - outputs state data			*/
/*																*/
/*	NAME														*/
/*	execute_state_output_event - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	execute_state_output_event(						*/
/*					struct	world_object	*world,				*/
/*					struct	date	current_date,				*/
/*					struct	date	end_date,				*/
/*					struct	command_line_object *command_line)	*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current world state - in worldfile format			*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	execute_state_output_event(
								   struct	world_object	*world,
								   struct	date	current_date,
								   struct	date	end_date,
								   struct	command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_basin_state(
		struct	basin_object *,
		struct	date,
		struct	command_line_object *,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int b,i;
	FILE	*outfile;
	char	filename[MAXSTR+100];
	char	ext[20];
	/*--------------------------------------------------------------*/
	/*	Try to open the world file in read mode.					*/
	/*--------------------------------------------------------------*/
	sprintf(ext,".Y%4dM%dD%dH%d",current_date.year,
		current_date.month,
		current_date.day,
		current_date.hour);

	strcpy(filename, command_line[0].world_filename);
	strcat(filename, ext);
	strcat(filename, ".state");

	/*--------------------------------------------------------------*/
	/*	open output file											*/
	/*--------------------------------------------------------------*/
	if ( ( outfile = fopen(filename, "w")) == NULL ){
		fprintf(stderr,"FATAL ERROR: in execute_state_output_event.\n");
		exit(EXIT_FAILURE);
	}

	fprintf(outfile, "\n%-30ld %s", world[0].ID,
		"world_id");
	fprintf(outfile, "\n%-30ld %s", world[0].num_basin_files,
		"num_basins");
	/*--------------------------------------------------------------*/
	/*	output basins												*/
	/*--------------------------------------------------------------*/
	for (b=0; b < world[0].num_basin_files; ++ b ) {
        printf("output basin state ...\n");
		output_basin_state(world[0].basins[b], current_date, command_line, outfile);
        printf("output basin state finished\n");
	}
	fclose(outfile);
    printf("output basin state file closed\n");
	return;
} /*end execute_state_output_event*/
