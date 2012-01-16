/*--------------------------------------------------------------*/
/* 																*/
/*					output_template_structure					*/
/*																*/
/*	output_template_structure - prints current template structure */
/*																*/
/*	NAME														*/
/*	output_template_structure  	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_template_structure(						*/
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

void	output_template_structure()
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int b,i;
	FILE	*outfile;
	/*--------------------------------------------------------------*/
	/*	open output file											*/
	/*--------------------------------------------------------------*/
	if ( ( outfile = fopen("template_structure.txt", "w")) == NULL ){
		fprintf(stderr,"FATAL ERROR: in output_template_structure.\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	output world information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile, "\n %s", "current_year");
	fprintf(outfile, "\n %s", "current_month");
	fprintf(outfile, "\n %s", "current_day");
	fprintf(outfile, "\n %s", "current_hour");
	fprintf(outfile, "\n %s", "end_year");
	fprintf(outfile, "\n %s", "end_month");
	fprintf(outfile, "\n %s", "end_day");
	fprintf(outfile, "\n %s", "end_hour");
	fprintf(outfile, "\n %s", "num_basin_default_files");
	fprintf(outfile, "\n %s", "basin_default_file");
	fprintf(outfile, "\n %s", "num_hillslope_default_files");
	fprintf(outfile, "\n %s", "hillslope_default_file");
	fprintf(outfile, "\n %s", "num_zone_default_files");
	fprintf(outfile, "\n %s", "zone_default_file");
	fprintf(outfile, "\n %s", "num_soil_default_files");
	fprintf(outfile, "\n %s", "soil_default_file");
	fprintf(outfile, "\n %s", "num_landuse_default_files");
	fprintf(outfile, "\n %s", "landuse_default_file");
	fprintf(outfile, "\n %s", "num_stratum_default_files");
	fprintf(outfile, "\n %s", "basin_default_file");
	fprintf(outfile, "\n %s", "num_base_stations");
	fprintf(outfile, "\n %s", "base_stations_file");
	fprintf(outfile,"\n");
	fprintf(outfile, "\n %s", "world_id");
	fprintf(outfile, "\n %s", "num_basins");
	fprintf(outfile,"\n");
	/*--------------------------------------------------------------*/
	/*	output basins												*/
	/*--------------------------------------------------------------*/

	fprintf(outfile,"\n %s","basin ID");
	fprintf(outfile,"\n %s","p_x");
	fprintf(outfile,"\n %s","p_y");
	fprintf(outfile,"\n %s","p_z");
	fprintf(outfile,"\n %s","p_default_ID");
	fprintf(outfile,"\n %s","p_latitude");
	fprintf(outfile,"\n %s","n_basestations");
	fprintf(outfile,"\n %s","p_base_station_ID");
	fprintf(outfile,"\n %s","num_hillslopes");
	fprintf(outfile,"\n");
	/*--------------------------------------------------------------*/
	/*	output hillslopes 											*/
	/*--------------------------------------------------------------*/

	fprintf(outfile,"\n %s","hillslope ID");
	fprintf(outfile,"\n %s","p_x");
	fprintf(outfile,"\n %s","p_y");
	fprintf(outfile,"\n %s","p_z");
	fprintf(outfile,"\n %s","p_default_ID");
	fprintf(outfile,"\n %s","p_base_flow");
	fprintf(outfile,"\n %s","n_basestations");
	fprintf(outfile,"\n %s","p_base_station_ID");
	fprintf(outfile,"\n %s","num_zones");
	fprintf(outfile,"\n");
	/*--------------------------------------------------------------*/
	/*	output zones 											*/
	/*--------------------------------------------------------------*/

	fprintf(outfile,"\n %s","zone ID");
	fprintf(outfile,"\n %s","p_x");
	fprintf(outfile,"\n %s","p_y");
	fprintf(outfile,"\n %s","p_z");
	fprintf(outfile,"\n %s","p_default_ID");
	fprintf(outfile,"\n %s","p_area");
	fprintf(outfile,"\n %s","p_slope");
	fprintf(outfile,"\n %s","p_aspect");
	fprintf(outfile,"\n %s","p_precip_lapse_rate");
	fprintf(outfile,"\n %s","p_e_horizon");
	fprintf(outfile,"\n %s","p_w_horizon");
	fprintf(outfile,"\n %s","n_basestations");
	fprintf(outfile,"\n %s","p_base_station_ID");
	fprintf(outfile,"\n %s","num_patches");
	fprintf(outfile,"\n");

	/*--------------------------------------------------------------*/
	/*	output patch information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n %s","patch ID");
	fprintf(outfile,"\n %s","x");
	fprintf(outfile,"\n %s","y");
	fprintf(outfile,"\n %s","z");
	fprintf(outfile,"\n %s","soil_default_ID");
	fprintf(outfile,"\n %s","landuse_default_ID");
	fprintf(outfile,"\n %s","area");
	fprintf(outfile,"\n %s","slope");
	fprintf(outfile,"\n %s","lna");
	fprintf(outfile,"\n %s","Ksat_vertical");
	fprintf(outfile,"\n %s","m_par");
	fprintf(outfile,"\n %s","unsat_storage");
	fprintf(outfile,"\n %s","sat_deficit");
	fprintf(outfile,"\n %s","snowpack.water_equivalent_depth");
	fprintf(outfile,"\n %s","snowpack_water_depth");
	fprintf(outfile,"\n %s","snowpack_T");
	fprintf(outfile,"\n %s","snowpack_surface_age");
	fprintf(outfile,"\n %s","snowpack_energy_deficit");
	fprintf(outfile,"\n %s","litter.rain_stored");
	fprintf(outfile,"\n %s","litter_cs.litr1c");
	fprintf(outfile,"\n %s","litter_ns.litr1n");
	fprintf(outfile,"\n %s","litter_cs.litr2c");
	fprintf(outfile,"\n %s","litter_cs.litr3c");
	fprintf(outfile,"\n %s","litter_cs.litr4c");
	fprintf(outfile,"\n %s","soil_cs.soil1c");
	fprintf(outfile,"\n %s","soil_ns.sminn");
	fprintf(outfile,"\n %s","soil_ns.nitrate");
	fprintf(outfile,"\n %s","soil_cs.soil2c");
	fprintf(outfile,"\n %s","soil_cs.soil3c");
	fprintf(outfile,"\n %s","soil_cs.soil4c");
	fprintf(outfile,"\n %s","n_basestations");
	fprintf(outfile,"\n %s","base_station_ID");
	fprintf(outfile,"\n %s","num_canopy_strata");
	/*--------------------------------------------------------------*/
	/*	output canopy_strata information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n");
	fprintf(outfile,"\n %s", "canopy_strata ID");
	fprintf(outfile,"\n %s", "default_ID");
	fprintf(outfile,"\n %s", "cover_fraction");
	fprintf(outfile,"\n %s", "gap_fraction");
	fprintf(outfile,"\n %s", "root_depth");
	fprintf(outfile,"\n %s", "snow_stored");
	fprintf(outfile,"\n %s", "rain_stored");
	fprintf(outfile,"\n %s", "cs_cpool");
	fprintf(outfile,"\n %s", "cs_leafc");
	fprintf(outfile,"\n %s", "cs_dead_leafc");
	fprintf(outfile,"\n %s", "cs_leafc_store");
	fprintf(outfile,"\n %s", "cs_leafc_transfer");
	fprintf(outfile,"\n %s", "cs_live_stemc");
	fprintf(outfile,"\n %s", "cs_livestemc_store");
	fprintf(outfile,"\n %s", "cs_livestemc_transfer");
	fprintf(outfile,"\n %s", "cs_dead_stemc");
	fprintf(outfile,"\n %s", "cs_deadstemc_store");
	fprintf(outfile,"\n %s", "cs_deadstemc_transfer");
	fprintf(outfile,"\n %s", "cs_live_crootc");
	fprintf(outfile,"\n %s", "cs_livecrootc_store");
	fprintf(outfile,"\n %s", "cs_livecrootc_transfer");
	fprintf(outfile,"\n %s", "cs_dead_crootc");
	fprintf(outfile,"\n %s", "cs_deadcrootc_store");
	fprintf(outfile,"\n %s", "cs_deadcrootc_transfer");
	fprintf(outfile,"\n %s", "cs_frootc"); 
	fprintf(outfile,"\n %s", "cs_frootc_store");
	fprintf(outfile,"\n %s", "cs_frootc_transfer");
	fprintf(outfile,"\n %s", "cs_cwdc");
	fprintf(outfile,"\n %s", "epv.prev_leafcalloc");
	fprintf(outfile,"\n %s", "ns_npool");
	fprintf(outfile,"\n %s", "ns_leafn");
	fprintf(outfile,"\n %s", "ns_dead_leafn");
	fprintf(outfile,"\n %s", "ns_leafn_store");
	fprintf(outfile,"\n %s", "ns_leafn_transfer");
	fprintf(outfile,"\n %s", "ns_live_stemn");
	fprintf(outfile,"\n %s", "ns_livestemn_store");
	fprintf(outfile,"\n %s", "ns_livestemn_transfer");
	fprintf(outfile,"\n %s", "ns_dead_stemn");
	fprintf(outfile,"\n %s", "ns_deadstemn_store");
	fprintf(outfile,"\n %s", "ns_deadstemn_transfer");
	fprintf(outfile,"\n %s", "ns_live_crootn");
	fprintf(outfile,"\n %s", "ns_livecrootn_store");
	fprintf(outfile,"\n %s", "ns_livecrootn_transfer");
	fprintf(outfile,"\n %s", "ns_dead_crootn");
	fprintf(outfile,"\n %s", "ns_deadcrootn_store");
	fprintf(outfile,"\n %s", "ns_deadcrootn_transfer");
	fprintf(outfile,"\n %s", "ns_frootn");
	fprintf(outfile,"\n %s", "ns_frootn_store");
	fprintf(outfile,"\n %s", "ns_frootn_transfer");
	fprintf(outfile,"\n %s", "ns_cwdn");
	fprintf(outfile,"\n %s", "epv_wstress_days");
	fprintf(outfile,"\n %s", "epv_max_fparabs");
	fprintf(outfile,"\n %s", "epv_min_vwc");
	fprintf(outfile,"\n %s", "n_basestations");
	fprintf(outfile,"\n %s", "base_station_ID");

	fclose(outfile);
	return;
} /*end output_template_structure*/
