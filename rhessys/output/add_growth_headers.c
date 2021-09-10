/*--------------------------------------------------------------*/
/* 																*/
/*					add_growth_headers					*/
/*																*/
/*	add_growth_headers - 												    	*/
/*	NAME														*/
/*	add_growth_headers    													*/
/*	SYNOPSIS													*/
/*	void add_growth_headers(struct world output_file_object *,				*/
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


void add_growth_headers(struct world_output_file_object *world_output_files,
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
	/*	Hourly							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].hourly;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"hour",
		"day",
		"month",
		"year",
		"basinID",
		//"lai",
		//"gpsn",
		//"plant_resp",
		//"soil_resp",
		//"nitrate",
		//"sminn",
		//"surfaceN",
		//"plantc",
		//"plantn",
		//"npool",
		//"litrc",
		//"litrn",
		//"soilc",
		//"soiln",
		"gwNO3",
		"gwNH4",
		"gwDON",
		"gwDOC",
		"streamflow_NO3",
		"streamflow_NH4",
		"streamflow_DON",
		"streamflow_DOC",
		"gwNO3out",
		"gwNH4out",
		"gwDONout",
		"gwDOCout",
		//"denitrif",
		//"nitrif",
		//"DOC",
		//"DON",
		//"root_depth",
		//"nfix",
		//"nuptake",
		//"grazingC",
		"StreamNO3_from_surface",
		"StreamNO3_from_sub");
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].daily;

	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"lai",
		"gpsn",
		"actual_C_growth",
		"actual_N_uptake",
		"storage_transfer_prop",
		"carbohydrate_transfer_c",
		"carbohydrate_transfer_n",
		"plant_resp",
		"leaf_resp",
		"soil_resp",
		"nitrate",
		"sminn",
		"fpi",
		"mineralized",
		"decomp_t_scalar",
		"decomp_w_scalar",
		"surfaceN",
		"plantc",
		"plantn",
		"cpool",
		"npool",
		"litrc",
		"litrn",
		"soilc",
		"soiln",
		"soiln_noslow",
		"gwNO3",
		"gwNH4",
		"gwDON",
		"gwDOC",
		"streamflow_NO3",
		"streamflow_NH4",
		"streamflow_DON",
		"streamflow_DOC",
		"gwNO3out",
		"gwNH4out",
		"gwDONout",
		"gwDOCout",
		"denitrif",
		"nitrif",
		"DOC",
		"DON",
		"root_depth",
		"nfix",
		"nuptake",
		"grazingC",
		"StreamNO3_from_surface",
		"StreamNO3_from_sub", "N_dep","fertilizer_store",
		"understory_leafc",
		"understory_stemc",
		"understory_biomassc",
		"understory_height",
		"overstory_leafc",
		"overstory_stemc",
		"overstory_biomassc",
		"overstory_height",
		"canopy_NO3_stored",
		"litter_NO3_stored",
		"NO3_throughfall",
		"Nvolatilized_snk");
	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].basin[0].yearly;
	fprintf(outfile, "%s %s %s %s %s %s %s %s %s %s \n",
		"year",
		"basinID",
		"gpsn",
		"plantresp",
		"newC",
		"soilhr",
		"strN",
		"denitrif","root_depth","mortf");

	}

	/*--------------------------------------------------------------*/
	/*	Hillslope file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].h != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].hillslope[0].daily;

	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"lai",
		"gpsn",
		"plant_resp",
		"soil_resp",
		"nitrate",
		"sminn",
		"surfaceN",
		"plantc",
		"plantn",
		"cpool",
		"npool",
		"litrc",
		"litrn",
		"soilc",
		"soiln",
		"gwNO3",
		"gwNH4",
		"gwDON",
		"gwDOC",
		"streamflow_NO3",
		"streamflow_NH4",
		"streamflow_DON",
		"streamflow_DOC",
		"gwNO3out",
		"gwNH4out",
		"gwDONout",
		"gwDOCout",
		"denitrif",
		"nitrif",
		"DOC",
		"DON",
		"root_depth",
		"nfix",
		"nuptake",
		"grazingC",
		"StreamNO3_from_surface",
		"StreamNO3_from_sub");
	}

	/*--------------------------------------------------------------*/
	/*	Zone file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].z != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].zone[0].daily;
	fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n ",
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
	}

	/*--------------------------------------------------------------*/
	/*	Patch file headers					*/
	/*--------------------------------------------------------------*/
	if (command_line[0].p != NULL) {
	/*--------------------------------------------------------------*/
	/*	Daily 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].daily;
	check = fprintf(outfile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"familyID",
		"lai",
		"plantc",
		"plantn",
		"net_psn",
		"plant_resp",
		"soil_resp",
		"litr1c",
		"litr2c",
		"litr3c",
		"litr4c",
		"litr1n",
		"litr2n",
		"litr3n",
		"litr4n",
		"litrc_sum",
		"lit.rain_cap",
		"soil1c",
		"soil2c",
		"soil3c",
		"soil4c",
		"soil1n",
		"soil2n",
		"soil3n",
		"soil4n",
		"soilDON",
		"soilDOC",
		"denitrif",
		"netleach",
		"DON_loss",
		"DOC_loss",
		"soilNO3",
		"soilNH4",
		"streamNO3",
		"streamNH4",
		"streamDON",
		"streamDOC",
		"surfaceNO3",
		"surfaceNH4",
		"surfaceDOC",
		"surfaceDON",
		"height",
		"nuptake",
		"root_depth",
		"nfix",
		"grazingC",
		"area");
	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].patch[0].yearly;
	fprintf(outfile, "%s %s %s %s %s %s %s %s %s %s %s %s\n",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"litter_c",
		"soil_c",
		"litter_n",
		"soil_n",
		"nitrate",
		"sminn","root_depth");
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
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s  %s\n" ,
		"day",
		"month",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"proj_lai",
		"leafc",
		"leafn",
		"cpool",
		"npool",
		"retransn",
		"dead_leafc",
		"frootc",
		"frootn",
		"live_stemc",
		"live_stemn",
		"leafc_store",
		"leafn_store",
		"dead_stemc",
		"dead_stemn",
		"live_crootc",
		"live_crootn",
		"dead_crootc",
		"dead_crootn",
		"cwdc",
		"mresp",
		"gresp",
		"psn_to_cpool","age","root_depth","gwseasonday","lfseasonday","gsi", "nlimit",
		"fleaf","froot","fwood","Nuptake","potential_Nuptake","smin2pl","retrans2pl","mort_fract",
		"assim_sunlit", "assim_shade", "trans_sunlit", "trans_shade", "leafc_age1", "leafc_age2", "proj_lai_sunlit","proj_lai_shade",
		"actual_C_growth", "actual_N_uptake",
		"storage_transfer_prop",
		"carbohydrate_transfer_c",
		"carbohydrate_transfer_n");

  /*--------------------------------------------------------------*/
	/* Shadow	Daily 			                                   				*/
	/*--------------------------------------------------------------*/
	if (command_line[0].vegspinup_flag > 0){
    outfile = world_output_files[0].shadow_strata[0].daily;
	  fprintf(outfile,
	  	"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" ,
	  	"day",
	  	"month",
	  	"year",
	  	"basinID",
		  "hillID  ",
		  "zoneID  ",
		  "patchID  ",
		  "stratumID  ",
		  "proj_lai  ",
		  "leafc  ",
		  "leafn  ",
		  "cpool  ",
		  "npool  ",
		  "dead_leafc  ",
		  "frootc  ",
		  "frootn  ",
		  "live_stemc  ",
		  "live_stemn  ",
		  "leafc_store  ",
		  "leafn_store  ",
		  "dead_stemc  ",
		  "dead_stemn  ",
		  "live_crootc  ",
		  "live_crootn  ",
		  "dead_crootc  ",
		  "dead_crootn  ",
		  "cwdc  ",
		  "mresp  ",
		  "gresp  ",
		  "psn_to_cpool","age","root_depth","gwseasonday","lfseasonday","gsi", "nlimit",
		  "fleaf","froot","fwood","Nuptake","smin2pl","retrans2pl","mort_fract");
  }

	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].canopy_stratum[0].yearly;
	fprintf(outfile, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"proj_lai",
		"leafc",
		"leafn",
		"frootc",
		"frootn",
		"stemc",
		"stemn",
		"cwdc",
		"cwdn",
		"psn","cpool", "mortfract","height","rootdepth");
	}


    if (command_line[0].f !=NULL) {
	/*--------------------------------------------------------------*/
	/*	Yearly 							*/
	/*--------------------------------------------------------------*/
	outfile = world_output_files[0].fire[0].yearly;
	fprintf(outfile, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
		"year",
		"basinID",
		"hillID",
		"zoneID",
		"patchID",
		"stratumID",
		"proj_lai",
		"leafc",
		"leafn",
		"frootc",
		"frootn",
		"stemc",
		"stemn",
		"cwdc",
		"cwdn",
		"psn","cpool", "mortfract","height","rootdepth");
	}




	return;
} /*end add_growth_headers*/
