/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/** @file main.c
 *
 *      @brief Main driver for createflowpaths.
 *
 *  OPTIONS
 *              -v      Verbose Option
 *              -l    roads to lowest flna interval
 *              -h      roads to highest flna interval
 *              -s print drainage statistics
 *              -r      road flag for drainage statistics
 *              -stream stream connectivity is assumed
 *                      1       random slope value
 *                      2       internal slope value
 *                      0       no connectivity (default)
 *              -scaledem scale dem values by this amount
 *              -scaletrans     scale streamside transmissivity
 *              -bi     basin ID
 *              -sw     use a sewer image to route water from roads
 *              -pst    print stream table
 *              -pre    input image file name prefix
 *              -w      road width (default is 5m)
 *              -a arcview ascii data files (default is GRASS ascii)
 *              -slp slope flag - change use of slope
 *                      in computation of gamma
 *                      0  standard
 *                      1  internal slpe of patch
 *                      2 max slope of patch
 *              -o output file name (default -pre opt + _flow_table.dat)
 *
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <grass/gis.h>

#include "main.h"
#include "grassio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"
#include "route_roofs.h"
#include "normalize_roof_patches.h"
#include "verify.h"
#include "patch_hash_table.h"

int main(int argc, char *argv[]) {
    /* local variable declarations */
    //int           i,
    int surface_num_stream = 0;
    int subsurface_num_stream = 0;
    int surface_num_patches = 0;
    int subsurface_num_patches = 0;
    FILE *out1, *out2;
    int basinid, tmp, maxr, maxc;
    double cell, width;
    int pst_flag;
    int f_flag; /**< boolean value determining whether route_roads_to_patches should be called */
    int fl_flag;
    int fh_flag;
    int vflag;
    int s_flag;
    int r_flag;
    int slp_flag; /**< Slope flag, values defined in main.h */
    int sc_flag; /**< Stream connectivity flag, values defined in main.h */
    //int           st_flag,
    int sewer_flag; /**< Sewer flag, boolean indicating whether sewer map is present */
    int singleFlowtable_flag; /**< boolean indicating if a single flow table is to produce or
    							   if separate surface and sub-surface tables are to be produced */
    int roofs_flag; /**< Roofs flag, was a roofs raster specified at the command line? */
    int priority_flag; /**< Priority flag, was a priority flow receiver raster specified at the command line? */
    int priority_weight = 3; /**< Weight to give priority flow receivers */
    int d_flag, dbg_flag;
    double scale_trans, scale_dem;
    char input_prefix[MAXS];
    char output_suffix[MAXS];

    /* filenames for each image and file */
    char name[MAXS], name2[MAXS];
    char* rnflna;
    char* rnslope;
    char* fntemplate;
    char rnbasin[MAXS];
    char rnhill[MAXS];
    char rnzone[MAXS];
    char rnpatch[MAXS];
    char* rndem;
    char* rnroads;
    char* rnimpervious;
    char* rnstream;
    char* rnsewers;
    char* rnroofs;
    char* rnpriority;
    char* rnperviousRcv = NULL;
        
    /* set pointers for images */
    double *dem;
    int *patch;
    float *slope;
    int *hill;
    int *zone;
    int *stream;
    int *roads;
    int *impervious;
    int *sewers;
    double *flna;
    double* roofs;
    int *priority = NULL;
    int *pervious_recv_out_rast = NULL;

    bool success = true;
    
    //float          *ehr;
    //float          *whr;

    PatchTable_t *surfacePatchTable = NULL;
    PatchTable_t *subsurfacePatchTable = NULL;
    struct flow_struct* surface_flow_table = NULL;
    struct flow_struct* subsurface_flow_table = NULL;
        
    d_flag = FALSE; /**< debuf flag                                  */
    vflag = FALSE; /**< verbose flag                                         */
    fl_flag = FALSE; /**< roads to lowest flna                       */
    fh_flag = FALSE; /**< roads to highest flna              */
    s_flag = FALSE; /**< printing stats flag                         */
    r_flag = FALSE; /**< road stats flag                             */
    sc_flag = STREAM_CONNECTIVITY_RANDOM; /**< stream connectivity flag              */
    slp_flag = SLOPE_STANDARD; /**< slope use flag                   */
    //st_flag  = 0;         /**< scaling stream side patches         */
    sewer_flag = FALSE; /**< route through a sewer network (NOT YET IMPLEMENTED) */
    roofs_flag = FALSE;
    priority_flag = FALSE;

    singleFlowtable_flag = TRUE; /**< Generate a single combined surface and sub-surface
    								  flow table unless a surface feature dataset is provided
    								  e.g. roofs */
    dbg_flag = FALSE; /**< debugging flag, set to not remove temp files */
    scale_trans = 1.0;
    scale_dem = 1.0; /**< scaling for dem values        */
    pst_flag = FALSE; /**< print stream table flag            */
    cell = DEFAULT_CELL_RESOLUTION; /**< default resolution of DEM          */
    width = DEFAULT_ROAD_WIDTH; /**< default road width            */
    basinid = DEFAULT_BASIN_ID;

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
    stream_connectivity_opt->description =
        "Stream connectivity type: [random(default), internal, none]";
    stream_connectivity_opt->multiple = NO;

    struct Option* scale_dem_opt = G_define_option();
    scale_dem_opt->key = "scaledem";
    scale_dem_opt->type = TYPE_DOUBLE;
    scale_dem_opt->required = NO;
    scale_dem_opt->description = "DEM scaling factor";

    struct Option* cell_size = G_define_option();
    cell_size->key = "cellsize";
    cell_size->type = TYPE_DOUBLE;
    cell_size->required = YES;
    cell_size->description = "cell size [Default 10m]";

    struct Option* scale_stream_trans = G_define_option();
    scale_stream_trans->key = "scaletrans";
    scale_stream_trans->type = TYPE_DOUBLE;
    scale_stream_trans->required = NO;
    scale_stream_trans->description =
        "Scaleing factor for streamside transmissivity";

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
    slope_use_opt->description =
        "Change the use of slope in the compuation of gamma [standard(default), internal, max]";

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

    struct Option* impervious_raster_opt = G_define_option();
    impervious_raster_opt->key = "impervious";
    impervious_raster_opt->type = TYPE_STRING;
    impervious_raster_opt->required = NO;
    impervious_raster_opt->description = "impervious";

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
    template_opt->description =
        "RHESSys template file from which to extract the basin, hill, zone, and patch GRASS rasters";

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

    struct Option* roof_opt = G_define_option();
    roof_opt->key = "roof";
    roof_opt->type = TYPE_STRING;
    roof_opt->required = NO;
    roof_opt->description = "Roof map";
        
    struct Option* priority_opt = G_define_option();
    priority_opt->key = "priority";
    priority_opt->type = TYPE_STRING;
    priority_opt->required = NO;
    priority_opt->description = "Priority flow receivers map";

    struct Option* pervious_recv_out_opt = G_define_option();
	pervious_recv_out_opt->key = "perviousrecv";
	pervious_recv_out_opt->type = TYPE_STRING;
	pervious_recv_out_opt->required = NO;
	pervious_recv_out_opt->description = "Map of pervious cells that actually received flow. Value of each cell represents number of contributor cells.";

    struct Option* weight_opt = G_define_option();
    weight_opt->key = "weight";
    weight_opt->type = TYPE_INTEGER;
    weight_opt->required = NO;
    weight_opt->description = "Weight to give priority flow receivers.  Defaults to 3";

    // Parse GRASS arguments
    if (G_parser(argc, argv)) exit(EXIT_FAILURE);

    // Get values from GRASS arguments
    dbg_flag = debug_flag->answer;
    fl_flag = lowest_flna_flag->answer;
    fh_flag = highest_flna_flag->answer;
    if (fl_flag || fh_flag) {
        f_flag = TRUE;
    } else {
        f_flag = FALSE;
    }
    s_flag = drainage_stats_flag->answer;
    r_flag = road_drainage_stats_flag->answer;

    if (stream_connectivity_opt->answer != NULL ) {
        // Default is 1, random connectivity. See sc_flag declaration for setting default.
        if (strcmp("random", stream_connectivity_opt->answer) == 0) {
            sc_flag = STREAM_CONNECTIVITY_RANDOM;
        } else if (strcmp("internal", stream_connectivity_opt->answer) == 0) {
            sc_flag = STREAM_CONNECTIVITY_INTERNAL;
        } else if (strcmp("none", stream_connectivity_opt->answer) == 0) {
            sc_flag = STREAM_CONNECTIVITY_NONE;
        } else {
            G_fatal_error("\"%s\" is not a valid argument to stream",
                          stream_connectivity_opt->answer);
        }
    }

    if (scale_dem_opt->answer != NULL ) {
        // Default is set at declaration, only modify if set
        if (sscanf(scale_dem_opt->answer, "%lf", &scale_dem) != 1) {
            G_fatal_error("Error setting the scale dem value");
        }
    }

    if (cell_size->answer != NULL ) {
        // Default is set at declaration, only modify if set
        if (sscanf(cell_size->answer, "%lf", &cell) != 1) {
            G_fatal_error("Error setting the cell size value");
        }
    }

    if (scale_stream_trans->answer != NULL ) {
        // Default is set at declaration, only modify if set
        if (sscanf(scale_stream_trans->answer, "%lf", &scale_trans) != 1) {
            G_fatal_error("Error setting the scale trans value");
        }
    }

    sewer_flag = use_sewer_flag->answer;
    pst_flag = print_stream_table_flag->answer;

    if (road_width_opt->answer != NULL ) {
        // Default is set at declaration
        if (sscanf(road_width_opt->answer, "%lf", &width) != 1) {
            G_fatal_error("Error setting the road width value");
        }
    }

    if (slope_use_opt->answer != NULL ) {
        if (strcmp("standard", slope_use_opt->answer) == 0) {
            slp_flag = SLOPE_STANDARD;
        } else if (strcmp("internal", slope_use_opt->answer) == 0) {
            slp_flag = SLOPE_INTERNAL;
        } else if (strcmp("max", slope_use_opt->answer) == 0) {
            slp_flag = SLOPE_MAX;
        } else {
            G_fatal_error("\"%s\" is not a valid argument to slopeuse",
                          slope_use_opt->answer);
        }
    }

    if (basin_id_opt->answer != NULL ) {
        // Default set at declaration
        if (sscanf(basin_id_opt->answer, "%d", &basinid) != 1) {
            G_fatal_error("Error setting the basin ID value");
        }
    }

    // Name for output files, default to template file name
    // Input prefix is left over from pre-grass version.
    strcpy(input_prefix, output_name_opt->answer);
    strcpy(output_suffix, ".flow");

    if (flna_raster_opt->answer != NULL ) {
        rnflna = flna_raster_opt->answer;
    }

    if (sewers_raster_opt->answer != NULL ) {
        rnsewers = sewers_raster_opt->answer;
    }

    // Right now, the only trigger for generating separate surface and subsurface flow tables
    // is if the user provides a roof connectivity layer
    if ( roof_opt->answer != NULL ) {
    	if ( impervious_raster_opt->answer == NULL ) {
    		G_fatal_error("Impervious raster must be specified when roof connectivity raster is specified");
    	}
    	singleFlowtable_flag = FALSE;
    	roofs_flag = TRUE;
    }

    if (singleFlowtable_flag == FALSE) {

    	// We're routing surface flows separately, check for a flow reciever priority map
    	if ( NULL != priority_opt->answer ) {
    		// Yes, there was a priority flow receivers map
    		priority_flag = TRUE;

    		// Check for weight
    		if ( NULL != weight_opt->answer ) {
    			priority_weight = atoi(weight_opt->answer);
    		}
    	}

    	// Check for pervious receiver output
		if ( NULL != pervious_recv_out_opt->answer ) {
			rnperviousRcv = pervious_recv_out_opt->answer;
		}
    }

    // Need to implement verbose
    rndem = dem_raster_opt->answer;
    fntemplate = template_opt->answer;
    rnroads = road_raster_opt->answer;
    rnimpervious = impervious_raster_opt->answer;
    rnstream = stream_raster_opt->answer;
    rnslope = slope_raster_opt->answer;
    rnroofs = roof_opt->answer;
    rnpriority = priority_opt->answer;
        
    printf("Create_flowpaths.C\n\n");

    // Read in the names of the basin, hill, zone, and patch maps from the
    // template file.
    FILE* template_fp = fopen(fntemplate, "r");
    if (template_fp == NULL ) {
        G_fatal_error("Can not open template file <%s>", fntemplate);
    }

    char template_buffer[MAXS];
    char first[MAXS];
    char second[MAXS];

    printf("Reading template file %s\n", fntemplate);

    while (fgets(template_buffer, sizeof(template_buffer), template_fp)
           != NULL ) {
        sscanf(template_buffer, "%s %s", first, second);

        // Check if the token is anything we are looking for
        if (strcmp("_basin", first) == 0) {
            strcpy(rnbasin, second);
            printf("Basin: %s\n", rnbasin);
        } else if (strcmp("_hillslope", first) == 0) {
            strcpy(rnhill, second);
            printf("Hillslope: %s\n", rnhill);
        } else if (strcmp("_zone", first) == 0) {
            strcpy(rnzone, second);
            printf("Zone: %s\n", rnzone);
        } else if (strcmp("_patch", first) == 0) {
            strcpy(rnpatch, second);
            printf("Patch: %s\n", rnpatch);
        }
    }
    fclose(template_fp);

    /* open some diagnostic output files */

    strcpy(name, input_prefix);
    strcat(name, ".build");
    if ((out1 = fopen(name, "w")) == NULL ) {
        printf("cannot open build file\n");
        exit(EXIT_FAILURE);
    }

    strcpy(name2, input_prefix);
    strcat(name2, ".pit");
    if ((out2 = fopen(name2, "w")) == NULL ) {
        printf("cannot open pit file\n");
        exit(EXIT_FAILURE);
    }

    /* allocate and input map images */
    // figure out what's happening with maxr, maxc, possible raster
    // size mismatch
    struct Cell_head dem_header;
    dem = (double*) raster2array(rndem, &dem_header, &maxr, &maxc, DCELL_TYPE);

    struct Cell_head patch_header;
    patch = (int*) raster2array(rnpatch, &patch_header, NULL, NULL, CELL_TYPE);

    // Get cell size based off of that in the patchmap
    // Does not assume pixels are square, instead takes the root of their
    // square.
    /* cell = sqrt(patch_header.ew_res * patch_header.ns_res); */

    printf("\n cell resolution is %lf ", cell);

    struct Cell_head zone_header;
    zone = (int*) raster2array(rnzone, &zone_header, NULL, NULL, CELL_TYPE);

    struct Cell_head hill_header;
    hill = (int*) raster2array(rnhill, &hill_header, NULL, NULL, CELL_TYPE);

    struct Cell_head stream_header;
    stream = (int*) raster2array(rnstream, &stream_header, NULL, NULL,
                                 CELL_TYPE);

    if ((STREAM_CONNECTIVITY_RANDOM == sc_flag)
        || (SLOPE_STANDARD != slp_flag)) {
        struct Cell_head slope_header;
        slope = (float*) raster2array(rnslope, &slope_header, NULL, NULL,
                                      FCELL_TYPE);
    }

    struct Cell_head roads_header;
    roads = (int*) raster2array(rnroads, &roads_header, NULL, NULL, CELL_TYPE);
        
    // Added to support roof raster map - hcj
    if (roofs_flag) {
    	int roofsRows, roofsCols;
    	struct Cell_head roofs_header;
    	roofs = (double*) raster2array(rnroofs, &roofs_header, NULL, NULL, DCELL_TYPE);

    	struct Cell_head impervious_header;
    	impervious = (int*) raster2array(rnimpervious, &impervious_header, NULL, NULL, CELL_TYPE);
    }

    if (priority_flag) {
    	struct Cell_head priority_header;
    	priority = (int*) raster2array(rnpriority, &priority_header, NULL, NULL, CELL_TYPE);
    }

    if (sewer_flag) {
        struct Cell_head sewers_header;
        sewers = (int*) raster2array(rnsewers, &sewers_header, NULL, NULL,
                                     CELL_TYPE);
    }

    if (f_flag) {
        struct Cell_head flna_header;
        flna = (double*) raster2array(rnflna, &flna_header, NULL, NULL,
                                      DCELL_TYPE);
    } else
        flna = NULL;

    /* allocate output map */
    if ( NULL != rnperviousRcv ) {
    	pervious_recv_out_rast = (int *) calloc( maxr * maxc, sizeof(int) );
    }

    /* allocate patch tables */
    // Use relatively large tables, some users may need to make the table larger
    // for very large numbers of patches (>100k) to improve performance (table size is currently static)
    if (!singleFlowtable_flag) {
    	surfacePatchTable = allocatePatchHashTable(PATCH_HASH_TABLE_DEFAULT_SIZE);
    }
    subsurfacePatchTable = allocatePatchHashTable(PATCH_HASH_TABLE_DEFAULT_SIZE);


    /* allocate flow table */
    if (!singleFlowtable_flag) {
		surface_flow_table = (struct flow_struct *) calloc((maxr * maxc),
														   sizeof(struct flow_struct));
    }
    subsurface_flow_table = (struct flow_struct *) calloc((maxr * maxc),
                                                          sizeof(struct flow_struct));

    if (!singleFlowtable_flag) {
		printf("\n Building surface flow table");
		surface_num_patches = build_flow_table(surface_flow_table, surfacePatchTable, dem, slope, hill, zone, patch,
											   stream, roads, sewers, roofs, flna, out1, maxr, maxc, f_flag, sc_flag,
											   sewer_flag, slp_flag, cell, scale_dem, true);

		printf("\n Building subsurface flow table");
    } else {
    	printf("\n Building flow table");
    }
    subsurface_num_patches = build_flow_table(subsurface_flow_table, subsurfacePatchTable, dem, slope, hill, zone, patch,
                                              stream, roads, sewers, roofs, flna, out1, maxr, maxc, f_flag, sc_flag,
                                              sewer_flag, slp_flag, cell, scale_dem, false);
        
    fclose(out1);

    // Do some verification for debugging purposes
    // success = verify_num_adjacent(surface_flow_table, surface_num_patches);
    
    // Short circuit roof patches to the nearest road patches
    if (roofs_flag) {
    	printf("\n Route roofs to roads");
    	success = route_roofs_to_roads(surface_flow_table, surface_num_patches, surfacePatchTable,
    			roofs, impervious, stream, priority, dem, priority_weight,
    			patch, hill, zone, maxr, maxc, pervious_recv_out_rast);
    }

    // Do some verification for debugging purposes
    // success = verify_num_adjacent(surface_flow_table, surface_num_patches);
    
    // Normalize roof patches
    if (roofs_flag) {
    	printf("\n Normalizing roof patches");
    	success = normalize_roof_patches(surface_flow_table, surface_num_patches);
    }

    if (!singleFlowtable_flag) {
		/* processes patches - computing means and neighbour slopes and gammas */
		printf("\n Computing surface gamma");
		surface_num_stream = compute_gamma(surface_flow_table, surface_num_patches, surfacePatchTable, out2, scale_trans, cell,
										   sc_flag, slp_flag, d_flag, true);

		printf("\n Computing subsurface gamma");
    } else {
    	printf("\n Computing gamma");
    }
    subsurface_num_stream = compute_gamma(subsurface_flow_table, subsurface_num_patches, subsurfacePatchTable, out2, scale_trans, cell,
                                          sc_flag, slp_flag, d_flag, false);

    /* remove pits and re-order patches appropriately */
    if (!singleFlowtable_flag) {
		printf("\n Removing surface pits");
		remove_pits(surface_flow_table, surface_num_patches, sc_flag, slp_flag, cell, out2);

		printf("\n Removing subsurface pits");
    } else {
    	printf("\n Removing pits");
    }
    remove_pits(subsurface_flow_table, subsurface_num_patches, sc_flag, slp_flag, cell, out2);

    /* add roads */
    if (!singleFlowtable_flag) {
		printf("\n Adding roads to surface");
		add_roads(surface_flow_table, surface_num_patches, out2, cell);

		printf("\n Adding roads to subsurface");
    } else {
    	printf("\n Adding roads");
    }
    add_roads(subsurface_flow_table, subsurface_num_patches, out2, cell);

    /* find_receiving patch for flna options */
    if (!singleFlowtable_flag) {
    	if (f_flag) route_roads_to_patches(surface_flow_table, surface_num_patches, fl_flag);
    }
    if (f_flag) route_roads_to_patches(subsurface_flow_table, subsurface_num_patches, fl_flag);

    if (!singleFlowtable_flag) {
		printf("\n Computing surface upslope area");
		tmp = compute_upslope_area(surface_flow_table, surface_num_patches, out2, r_flag, cell);

		printf("\n Computing subsurface upslope area");
    } else {
    	printf("\n Computing upslope area");
    }
    tmp = compute_upslope_area(subsurface_flow_table, subsurface_num_patches, out2, r_flag, cell);

    if (s_flag) {
        if (!singleFlowtable_flag) {
        	printf("\n Printing surface drainage stats");
        	print_drain_stats(surface_num_patches, surface_flow_table);
        	tmp = compute_dist_from_road(surface_flow_table, surface_num_patches, out2, cell);
        	tmp = compute_drainage_density(surface_flow_table, surface_num_patches, cell);

        	printf("\n Printing subsurface drainage stats");
        } else {
        	printf("\n Printing drainage stats");
        }
        print_drain_stats(subsurface_num_patches, subsurface_flow_table);
        tmp = compute_dist_from_road(subsurface_flow_table, subsurface_num_patches, out2, cell);
        tmp = compute_drainage_density(subsurface_flow_table, subsurface_num_patches, cell);
    }

    if (!singleFlowtable_flag) {
		printf("\n Printing surface flowtable");
		strncpy(output_suffix, "_surface.flow", MAXS);
		print_flow_table(surface_num_patches, surface_flow_table, sc_flag, slp_flag, cell,
						 scale_trans, input_prefix, output_suffix, width);

		printf("\n Printing subsurface flowtable");
		strncpy(output_suffix, "_subsurface.flow", MAXS);
    } else {
    	printf("\n Printing flowtable");
    	strncpy(output_suffix, ".flow", MAXS);
    }
    print_flow_table(subsurface_num_patches, subsurface_flow_table, sc_flag, slp_flag, cell,
                     scale_trans, input_prefix, output_suffix, width);

    /* Print stream table */
    // SHOULD THIS ONLY BE DONE FOR THE SURFACE FLOW TABLE IF THERE ARE TWO FLOW TABLES? bcm
    if (pst_flag) {
    	if (!singleFlowtable_flag) {
			printf("\n Printing surface stream table");
			strncpy(output_suffix, "_surface.flow", MAXS);
			print_stream_table(surface_num_patches, surface_num_stream, surface_flow_table, sc_flag,
							   slp_flag, cell, scale_trans, input_prefix, output_suffix, width,
							   basinid);

			printf("\n Printing subsurface stream table");
			strncpy(output_suffix, "_subsurface.flow", MAXS);
    	} else {
    		printf("\n Printing  stream table");
    		strncpy(output_suffix, ".flow", MAXS);
    	}
        print_stream_table(subsurface_num_patches, subsurface_num_stream, subsurface_flow_table, sc_flag,
                           slp_flag, cell, scale_trans, input_prefix, output_suffix, width,
                           basinid);
    }

    fclose(out2);

    // Remove temporary files
    char buildfn[MAXS];
    char gammafn[MAXS];
    char pitfn[MAXS];
    strcpy(buildfn, input_prefix);
    strcpy(gammafn, input_prefix);
    strcpy(pitfn, input_prefix);
    strcat(buildfn, ".build");
    strcat(gammafn, ".gamma");
    strcat(pitfn, ".pit");

    if (!dbg_flag) { // Do not clean up temp files if debugging is enabled
        printf("\n Cleaning up temporary files");
        if (remove(buildfn) != 0)
            printf("\n Unable to remove .build temp file");
        if (remove(gammafn) != 0)
            printf("\n Unable to remove .gamma temp file");
        if (remove(pitfn) != 0)
            printf("\n Unable to remove .pit temp file");
        if (remove("RoofGeometries.txt") != 0) {
        	printf("\n Unable to remove RoofGeometry temp file");
        }
    }

    /* write output map */
    if ( NULL != rnperviousRcv ) {
    	array2raster(pervious_recv_out_rast, rnperviousRcv, CELL_TYPE,
    			maxr, maxc);
    }

    if (!singleFlowtable_flag) {
    	freePatchHashTable(surfacePatchTable);
    }
    freePatchHashTable(subsurfacePatchTable);

    printf("\n Finished Createflowpaths \n\n");
    return (EXIT_SUCCESS);
} /* end main.c */

