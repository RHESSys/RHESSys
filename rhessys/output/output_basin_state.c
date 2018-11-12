/*--------------------------------------------------------------*/
/* 																*/
/*					output_basin_state					*/
/*																*/
/*	output_basin_state - outputs state data			*/
/*																*/
/*	NAME														*/
/*	output_basin_state - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_basin_state(						*/
/*					struct	basin_object	*basin,				*/
/*					struct	date	current_date,				*/
/*					FILE	*outfile							*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current basin state variable to a make a new		*/
/*	world input file											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	May 20, 1997	C.Tague					*/
/*	- typo in counter for hillslope loop (changed from i to h ) */
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_basin_state(
						   struct	basin_object	*basin,
						   struct	date	current_date,
						   struct	command_line_object *command_line,
						   FILE	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_hillslope_state(
		struct	hillslope_object *,
		struct	date,
		struct	command_line_object *,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int h,i;
	/*--------------------------------------------------------------*/
	/*	output basin information									*/
	/*--------------------------------------------------------------*/

	printf("\n Beginning basin output state");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30ld %s",basin[0].ID, "basin_ID");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30.8f %s",basin[0].x, "x");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30.8f %s",basin[0].y, "y");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30.8f %s",basin[0].z, "z");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30ld %s",basin[0].defaults[0][0].ID, "basin_parm_ID");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30.8f %s",basin[0].latitude, "latitude");
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30ld %s",basin[0].num_base_stations, "n_basestations");
	for (i=0; i < basin[0].num_base_stations; i++){
		fprintf(outfile,"\n   ");
		fprintf(outfile,"%-30ld %s",basin[0].base_stations[i][0].ID,
			"p_base_station_ID");
	}
	fprintf(outfile,"\n   ");
	fprintf(outfile,"%-30ld %s",basin[0].num_hillslopes, "num_hillslopes");
	/*--------------------------------------------------------------*/
	/*	output hillslopes 											*/
	/*--------------------------------------------------------------*/
	for (h=0; h < basin[0].num_hillslopes; ++ h ) {
		output_hillslope_state(basin[0].hillslopes[h], current_date, command_line, outfile);
	}
    printf("\n Finishing basin output state\n");
	return;
} /*end output_basin_state*/
