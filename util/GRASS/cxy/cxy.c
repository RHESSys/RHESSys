/*--------------------------------------------------------------*/
/*                                                              */
/*                                                              */
/*  NAME                                                        */
/*		 cxy.c (create x, y maps)                       */
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 Create x and y location maps based on          */
/*               the GRASS active region.                       */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-d print debug information 			*/
/*		-v print verbose information 			*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  1.0  25 June, 2012                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grass/gis.h>
#include "grassio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"
#include "limits.h"

/* Function prototypes */

/* Global variables */
int    debug;
int    verbose;

main(int argc, char *argv[])
{

    /* Constants. */

    /* local variable declarations */
    double xPosVal;
    double yPosVal;
    int fdx;
    int fdy;
    int	i;
    int index;
    int maxr, maxc;
    int row, col;
    struct Cell_head active_region_header;
    struct Cell_head xmap_header;
    struct Cell_head ymap_header;

    // Array for one row of values.
    void* xrast;
    void* yrast;

    /* filenames for each image and file */
    char* rnxmap;
    char* rnymap;

    // GRASS init
    G_gisinit(argv[0]);

    // GRASS module header
    struct GModule* module;
    module = G_define_module();
    module->keywords = "RHESSys";
    module->description = "Create x snd y maps from the GRASS active module region.";

    struct Option* xmap_raster_opt = G_define_option();
    xmap_raster_opt->key = "xmap";
    xmap_raster_opt->type = TYPE_STRING;
    xmap_raster_opt->required = YES;
    xmap_raster_opt->description = "x location map";

    struct Option* ymap_raster_opt = G_define_option();
    ymap_raster_opt->key = "ymap";
    ymap_raster_opt->type = TYPE_STRING;
    ymap_raster_opt->required = YES;
    ymap_raster_opt->description = "y location map";

    G_get_set_window(&active_region_header);
    /* Note that coor->answer is not given a default value. */
    struct Flag* verbose_flag  = G_define_flag();
    verbose_flag ->key = 'v';
    verbose_flag ->description = "print verbose information";

    struct Flag* debug_flag  = G_define_flag();
    debug_flag ->key = 'd';
    debug_flag ->description = "print debug info";

    // Parse GRASS arguments
    if (G_parser(argc, argv))
        exit(1);

    // Get values from GRASS arguments
    rnxmap   = xmap_raster_opt->answer;
    rnymap   = ymap_raster_opt->answer;

    // Get the size of the active module region (http://grass.osgeo.org/programming7/gislib.html#Region)
    maxr = G_window_rows();
    maxc = G_window_cols();

    // Check if the x and y maps already exist in the current mapset
    //char name[GNAME_MAX];

    //if (G_find_cell(name,G_mapset()) == NULL)
    /* not found */

    // Check if the raster file name is legal
    //G_legal_filename 

    debug = debug_flag->answer;
    verbose = verbose_flag->answer;

    xrast = G_allocate_raster_buf(DCELL_TYPE);
    yrast = G_allocate_raster_buf(DCELL_TYPE);

    if (verbose) {
        printf("number of rows: %d\n", maxr);
        printf("number of columns: %d\n", maxc);
    }

    if (verbose == 1) {
        printf("Writing x location information to map \"%s\"\n", rnxmap);
        printf("Writing y location information to map \"%s\"\n", rnymap);
    }

    if ((fdx = G_open_raster_new(rnxmap, DCELL_TYPE)) < 0) {
        G_fatal_error("Unable to create x values raster map <%s>", rnxmap);
    }

    if ((fdy = G_open_raster_new(rnymap, DCELL_TYPE)) < 0) {
        G_fatal_error("Unable to create y values raster map <%s>", rnymap);
    }

    // Allocate space for one row each of x and y locations.
    xrast = G_allocate_raster_buf(DCELL_TYPE);
    yrast = G_allocate_raster_buf(DCELL_TYPE);

    // Loop through the active region
    for (row = 0; row < maxr; ++row) {
        for (col = 0; col < maxc; ++col) {
            index = col + row*maxc;
            // Get the x,y location values (Adding .5 to col, row causes position of center of cell to be returned.)
            xPosVal = G_col_to_easting((double) col + .5, &active_region_header);
            yPosVal = G_row_to_northing((double) row + .5, &active_region_header);
            //printf("xPos: %7.2f, yPos: %7.2f\n", xPosVal, yPosVal);
            ((double*)xrast)[col] = xPosVal;
            ((double*)yrast)[col] = yPosVal;
        }

        // Write out one row of x values
        if (G_put_raster_row(fdx, xrast, DCELL_TYPE) < 0) {
            G_fatal_error("Failed writing x values raster map <%s>", rnxmap);
        }

        // Write out one row of y values
        if (G_put_raster_row(fdy, yrast, DCELL_TYPE) < 0) {
            G_fatal_error("Failed writing y values raster map <%s>", rnymap);
        }
    }

    G_free(xrast);
    G_close_cell(fdx);

    G_free(yrast);
    G_close_cell(fdy);
}
