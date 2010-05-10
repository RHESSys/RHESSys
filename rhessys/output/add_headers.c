/*--------------------------------------------------------------*/
/* 																*/
/*					add_headers					*/
/*																*/
/*	add_headers - 
/*																*/
/*	NAME														*/
/*	add_headers 
/*																*/
/*	SYNOPSIS													*/
/*	void add_headers(struct world output_file_object *,				*/
/*			struct command_line_object *)					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Adds headers for yearly, monthly, daily and	*/
/*	hourly basin output 					*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "rhessys.h"


void add_headers(struct world_output_file_object *world_output_files, 
			struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	FILE *outfile;
	int check;
	/*--------------------------------------------------------------*/
	/*	Basin file headers					*/
	/*--------------------------------------------------------------*/

	if (command_line[0].b != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].daily;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"pot_surface_infil",
		"snow_thr",
		"sat_def_z",
		"sat_def",
		"rz_storage",
		"unsat_stor",
		"rz_drainage",
		"unsat_drain",
		"cap",
		"evap",
		"snow",
		"trans",
		"baseflow",
		"return",
		"streamflow",
		"psn",
		"lai",
		"gw.Qout",
		"gw.Nout",
		"gw.storage",
		"gw.NO3",
		"detention_store",
		"%sat_area",
		"litter_store",
		"canopy_store",
		"%snow_cover",
		"snow_subl",
		"trans_var",
		"acc_trans",
		"acctransv_var",
		"pet", "dC13"
		);

	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].monthly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
		"month",
		"year",
		"basinID",
		"streamflow",
		"streamflow_NO3",
		"denitrif",
		"DOC",
		"DON",
		"et",
		"psn",
		"lai",
		"nitrif",
		"mineralized",
		"uptake");
	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].yearly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
		"year",
		"basinID",
		"streamflow",
		"streamflow_NO3",
		"denitrif",
		"DOC",
		"DON",
		"et",
		"psn","lai","nitrif",
		"mineralized", "uptake", "num_thresh","pet");
	}

	/*--------------------------------------------------------------*/
	/*	Hillslope file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].h != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].hillslope[0].daily;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"rain_thr",
		"snow_thr",
		"sat_def_z",
		"sat_def",
		"unsat_stor",
		"unsat_drain",
		"cap",
		"evap",
		"snow",
		"trans",
		"baseflow",
		"return",
		"streamflow",
		"psn",
		"lai",
		"gw.Qout",
		"gw.Nout",
		"gw.storage",
		"gw.NO3",
		"area"
		);

	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].hillslope[0].monthly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
		"month",
		"year",
		"basinID",
		"hillslopeID",
		"streamflow",
		"streamflow_NO3",
		"snowpack",
		"denitrif",
		"DOC",
		"DON",
		"et",
		"psn",
		"lai",
		"nitrif",
		"mineralized",
		"uptake","area");
	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].hillslope[0].yearly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
		"year",
		"basinID",
		"hillslopeID",
		"streamflow",
		"streamflow_NO3",
		"denitrif",
		"DOC",
		"DON",
		"et",
		"psn","lai","nitrif",
		"mineralized", "uptake","area");
	}
	/*--------------------------------------------------------------*/
	/*	Zone file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].z != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].zone[0].daily;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n " ,
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"ID",
		"rain",
		"snow",
		"tmax",
		"tmin",
		"vpd",
		"Kdown_direct",
		"Kdown_diffuse",
		"PAR_direct",
		"PAR_diffuse","relH");

	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].zone[0].monthly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s\n" ,
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"precip",
		"K_direct",
		"K_diffuse",
		"tmax", "tmin");

	/*--------------------------------------------------------------*/
	/*	Hourly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].zone[0].hourly;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n " ,
		"day",
		"month",
		"year",
		"hour",
		"basinID",
		"hillID",
		"ID",
		"rain",
		"snow",
		"tday",
		"tavg",
		"vpd",
		"Kdown_direct",
		"Kdown_diffuse",
		"PAR_direct",
		"PAR_diffuse");



	}

	/*--------------------------------------------------------------*/
	/*	Patch file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].p != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].daily;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"rain_thr",
		"detention_store",
		"sat_def_z",
		"sat_def",
		"rz_storage",
		"potential_rz_store",
		"rz_field_capacity",
		"rz_wilting_point",
		"unsat_stor",
		"rz_drainage",
		"unsat_drain",
		"sublimation",
		"return",
		"evap",
		"evap_surface",
		"soil_evap",
		"snow",
		"trans_sat",
		"trans_unsat",
		"Qin",
		"Qout",
		"psn",
		"root_zone.S",
		"root.depth",
		"litter.rain_stor",
		"litter.S","area","pet","lai");
	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].monthly;
	check = fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s \n", 
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"leach",
		"denitrif",
		"soil_moist_deficit",
		"et",
		"psn",
		"DOC",
		"DON","lai","nitrif","mineralized","uptake","theta","snow","area","nitrate","sminn");
	/*--------------------------------------------------------------*/
	/*	Yearly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].yearly;
	fprintf(outfile, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
			"year",
			"basinID",
			"hillID",
			"zoneID",
			"patchID",
			"num_threshold_sat_def",
			"peaksweday",
			"meltday",
			"leach",
			"denitrif",
			"DOC_loss",
			"DON_loss",
			"psn", "trans",
			"et","lai","nitrif","mineralized","uptake","Theta","sd","pkswe", "pktrans", "pkpet", "rec.wyd","rec.pet.wyd", 
			"ndays_sat", "ndays_sat70", "midsm_wyd",
			"area","pet"
			);
	}

	/*--------------------------------------------------------------*/
	/*	Stratum file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].c != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].canopy_stratum[0].daily;
	fprintf(outfile,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s \n" ,
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"lai",
		"evap",
		"APAR_direct",
		"APAR_diffuse",
		"sublim",
		"trans",
		"ga",
		"gsurf",
		"gs",
		"psi",
		"leaf_day_mr",
		"psn_to_cpool",
		"rain_stored",
		"snow_stored",
		"rootzone.S",
		"m_APAR","m_tavg","m_LWP","m_CO2","m_tmin","m_vpd","dC13");
	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].canopy_stratum[0].monthly;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s \n", 
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"lai",
		"psn",
		"lwp");
	/*--------------------------------------------------------------*/
	/*	Yearly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].canopy_stratum[0].yearly;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s\n",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"psn",
		"lwp","root_depth");
	}
	return;
} /*end add_headers*/
