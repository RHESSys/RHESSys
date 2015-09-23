/*--------------------------------------------------------------*/
/* 																*/
/*					output_patch_state					*/
/*																*/
/*	output_patch_state - outputs state data			*/
/*																*/
/*	NAME														*/
/*	output_patch_state - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_patch_state(						*/
/*					struct	patch_object	*patch,				*/
/*					struct	date	current_date,				*/
/*					FILE	*outfile							*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current patch state variable to a make a new		*/
/*	world input file											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_patch_state(
						   struct	patch_object	*patch,
						   struct	date	current_date,
						   struct	command_line_object *command_line,
						   FILE	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_canopy_strata_state(
		struct	canopy_strata_object *,
		struct	date,
		struct	command_line_object *,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int p,i;
	/*--------------------------------------------------------------*/
	/*	output patch information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30ld %s",patch[0].ID, "patch_ID");

	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].x, "x");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].y, "y");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].z, "z");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30ld %s",patch[0].soil_defaults[0][0].ID, "soil_parm_ID");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30ld %s",patch[0].landuse_defaults[0][0].ID, "landuse_parm_ID");
	fprintf(outfile,"\n          ");

	if (command_line[0].firespread_flag == 1) {
		fprintf(outfile,"%-30ld %s",patch[0].fire_defaults[0][0].ID, "fire_parm_ID");
		fprintf(outfile,"\n          ");
	}

	if (command_line[0].surface_energy_flag == 1) {
		fprintf(outfile,"%-30ld %s",patch[0].surface_energy_defaults[0][0].ID, "surface_energy_parm_ID");
		fprintf(outfile,"\n          ");
	}


	fprintf(outfile,"%-30.8f %s",patch[0].area, "area");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].slope/DtoR, "slope");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].lna, "lna");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].Ksat_vertical, "Ksat_vertical");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].original_m, "mpar");
	fprintf(outfile,"\n          ");
	if (command_line[0].stdev_flag == 1) {
		fprintf(outfile,"%-30.8f %s",patch[0].std, "std");
		fprintf(outfile,"\n          ");
	}
	fprintf(outfile,"%-30.8f %s",patch[0].rz_storage, "rz_storage");
	fprintf(outfile,"\n          ");	
	fprintf(outfile,"%-30.8f %s",patch[0].unsat_storage, "unsat_storage");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].sat_deficit, "sat_deficit");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].snowpack.water_equivalent_depth,
		"snowpack.water_equivalent_depth");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].snowpack.water_depth,
		"snowpack.water_depth");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].snowpack.T, "snowpack.T");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].snowpack.surface_age,
		"snowpack.surface_age");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f  %s",patch[0].snowpack.energy_deficit,
		"snowpack.energy_deficit");
	fprintf(outfile,"\n          ");


	if (command_line[0].snow_scale_flag == 1) {
		fprintf(outfile,"%-30.8f  %s",patch[0].snow_redist_scale,
			"snow_redist_scale");
		fprintf(outfile,"\n          ");
		}

	fprintf(outfile,"%-30.8f %s",patch[0].litter.cover_fraction,
		"litter.cover_fraction");
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30.8f %s",patch[0].litter.rain_stored,
		"litter.rain_stored");
	fprintf(outfile,"\n          ");


  if (command_line[0].vegspinup_flag > 0){
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_litter_cs->litr1c, "litter_cs.litr1c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_litter_ns->litr1n, "litter_ns.litr1n");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_litter_cs->litr2c, "litter_cs.litr2c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_litter_cs->litr3c, "litter_cs.litr3c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_litter_cs->litr4c, "litter_cs.litr4c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_cs->soil1c, "soil_cs.soil1c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_ns->sminn, "soil_ns.sminn");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_ns->nitrate, "soil_ns.nitrate");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_cs->soil2c, "soil_cs.soil2c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_cs->soil3c, "soil_cs.soil3c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].shadow_soil_cs->soil4c, "soil_cs.soil4c");
	  fprintf(outfile,"\n          ");
  }
  else{
    fprintf(outfile,"%-30.8f %s",patch[0].litter_cs.litr1c, "litter_cs.litr1c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].litter_ns.litr1n, "litter_ns.litr1n");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].litter_cs.litr2c, "litter_cs.litr2c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].litter_cs.litr3c, "litter_cs.litr3c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].litter_cs.litr4c, "litter_cs.litr4c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_cs.soil1c, "soil_cs.soil1c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_ns.sminn, "soil_ns.sminn");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_ns.nitrate, "soil_ns.nitrate");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_cs.soil2c, "soil_cs.soil2c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_cs.soil3c, "soil_cs.soil3c");
	  fprintf(outfile,"\n          ");
	  fprintf(outfile,"%-30.8f %s",patch[0].soil_cs.soil4c, "soil_cs.soil4c");
	  fprintf(outfile,"\n          ");
  }

	fprintf(outfile,"%-30ld %s",patch[0].num_base_stations, "n_basestations");
	for (i=0; i < patch[0].num_base_stations; i++){
		fprintf(outfile,"\n          ");
		fprintf(outfile,"%-30ld %s",patch[0].base_stations[i][0].ID,
			"base_station_ID");
	}
	fprintf(outfile,"\n          ");
	fprintf(outfile,"%-30ld %s",patch[0].num_canopy_strata, "num_canopy_strata");
	/*--------------------------------------------------------------*/
	/*	output canopy_stratas 											*/
	/*--------------------------------------------------------------*/
	for (p=0; p < patch[0].num_canopy_strata; ++ p ) {
    if (command_line[0].vegspinup_flag > 0){
		  output_canopy_strata_state(patch[0].shadow_strata[p],
			current_date, command_line, outfile);
    } 
    else {
		output_canopy_strata_state(patch[0].canopy_strata[p],
			current_date, command_line, outfile);
    }
  }
	return;
} /*end output_patch_state*/
