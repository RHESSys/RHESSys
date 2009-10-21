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
		exit(0);
	}
	/*--------------------------------------------------------------*/
	/*	output world information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile, "\n%-30ld %s", current_date.year, "current_year");
	fprintf(outfile, "\n%-30ld %s", current_date.month, "current_month");
	fprintf(outfile, "\n%-30ld %s", current_date.day, "current_day");
	fprintf(outfile, "\n%-30ld %s", current_date.hour, "current_hour");
	fprintf(outfile, "\n%-30ld %s", end_date.year, "end_year");
	fprintf(outfile, "\n%-30ld %s", end_date.month, "end_month");
	fprintf(outfile, "\n%-30ld %s", end_date.day, "end_day");
	fprintf(outfile, "\n%-30ld %s", end_date.hour, "end_hour");
	fprintf(outfile, "\n%-30ld %s", world[0].defaults[0].num_basin_default_files,
		"num_basin_default_files");
	for (i=0; i<world[0].defaults[0].num_basin_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].basin_default_files[i],
			"basin_default_file");
	}
	fprintf(outfile, "\n%-30ld %s",
		world[0].defaults[0].num_hillslope_default_files,
		"num_hillslope_default_files");
	for (i=0; i<world[0].defaults[0].num_hillslope_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].hillslope_default_files[i],
			"hillslope_default_file");
	}
	fprintf(outfile, "\n%-30ld %s", world[0].defaults[0].num_zone_default_files,
		"num_zone_default_files");
	for (i=0; i<world[0].defaults[0].num_zone_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].zone_default_files[i],
			"zone_default_file");
	}

	fprintf(outfile, "\n%-30ld %s", world[0].defaults[0].num_soil_default_files,
		"num_soil_default_files");
	for (i=0; i<world[0].defaults[0].num_soil_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].soil_default_files[i],
			"soil_default_file");
	}
	fprintf(outfile, "\n%-30ld %s", world[0].defaults[0].num_landuse_default_files,
		"num_landuse_default_files");
	for (i=0; i<world[0].defaults[0].num_landuse_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].landuse_default_files[i],
			"landuse_default_file");
	}
	fprintf(outfile, "\n%-30ld %s",
		world[0].defaults[0].num_stratum_default_files,
		"num_stratum_default_files");
	for (i=0; i<world[0].defaults[0].num_stratum_default_files; i++){
		fprintf(outfile, "\n%-30s %s", world[0].stratum_default_files[i],
			"basin_default_file");
	}
	fprintf(outfile, "\n%-30ld %s", world[0].num_base_stations,
		"num_base_stations");
	for (i=0; i<world[0].num_base_stations; i++){
		fprintf(outfile, "\n%-30s %s", world[0].base_station_files[i],
			"base_stations_file");
	}
	fprintf(outfile, "\n%-30ld %s", world[0].ID,
		"world_id");
	fprintf(outfile, "\n%-30ld %s", world[0].num_basin_files,
		"num_basins");
	/*--------------------------------------------------------------*/
	/*	output basins												*/
	/*--------------------------------------------------------------*/
	for (b=0; b < world[0].num_basin_files; ++ b ) {
		printf("output basin state");
		output_basin_state(world[0].basins[b], current_date, command_line, outfile);
	}
	fclose(outfile);
	return;
} /*end execute_state_output_event*/
