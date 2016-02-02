/*--------------------------------------------------------------*/
/* 																*/
/*					add_csv_headers					*/
/*																*/
/*	add_csv_headers - 
/*																*/
/*	NAME														*/
/*	add_csv_headers 
/*																*/
/*	SYNOPSIS													*/
/*	void add_csv_headers(struct world output_file_object *,				*/
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


void add_csv_headers(struct world_output_file_object *world_output_files, 
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
	fprintf(outfile,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"pot_surface_infil",
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
		"detention_store",
		"%sat_area",
		"litter_store",
		"%snow_cover",
		"snow_subl"
		);

	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].monthly;
	check = fprintf(outfile,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
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
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
		"year",
		"basinID",
		"streamflow",
		"streamflow_NO3",
		"denitrif",
		"DOC",
		"DON",
		"et",
		"psn","lai","nitrif",
		"mineralized", "uptake", "num_thresh");
	}

	/*--------------------------------------------------------------*/
	/*	Hillslope file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].h != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].hillslope[0].daily;
	fprintf(outfile,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
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
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
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
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
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
	fprintf(outfile,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n " ,
		"day",
		"month",
		"year",
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

	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].zone[0].monthly;
	check = fprintf(outfile,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"precip",
		"K_direct",
		"K_diffuse",
		"tmax", "tmin");

	}

	/*--------------------------------------------------------------*/
	/*	Patch file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].p != NULL) {
		/*--------------------------------------------------------------*/
		/*	Check to see if SCM mode is called - requires column heads	*/
		/*--------------------------------------------------------------*/
		if (command_line[0].scm_flag != 1){

			/*--------------------------------------------------------------*/
			/*	Daily - Not SCM mode 							*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].patch[0].daily;
			check = fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
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
				"unsat_stor",
				"unsat_drain",
				"cap",
				"return",
				"evap",
				"snow",
				"trans ",
				"Qin",
				"Qout",
				"psn",
				"root_zone.S",
				"litter.rain_stor",
				"litter.S","area");
				
			/*--------------------------------------------------------------*/
			/*	Hourly - Not SCM mode 							*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].patch[0].hourly;
			check = fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
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
				"unsat_stor",
				"unsat_drain",
				"cap",
				"return",
				"evap",
				"snow",
				"trans ",
				"Qin",
				"Qout",
				"psn",
				"root_zone.S",
				"litter.rain_stor",
				"litter.S","area");
		} else {
			/*--------------------------------------------------------------*/
			/*	Daily 		- SCM MODE					*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].patch[0].daily;
			check = fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
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
				"unsat_stor",
				"unsat_drain",
				"cap",
				"return",
				"evap",
				"snow",
				"trans ",
				"Qin",
				"Qout",
				"psn",
				"root_zone.S",
				"litter.rain_stor",
				"litter.S","area",
				"surface_Qin",  // Add for SCM				
				"subsurface_Qin",  // Add for SCM
				"surface_Qout",  // Add for SCM				
				"subsurface_Qin",  // Add for SCM
				"recharge",  // Add for SCM
				"scm_ave_height");  // Add for SCM;
				
			/*--------------------------------------------------------------*/
			/*	Hourly 		- SCM MODE					*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].patch[0].hourly;
			check = fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
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
				"unsat_stor",
				"unsat_drain",
				"cap",
				"return",
				"evap",
				"snow",
				"trans ",
				"Qin",
				"Qout",
				"psn",
				"root_zone.S",
				"litter.rain_stor",
				"litter.S","area",
				"surface_Qin",  // Add for SCM				
				"subsurface_Qin",  // Add for SCM
				"surface_Qout",  // Add for SCM				
				"subsurface_Qin",  // Add for SCM
				"recharge",  // Add for SCM
				"scm_ave_height");  // Add for SCM;
		}
				
	/*--------------------------------------------------------------*/
	/*	Monthly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].monthly;
	check = fprintf(outfile,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n", 
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
		"DON","lai","nitrif","mineralized","uptake","theta","snow","area");
	/*--------------------------------------------------------------*/
	/*	Yearly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].yearly;
	fprintf(outfile, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" ,
			"year",
			"basinID",
			"hillID",
			"zoneID",
			"patchID",
			"num_threshold_sat_def",
			"leach",
			"denitrif",
			"DOC_loss",
			"DON_loss",
			"psn",
			"et","lai","nitrif","mineralized","uptake","Theta","area"
			);
	}

	/*--------------------------------------------------------------*/
	/*	Stratum file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].c != NULL) {
		/*--------------------------------------------------------------*/
		/*	Check to see if SCM mode is called - requires column heads	*/
		/*--------------------------------------------------------------*/
		if (command_line[0].scm_flag != 1){

			/*--------------------------------------------------------------*/
			/*	Daily - Not SCM mode 							*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].canopy_stratum[0].daily;
			fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n" ,
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
				"rootzone.S"); 
		} else {
		
			/*--------------------------------------------------------------*/
			/*	Daily -  SCM mode 							*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].canopy_stratum[0].daily;
			fprintf(outfile,
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n" ,
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
				"algae_chlA",
				"algae_totalN",
				"algae_totalC"); 
		
		}
				
			/*--------------------------------------------------------------*/
			/*	Monthly							*/
			/*--------------------------------------------------------------*/
			outfile = world_output_files[0].canopy_stratum[0].monthly;
			fprintf(outfile,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n", 
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
	fprintf(outfile,"%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
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
} /*end add_csv_headers*/
