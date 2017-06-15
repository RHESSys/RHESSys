/*--------------------------------------------------------------*/
/* 																*/
/*					execute_redefine_strata_event								*/
/*																*/
/*	execute_redefine_strata_event.c - creates a world object					*/
/*																*/
/*	NAME														*/
/*	execute_redefine_strata_event.c - creates a world object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void execute_redefine_strata_event_line(world, &command_line)	*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	execute_redefines stratum objects based on an input file			*/
/*	input file name is world file names suffexed with .str			*/
/*	format of input file is as follows					*/
/*	world_ID								*/
/*	num_basin								*/
/*	basin _ID								*/
/*		num_hillslopes							*/
/*		hill_ID								*/
/*		num_zones							*/
/*			zone_ID							*/
/*			num_patches						*/
/*			patch_ID						*/
/*			num_stratum						*/
/*			stratum_ID						*/
/*				stratum variables as in worldfile input         */
/*				.....for each stratum to be execute_redefined		*/

/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, March 15, 1998.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

void	execute_redefine_strata_event(struct world_object *world,
									  struct command_line_object *command_line,
									  struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void input_new_strata( struct command_line_object *,
		FILE *,
		int,
		struct base_station_object **,
		struct default_object *,
		struct patch_object *,
		struct canopy_strata_object *);
	struct patch_object	*find_patch( int,
		int, int, int,
		struct world_object *);
	struct canopy_strata_object	*find_stratum( int, int,
		int, int, int,
		struct world_object *);
	struct canopy_strata_object	*find_stratum_in_patch( int, 
		struct patch_object *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	FILE	*stratum_file;
	int	b,h,z,p,c;
	int	basin_ID, world_ID, hill_ID, zone_ID, patch_ID, stratum_ID;
	int	num_basin, num_hill, num_zone, num_patch, num_stratum;
	char	stratum_filename[MAXSTR];
	char	record[MAXSTR];
	char	ext[11];
	struct	canopy_strata_object	*stratum;
	struct	patch_object	*patch;
	/*--------------------------------------------------------------*/
	/*	Try to open the world file in read mode.					*/
	/*--------------------------------------------------------------*/
	sprintf(ext,".Y%4dM%dD%dH%d",current_date.year,
		current_date.month,
		current_date.day,
		current_date.hour);
	strcpy(stratum_filename, command_line[0].world_filename);
	strcat(stratum_filename, ext);
	if ( (stratum_file = fopen(stratum_filename,"r")) == NULL ){
		fprintf(stderr,
			"FATAL ERROR:  Cannot open stratum execute_redefine file %s\n",
			stratum_filename);
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Read in the world ID.							*/
	/*--------------------------------------------------------------*/
	fscanf(stratum_file,"%d",&world_ID);
	read_record(stratum_file, record);
	/*--------------------------------------------------------------*/
	/*	Read in the number of basin	files.							*/
	/*--------------------------------------------------------------*/
	fscanf(stratum_file,"%d",&num_basin);
	read_record(stratum_file, record);
	/*--------------------------------------------------------------*/
	/*	Construct the basins. 										*/
	/*--------------------------------------------------------------*/
	for (b=0; b < num_basin; b++ ){
		fscanf(stratum_file,"%d",&basin_ID);
		read_record(stratum_file, record);
		fscanf(stratum_file,"%d",&num_hill);
		read_record(stratum_file, record);
		for ( h = 0; h < num_hill; h++){
			fscanf(stratum_file,"%d",&hill_ID);
			read_record(stratum_file, record);
			fscanf(stratum_file,"%d",&num_zone);
			read_record(stratum_file, record);
			for ( z=0; z < num_zone; z++) {
				fscanf(stratum_file,"%d",&zone_ID);
				read_record(stratum_file, record);
				fscanf(stratum_file, "%d",&num_patch);
				read_record(stratum_file, record);
				for (p=0; p < num_patch; p++) {
					fscanf(stratum_file,"%d",&patch_ID);
					read_record(stratum_file, record);
					fscanf(stratum_file, "%d",&num_stratum);
					read_record(stratum_file, record);
					for (c=0; c < num_stratum; c++) {
						fscanf(stratum_file,"%d",&stratum_ID);
						read_record(stratum_file, record);
						patch = find_patch(patch_ID,
							zone_ID,
							hill_ID,
							basin_ID,
							world);
						stratum = find_stratum_in_patch(stratum_ID, patch);
						input_new_strata(command_line, stratum_file,
							world[0].num_base_stations,
							world[0].base_stations,
							world[0].defaults,
							patch,
							stratum);
					} /* end c */
				} /* end p */
			} /* end z */
		} /* end h */
	} /*end b */
	/*--------------------------------------------------------------*/
	/*	Close the stratum_file.										*/
	/*--------------------------------------------------------------*/
	if ( fclose(stratum_file) != 0 )
		exit(EXIT_FAILURE);
	return;
} /*end execute_redefine_strata_event.c*/
