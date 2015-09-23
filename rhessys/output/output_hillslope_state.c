/*--------------------------------------------------------------*/
/* 																*/
/*					output_hillslope_state					*/
/*																*/
/*	output_hillslope_state - outputs state data			*/
/*																*/
/*	NAME														*/
/*	output_hillslope_state - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_hillslope_state(						*/
/*					struct	hillslope_object	*hillslope,				*/
/*					struct	date	current_date,				*/
/*					FILE	*outfile							*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current hillslope state variable to a make a new		*/
/*	world input file											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_hillslope_state(
							   struct	hillslope_object	*hillslope,
							   struct	date	current_date,
							   struct	command_line_object *command_line,
							   FILE	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_zone_state(
		struct	zone_object *,
		struct	date,
		struct	command_line_object *,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int h,i;
	/*--------------------------------------------------------------*/
	/*	output hillslope information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30d %s",hillslope[0].ID, "hillslope_ID");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30.8lf %s",hillslope[0].x, "x");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30.8lf %s",hillslope[0].y, "y");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30.8lf %s",hillslope[0].z, "z");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30d %s",hillslope[0].defaults[0][0].ID, "hill_parm_ID");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30.8lf %s",hillslope[0].gw.storage, "gw.storage");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30.8lf %s",hillslope[0].gw.NO3, "gw.NO3");
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30d %s",hillslope[0].num_base_stations, "n_basestations");
	for (i=0; i < hillslope[0].num_base_stations; i++){
		fprintf(outfile,"\n     ");
		fprintf(outfile,"%-30ld %s",hillslope[0].base_stations[i][0].ID,
			"p_base_station_ID");
	}
	fprintf(outfile,"\n     ");
	fprintf(outfile,"%-30ld %s",hillslope[0].num_zones, "num_zones");
	/*--------------------------------------------------------------*/
	/*	output zones 											*/
	/*--------------------------------------------------------------*/
	for (h=0; h < hillslope[0].num_zones; ++ h ) {
		output_zone_state(hillslope[0].zones[h], current_date, command_line, outfile);
	}
	return;
} /*end output_hillslope_state*/
