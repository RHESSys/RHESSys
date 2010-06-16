/*--------------------------------------------------------------*/
/*                                                              */
/*		create_flowpaths										*/
/*                                                              */
/*  NAME                                                        */
/*		 create_flowpaths										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 create_flowpaths( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*		-l    roads to lowest flna interval 					*/
/*		-h  	roads to highest flna interval					*/
/*		-s print drainage statistics							*/
/*		-r 	road flag for drainage statistics					*/
/*		-stream	stream connectivity is assumed						*/
/*			1	random slope value								*/
/*			2	internal slope value							*/
/*			0	no connectivity (default)						*/
/*		-scaledem scale dem values by this amount                     */
/*		-scaletrans	scale streamside transmissivity						*/
/*		-bi	basin ID				*/
/*		-sw	use a sewer image to route water from roads     	*/
/*		-pst	print stream table	*/
/*		-pre	input image file name prefix					*/
/*		-w 	road width (default is 5m)							*/
/*		-a arcview ascii data files (default is GRASS ascii)	*/
/*		-slp slope flag - change use of slope		            */
/*			in computation of gamma			                    */
/*			0  standard				                            */
/*			1  internal slpe of patch		                    */
/*			2 max slope of patch		                    	*/
/*		-o output file name (default -pre opt + _flow_table.dat	*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <grass/gis.h>

#include "grassio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"




main(int argc, char *argv[])  
{
/* local variable declarations */
    int		i, num_stream, num_patches;   
	FILE	*out1, *out2;    
	int		basinid, tmp, maxr, maxc;
	double	cell, width;
	int		pst_flag, f_flag, fl_flag, fh_flag, vflag;
	int		s_flag, r_flag, slp_flag, sc_flag;
	int		st_flag, sewer_flag;
	int		d_flag;
	double	scale_trans, scale_dem;
	char    input_prefix[MAXS];
	char    output_suffix[MAXS];


/* filenames for each image and file */

	char	name[MAXS], name2[MAXS];
	char*	rnflna;
	char* 	rnslope;
	char*	fntemplate;
	char	rnbasin[MAXS];
	char	rnhill[MAXS];
	char	rnzone[MAXS];
	char	rnpatch[MAXS];
	char*	rndem;
	char*	rnroads;
	char*	rnstream;
	char*	rnsewers;

