/*--------------------------------------------------------------*/
/* 																*/
/*					output_zone_state					*/
/*																*/
/*	output_zone_state - outputs state data			*/
/*																*/
/*	NAME														*/
/*	output_zone_state - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_zone_state(						*/
/*					struct	zone_object	*zone,				*/
/*					struct	date	current_date,				*/
/*					FILE	*outfile							*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current zone state variable to a make a new		*/
/*	world input file											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/* June 1, C.Tague			*/
/*  aspect and slope must be converted 		*/
/* 	back to degrees to be consistent	*/ 
/*	with input in construct_zone		*/
/*  															*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_zone_state(
						  struct	zone_object	*zone,
						  struct	date	current_date,
						  struct	command_line_object *command_line,
						  FILE	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_patch_state(
		struct	patch_object *,
		struct	date,
		struct	command_line_object *,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int z,i;
	/*--------------------------------------------------------------*/
	/*	output zone information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30ld %s",zone[0].ID, "zone_ID");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].x, "x");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].y, "y");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].z, "z");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30ld %s",zone[0].defaults[0][0].ID, "zone_parm_ID");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].area, "area");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].slope / DtoR, "slope");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].aspect / DtoR, "aspect");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].precip_lapse_rate, "precip_lapse_rate");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].e_horizon, "e_horizon");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30.8f %s",zone[0].w_horizon, "w_horizon");
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30ld %s",zone[0].num_base_stations, "n_basestations");
	for (i=0; i < zone[0].num_base_stations; i++){
		fprintf(outfile,"\n       ");
		fprintf(outfile,"%-30ld %s",zone[0].base_stations[i][0].ID,
			"zone_base_station_ID");
	}
	fprintf(outfile,"\n       ");
	fprintf(outfile,"%-30ld %s",zone[0].num_patches, "num_patches");
	/*--------------------------------------------------------------*/
	/*	output patchs 											*/
	/*--------------------------------------------------------------*/
	for (z=0; z < zone[0].num_patches; ++ z ) {
		output_patch_state(zone[0].patches[z], current_date, command_line, outfile);
	}
	return;
} /*end output_zone_state*/
