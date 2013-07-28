/*--------------------------------------------------------------*/
/*                                                              */
/*		lairead						*/
/*                                                              */
/*  NAME                                                        */
/*		 lairead					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 lairead( 					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*	-allom  name of allometric ratios file			*/
/*	-old    old worldfile name				*/
/*	-redef  new redefine worldfile name			*/
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
#include <float.h>
#include <limits.h>
#include <grass/gis.h>
#include "grassio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"

// Prints out program arguments documentation
void usage(void);


main(int argc, char *argv[])
{
    /* local variable declarations */
    int	 	i, nvegtype, num_patches;
    FILE 	*out1, *out2, *fac;
    FILE 	*fdWorld, *fdRedefWorld;
    int		tmp, maxr, maxc;

    /* filenames for each image and file */
    char *fnAllom, *fnRedefWorld, *fnWorld;
    char name[MAXS];

    /* Raster names */
    char *rnLAI, *rnVegid, *rnPatch, *rnHill, *rnZone, *rnMask;

    /* set pointers for images */
    int	     *vegid;
    int      *mask;
    int      *patch;
    int      *hill;
    int      *zone;
    float *lai;

    struct    allom_struct	*allometric_table;
    struct    flow_struct	*flow_table;

    struct Cell_head lai_header;
    struct Cell_head vegid_header;
    struct Cell_head mask_header;
    struct Cell_head patch_header;
    struct Cell_head zone_header;
    struct Cell_head hill_header;
    struct Cell_head root_header;

    // GRASS init
    G_gisinit(argv[0]);

    // GRASS module header
    struct GModule* module;
    module = G_define_module();
    module->keywords = "RHESSys";
    module->description = "Output a redefined worldfile with initialized or altered stratum state variables based on an LAI image";

    // GRASS arguments
    struct Option* worldfile_name_opt = G_define_option();
    worldfile_name_opt->key = "old";
    worldfile_name_opt->type = TYPE_STRING;
    worldfile_name_opt->required = YES;
    worldfile_name_opt->description = "Existing worldfile name";

    struct Option* redef_worldfile_name_opt = G_define_option();
    redef_worldfile_name_opt->key = "redef";
    redef_worldfile_name_opt->type = TYPE_STRING;
    redef_worldfile_name_opt->required = YES;
    redef_worldfile_name_opt->description = "Redefined worldfile name";

    struct Option* allom_filename_opt = G_define_option();
    allom_filename_opt->key = "allom";
    allom_filename_opt->type = TYPE_STRING;
    allom_filename_opt->required = YES;
    allom_filename_opt->description = "Allometric file name";

    // Arguments that specify the names of required raster maps
    struct Option* lai_raster_opt = G_define_option();
    lai_raster_opt->key = "lai";
    lai_raster_opt->type = TYPE_STRING;
    lai_raster_opt->required = YES;
    lai_raster_opt->description = "Leaf Area Index (LAI) raster map name";

    struct Option* vegid_raster_opt = G_define_option();
    vegid_raster_opt->key = "vegid";
    vegid_raster_opt->type = TYPE_STRING;
    vegid_raster_opt->required = YES;
    vegid_raster_opt->description = "VegId raster map name";

    struct Option* zone_raster_opt = G_define_option();
    zone_raster_opt->key = "zone";
    zone_raster_opt->type = TYPE_STRING;
    zone_raster_opt->required = YES;
    zone_raster_opt->description = "Zone raster map name";

    struct Option* hill_raster_opt = G_define_option();
    hill_raster_opt->key = "hill";
    hill_raster_opt->type = TYPE_STRING;
    hill_raster_opt->required = YES;
    hill_raster_opt->description = "Hill raster map name";

    struct Option* patch_raster_opt = G_define_option();
    patch_raster_opt->key = "patch";
    patch_raster_opt->type = TYPE_STRING;
    patch_raster_opt->required = NO;
    patch_raster_opt->description = "Patch raster map name";

    struct Option* mask_raster_opt = G_define_option();
    mask_raster_opt->key = "mask";
    mask_raster_opt->type = TYPE_STRING;
    mask_raster_opt->required = NO;
    mask_raster_opt->description = "Mask raster map name. If none supplied 'mask' will be used.";

    // Parse GRASS arguments
    if (G_parser(argc, argv))
        exit(EXIT_FAILURE);

    // Get values from GRASS arguments
    fnWorld    = worldfile_name_opt->answer;
    fnRedefWorld  = redef_worldfile_name_opt->answer;
    fnAllom       = allom_filename_opt->answer;
    rnLAI         = lai_raster_opt->answer;
    rnVegid       = vegid_raster_opt->answer;
    rnZone        = zone_raster_opt->answer;
    rnHill        = hill_raster_opt->answer;
    rnPatch       = patch_raster_opt->answer;
    rnMask		  = mask_raster_opt->answer;

    /* allometric file */
    if ( (fac = fopen(fnAllom, "r")) == NULL) {
        printf("cannot open allometric ratio file \'%s\'\n", fnAllom);
        exit(EXIT_FAILURE);
    }

    if ( (fdWorld = fopen(fnWorld, "r")) == NULL) {
        printf("cannot open world file \'%s\' for reading.\n", fnWorld);
        usage();
        exit(EXIT_FAILURE);
    }

    /* redefine world */
    if ( (fdRedefWorld = fopen(fnRedefWorld, "w")) == NULL) {
        printf("cannot open new world \'%s\' file for output.\n", fnRedefWorld);
        usage();
        exit(EXIT_FAILURE);
    }

    /* open some diagnostic output files */
    strcpy(name, fnRedefWorld);
    strcat(name, ".log");
    if ( (out1 = fopen(name, "w")) == NULL) {
        printf("cannot open diagnostic file %s for writing\n", name);
        usage();
        exit(EXIT_FAILURE);
    }

    /* allocate and input map images */
    //printf("Reading %s raster map\n", rnVegid);

    if (rnVegid != NULL) {
        vegid = (int*)raster2array(rnVegid, &vegid_header, &maxr, &maxc, CELL_TYPE);
    }

    mask = (int *) calloc(maxr*maxc, sizeof(int));

    if (rnMask != NULL) {
    	mask = (int*)raster2array(rnMask, &mask_header, NULL, NULL, CELL_TYPE);
    } else {
    	mask = (int*)raster2array("mask", &mask_header, NULL, NULL, CELL_TYPE);
    }

    //printf("Reading %s raster map\n", rnPatch);
    patch = (int *) calloc(maxr*maxc, sizeof(int));

    if (rnPatch != NULL) {
        patch = (int*)raster2array(rnPatch, &patch_header, NULL, NULL, CELL_TYPE);
    }

    //printf("Reading %s raster map\n", rnZone);
    zone = (int *) malloc(maxr*maxc*sizeof(int));

    if (rnZone != NULL) {
        zone = (int*)raster2array(rnZone, &zone_header, NULL, NULL, CELL_TYPE);
    }

    //printf("Reading %s raster map\n", rnHill);
    hill = (int *) malloc(maxr*maxc*sizeof(int));

    if (rnHill != NULL) {
        hill = (int*)raster2array(rnHill, &hill_header, NULL, NULL, CELL_TYPE);
    }

    //printf("Reading %s raster map\n", rnLAI);
    lai = (float *) malloc(maxr*maxc*sizeof(float));

    if (rnLAI != NULL) {
        printf("Converting lai raster map\n");
        lai = (float*)raster2array(rnLAI, &lai_header, NULL, NULL, FCELL_TYPE);
    }

    int row;
    int col;
    int index;

    // The non-Grass version of this program read in all data from ASCII files that were created by 
    // exporting data from GRASS rasters using r.out.ascii, which would assign 0 to all NULL values.
    for (row = 0; row < maxr; ++row) {
        for (col = 0; col < maxc; ++col) {
            index = col + row*maxc;
            //printf("row: %d, col: %d, vegid: %d, lai: %7.2f, patch: %d, zone: %d, hill: %d\n", row, col, vegid[index], lai[index], patch[index], zone[index], hill[index]);

            if (mask[index] != 1) {
                lai[index] = 0.0;
                hill[index] = 0;
                zone[index] = 0;
                patch[index] = 0;
                vegid[index] = 0;
            } else {
                if (lai[index] < 0.0)
                    lai[index] = 0.0;
            }
        }
    }

    /* allocate flow table */
    flow_table = (struct flow_struct *)calloc((maxr*maxc),sizeof(struct flow_struct));

    printf("Building patch table...\n");
    /* build representation of patches */
    num_patches = build_flow_table(out1, flow_table, vegid, lai, hill, zone, patch, maxr, maxc);

    /* read in allometric ratios table */
    fscanf(fac,"%d", &nvegtype);
    allometric_table = (struct allom_struct *)calloc(nvegtype, sizeof(struct allom_struct));
    read_allom_table(fac, nvegtype, allometric_table);
    printf("number of vegetation types from allometric table: %d\n\n", nvegtype);

    /* link patches with allometry */
    printf("\n Linking vegetation and allometry...\n");
    link_patch_veg(flow_table, allometric_table, num_patches, nvegtype);

    /* now read in and change the worldfile */
    change_world(fnWorld, fdWorld, fdRedefWorld, flow_table, num_patches);

    printf("\n Finished LAIread \n\n");
    exit(EXIT_SUCCESS);
} /* end lairead.c */


void usage(void) {
	G_usage();

    return;
}