/* set pointers for images */

    double	    *dem;
    int          *patch;	
    float        *slope;	
    int          *hill;	
    int          *zone;	
    int			 *stream;
    int			 *roads;
    int			 *sewers;
	double		 *flna;
	float		 *ehr;
	float		 *whr;
	struct		flow_struct	*flow_table;

	d_flag 	 = 0;		/* debuf flag					 */
    vflag    = 0;		/* verbose flag					 */
    fl_flag  = 0;		/* roads to lowest flna			 */
    fh_flag  = 0;		/* roads to highest flna		 */
	s_flag	 = 0;		/* printing stats flag			 */
	r_flag 	 = 0;		/* road stats flag				 */
	sc_flag  = 1;		/* stream connectivity flag		 */
	slp_flag = 0;		/* slope use flag		         */
	st_flag  = 0;		/* scaling stream side patches 	 */
	sewer_flag = 0;		/* route through a sewer network */
	scale_trans = 1.0;
	scale_dem = 1.0;	/* scaling for dem values        */
	pst_flag = 0;		/* print stream table flag            */
	cell = 10.0;		/* default resolution            */
	width = 5;			/* default road width            */
	basinid = 1;

	// GRASS init
	G_gisinit(argv[0]);

	// GRASS module header
	struct GModule* module;
	module = G_define_module();
	module->keywords = "RHESSys";
	module->description = "Creates a flowpaths file for input into RHESSys";

	// GRASS arguments
	struct Flag* debug_flag = G_define_flag();
	debug_flag->key = 'g';
	debug_flag->description = "Enable printouts during compuation of flowpaths";

	struct Flag* lowest_flna_flag = G_define_flag();
	lowest_flna_flag->key = 'l';
	lowest_flna_flag->description = "Roads to lowest flna interval";

	struct Flag* highest_flna_flag = G_define_flag();
	highest_flna_flag->key = 'h';
	highest_flna_flag->description = "Roads to highest flna interval";

	struct Flag* drainage_stats_flag = G_define_flag();
	drainage_stats_flag->key = 'd';
	drainage_stats_flag->description = "Print drainage statistics";

	struct Flag* road_drainage_stats_flag = G_define_flag();
	road_drainage_stats_flag->key = 'r';
	road_drainage_stats_flag->description = "Road flag for drainage statistics";

	struct Option* stream_connectivity_opt = G_define_option();
	stream_connectivity_opt->key = "streamcon";
	stream_connectivity_opt->type = TYPE_STRING;
	stream_connectivity_opt->required = NO;
	stream_connectivity_opt->description = "Stream connectivity type: [random(default), internal, none]";
	stream_connectivity_opt->multiple=NO;

	struct Option* scale_dem_opt = G_define_option();
	scale_dem_opt->key = "scaledem";
	scale_dem_opt->type = TYPE_DOUBLE;
	scale_dem_opt->required = NO;
	scale_dem_opt->description = "DEM scaling factor";

	struct Option* cell_size = G_define_option();
	cell_size->key = "cellsize";
	cell_size->type = TYPE_DOUBLE;
	cell_size->required = NO;
	cell_size->description = "cell size";


	struct Option* scale_stream_trans = G_define_option();
	scale_stream_trans->key = "scaletrans";
	scale_stream_trans->type = TYPE_DOUBLE;
	scale_stream_trans->required = NO;
	scale_stream_trans->description = "Scaleing factor for streamside transmissivity";

	struct Flag* use_sewer_flag = G_define_flag();
	use_sewer_flag->key = 's';
	use_sewer_flag->description = "Use a sewer image to route water from roads";

	struct Flag* print_stream_table_flag = G_define_flag();
	print_stream_table_flag->key = 'p';
	print_stream_table_flag->description = "Print stream table";

	struct Option* road_width_opt = G_define_option();
	road_width_opt->key = "roadwidth";
	road_width_opt->type = TYPE_DOUBLE;
	road_width_opt->required = NO;
	road_width_opt->description = "Road width(m). [Default 5m]";
	
	struct Option* slope_use_opt = G_define_option();
	slope_use_opt->key = "slopeuse";
	slope_use_opt->type = TYPE_STRING;
	slope_use_opt->required = NO;
	slope_use_opt->description = "Change the use of slope in the compuation of gamma [standard(default), internal, max]";

	struct Option* basin_id_opt = G_define_option();
	basin_id_opt->key = "basinid";
	basin_id_opt->type = TYPE_INTEGER;
	basin_id_opt->required = NO;
	basin_id_opt->description = "Basin ID";

	struct Option* output_name_opt = G_define_option();
	output_name_opt->key = "output";
	output_name_opt->type = TYPE_STRING;
	output_name_opt->required = YES;
	output_name_opt->description = "Output name";

	// Arguments that specify the names of required raster maps

	struct Option* stream_raster_opt = G_define_option();
	stream_raster_opt->key = "stream";
	stream_raster_opt->type = TYPE_STRING;
	stream_raster_opt->required = YES;
	stream_raster_opt->description = "stream";

	struct Option* road_raster_opt = G_define_option();
	road_raster_opt->key = "road";
	road_raster_opt->type = TYPE_STRING;
	road_raster_opt->required = YES;
	road_raster_opt->description = "road";

	struct Option* dem_raster_opt = G_define_option();
	dem_raster_opt->key = "dem";
	dem_raster_opt->type = TYPE_STRING;
	dem_raster_opt->required = YES;
	dem_raster_opt->description = "dem";

	struct Option* slope_raster_opt = G_define_option();
	slope_raster_opt->key = "slope";
	slope_raster_opt->type = TYPE_STRING;
	slope_raster_opt->required = YES;
	slope_raster_opt->description = "slope";

	struct Option* template_opt = G_define_option();
	template_opt->key = "template";
	template_opt->type = TYPE_STRING;
	template_opt->required = YES;
	template_opt->description = "RHESSys template file from which to extract the basin, hill, zone, and patch GRASS rasters";

	struct Option* flna_raster_opt = G_define_option();
	flna_raster_opt->key = "flna";
	flna_raster_opt->type = TYPE_STRING;
	flna_raster_opt->required = NO;
	flna_raster_opt->description = "FLNA map";

	struct Option* sewers_raster_opt = G_define_option();
	sewers_raster_opt->key = "sewer";
	sewers_raster_opt->type = TYPE_STRING;
	sewers_raster_opt->required = NO;
	sewers_raster_opt->description = "Sewer map";

	// Parse GRASS arguments
	if (G_parser(argc, argv))
		exit(1);

	// Get values from GRASS arguments
	fl_flag = lowest_flna_flag->answer;
	fh_flag = highest_flna_flag->answer;
	if (fl_flag || fh_flag) {
		f_flag = 1;
	} else {
		f_flag = 0;
	}
	s_flag = drainage_stats_flag->answer;
	r_flag = road_drainage_stats_flag->answer;

	if (stream_connectivity_opt->answer != NULL) {  
		// Default is 1, random connectivity. See sc_flag declaration for setting default.
		if (strcmp("random",stream_connectivity_opt->answer)) {
			sc_flag = 1;
		} else if (strcmp("internal", stream_connectivity_opt->answer)) {
			sc_flag = 2;
		} else if (strcmp("none", stream_connectivity_opt->answer)) {
			sc_flag = 0;
		} else {
			G_fatal_error("\"%s\" is not a valid argument to stream", stream_connectivity_opt->answer);
		}
	}

	if (scale_dem_opt->answer != NULL) {	
		// Default is set at declaration, only modify if set
		if (sscanf(scale_dem_opt->answer, "%lf", &scale_dem) != 1) {
			G_fatal_error("Error setting the scale dem value");
		}
	}

	if (cell_size->answer != NULL) {	
		// Default is set at declaration, only modify if set
		if (sscanf(cell_size->answer, "%lf", &cell) != 1) {
			G_fatal_error("Error setting the cell size value");
		}
	}
	
	if (scale_stream_trans->answer != NULL) {
		// Default is set at declaration, only modify if set
		if (sscanf(scale_stream_trans->answer, "%lf", &scale_trans) != 1) {
			G_fatal_error("Error setting the scale trans value");
		}
	}

	sewer_flag = use_sewer_flag->answer;
	pst_flag = print_stream_table_flag->answer;

	if (road_width_opt->answer != NULL) {
		// Default is set at declaration
		if (sscanf(road_width_opt->answer, "%lf", &width) != 1) {
			G_fatal_error("Error setting the road width value");
		}
	}

	if (slope_use_opt->answer != NULL) {
		if (strcmp("standard", slope_use_opt->answer)) {
			slp_flag = 0;
		} else if (strcmp("internal", slope_use_opt->answer)) {
			slp_flag = 1;
		} else if (strcmp("max", slope_use_opt->answer)) {
			slp_flag = 2;
		} else {
			G_fatal_error("\"%s\" is not a valid argument to slopeuse", slope_use_opt->answer);
		}
	}

	if (basin_id_opt->answer != NULL) {
		// Default set at declaration
		if (sscanf(basin_id_opt->answer, "%d", &basinid) != 1) {
			G_fatal_error("Error setting the basin ID value");
		}
	}

	// Name for output files, default to template file name
	strcpy(input_prefix, output_name_opt->answer);	
	strcpy(output_suffix, "");

	if (flna_raster_opt->answer != NULL) {
		rnflna = flna_raster_opt->answer;
	}

	if (sewers_raster_opt->answer != NULL) {
		rnsewers = sewers_raster_opt->answer;
	}
		// Need to implement verbose	
	rndem = dem_raster_opt->answer;
	fntemplate = template_opt->answer;
	rnroads = road_raster_opt->answer;
	rnstream = stream_raster_opt->answer;
	rnslope = slope_raster_opt->answer;

    printf("Create_flowpaths.C\n\n");
    printf("Create_flowpaths.C\n\n");


	// Read in the names of the basin, hill, zone, and patch maps from the
	// template file.
	FILE* template_fp = fopen(fntemplate, "r");
	if (template_fp == NULL) {
		G_fatal_error("Can not open template file <%s>", fntemplate);
	}

	char	template_buffer[MAXS];
	char	first[MAXS];
	char	second[MAXS];

	printf("\n Reading template file %s", fntemplate);
	printf("\n Reading template file %s", fntemplate);

	while (fgets(template_buffer, sizeof(template_buffer), template_fp) != NULL) {
		sscanf(template_buffer, "%s %s", first, second);

		// Check if the token is anything we are looking for
		if (strcmp("_basin", first) == 0 ) {
			strcpy(rnbasin, second);
			printf("Basin: %s\n", rnbasin);
		} else if (strcmp("_hillslope", first) == 0 ) {
			strcpy(rnhill, second);
			printf("Hillslope: %s\n", rnhill);
		} else if (strcmp("_zone", first) == 0 ) {
			strcpy(rnzone, second);
			printf("Zone: %s\n", rnzone);
		} else if (strcmp("_patch", first) == 0 ) {
			strcpy(rnpatch, second);
			printf("Patch: %s\n", rnpatch);
		}
	}
	fclose (template_fp);
	printf("Basin: %s\n", rnbasin);
	printf("Hillslope: %s\n", rnhill);
	printf("Zone: %s\n", rnzone);
	printf("Patch: %s\n", rnpatch);



	/* open some diagnostic output files */

  strcpy(name, input_prefix);
  strcat(name, ".build"); 
  if ( (out1 = fopen(name, "w")) == NULL)
        {
        printf("cannot open build file\n");
        exit(1);
       	} 

  strcpy(name2, input_prefix);
  strcat(name2, ".pit"); 
  if ( (out2 = fopen(name2, "w")) == NULL)
        {
        printf("cannot open pit file\n");
        exit(1);
       	} 


	/* allocate and input map images */
	// figure out what's happening with maxr, maxc, possible raster
	// size mismatch
	struct Cell_head dem_header;
	dem = (double*)raster2array(rndem, &dem_header, &maxr, &maxc, DCELL_TYPE);

	struct Cell_head patch_header;
	patch = (int*)raster2array(rnpatch, &patch_header, NULL, NULL, CELL_TYPE);

	// Get cell size based off of that in the patchmap
	// Assuming square cells, otherwise fixes deeper in cf will need to be made.
	/*
	if (patch_header.ew_res != patch_header.ns_res) {
		printf("Attempting to use non-square cells\n");
		exit(1);
	}
	cell = patch_header.ew_res;
	cell = 10.0;
	*/
	printf("\n cell resolution is %lf\n", cell);

	struct Cell_head zone_header;
	zone = (int*)raster2array(rnzone, &zone_header, NULL, NULL, CELL_TYPE);

	struct Cell_head hill_header;
	hill = (int*)raster2array(rnhill, &hill_header, NULL, NULL, CELL_TYPE);

	struct Cell_head stream_header;
	stream = (int*)raster2array(rnstream, &stream_header, NULL, NULL, CELL_TYPE);

    if ((sc_flag == 1) || (slp_flag > 0) ){ 
		struct Cell_head slope_header;
		slope = (float*)raster2array(rnslope, &slope_header, NULL, NULL, FCELL_TYPE);
	}

	struct Cell_head roads_header;
	roads = (int*)raster2array(rnroads, &roads_header, NULL, NULL, CELL_TYPE);

    if (sewer_flag == 1) {
		struct Cell_head sewers_header;
		sewers = (int*)raster2array(rnsewers, &sewers_header, NULL, NULL, CELL_TYPE);
	}

	
	if (f_flag) {
		struct Cell_head flna_header;
		flna = (double*)raster2array(rnflna, &flna_header, NULL, NULL, DCELL_TYPE);
	}
	else flna = NULL;


	/* allocate flow table */
	flow_table = (struct flow_struct *)calloc((maxr*maxc),sizeof(struct flow_struct));


	printf("Building flow table\n");
	num_patches = build_flow_table(flow_table, dem, slope, hill, zone, patch, 
					stream, roads, sewers, flna, out1, maxr, 
					maxc,f_flag, sc_flag, sewer_flag, slp_flag, cell, 
                    scale_dem);

	fclose(out1);

	/* processes patches - computing means and neighbour slopes and gammas */
	printf("\n Computing gamma");
	num_stream = compute_gamma(flow_table, num_patches, out2,scale_trans,cell,sc_flag,
                        slp_flag, d_flag);

	
	 /* remove pits and re-order patches appropriately */
	 
	printf("\n Removing pits");
	   remove_pits(flow_table, num_patches, sc_flag, slp_flag, cell, out2); 
	 
	/* add roads */
	printf("\n Adding roads");
	 add_roads(flow_table, num_patches, out2, cell);

	/* find_receiving patch for flna options */
	if (f_flag)
		route_roads_to_patches(flow_table, num_patches, fl_flag);
	

	printf("\n Computing upslope area");
 		tmp = compute_upslope_area(flow_table, num_patches, out2, r_flag,cell);

	if (s_flag == 1) {
		printf("\n Printing drainage stats");
		print_drain_stats(num_patches, flow_table);
		tmp = compute_dist_from_road(flow_table, num_patches, out2, cell);	
 		tmp = compute_drainage_density(flow_table, num_patches, cell);
		}
	

	printf("\n Printing flowtable");
	print_flow_table(num_patches,flow_table,sc_flag,slp_flag,cell,scale_trans,
                     input_prefix, output_suffix, width);

	if (pst_flag == 1) {
		printf("\n Printing  stream table");
		print_stream_table(num_patches,num_stream, flow_table,sc_flag,slp_flag,cell,scale_trans,
                     input_prefix, output_suffix, width, basinid);
		}

	
	fclose(out2);
	printf("\n Finished Createflowpaths \n\n");
    exit(0);


} /* end create_flowpaths.c */


