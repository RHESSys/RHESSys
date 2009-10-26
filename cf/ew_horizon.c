/*--------------------------------------------------------------*/
/*                                                              */
/*		ew_horizon									    */
/*                                                              */
/*  NAME                                                        */
/*		 ew_horizon										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 ew_horizon( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*  revision:  6.0 29 April, 2005                               */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>

#include <grass/gis.h>
#include <grass/gisdefs.h>

#include "blender.h"
#include "fileio.h"
#include "grassio.h"

const RASTER_MAP_TYPE OUTPUT_TYPE = DCELL_TYPE; // Force raster output
												// as doubles


int main(int argc, char** argv)
{
	// GRASS init
	G_gisinit(argv[0]);

	// GRASS module header
	struct GModule* module = G_define_module();
	module->keywords = "RHESSys";
	module->description = "Calculates the east and west horizons for a DEM";

	// GRASS arguments
	struct Option* input_opt;
	input_opt = G_define_option();
	input_opt->key = "input";
	input_opt->type = TYPE_STRING;
	input_opt->required = YES;
	input_opt->description = "input raster name";
	input_opt->multiple = NO;

	struct Option* output_opt;
	output_opt = G_define_option();
	output_opt->key = "output";
	output_opt->type = TYPE_STRING;
	output_opt->required = NO;
	output_opt->description = "Optional file";
	output_opt->multiple = NO;

	struct Option* scale_opt;
	scale_opt = G_define_option();
	scale_opt->key = "scale";
	scale_opt->type = TYPE_DOUBLE;
	scale_opt->required = NO;
	scale_opt->description = "Factor to scale the DEM by before processing";
	scale_opt->multiple = NO;

	struct Flag* file_flag;
	file_flag = G_define_flag();
	file_flag->key = 'f';
	file_flag->description = "Set for a text printout of the horizon maps";

	struct Flag* debug_flag;
	debug_flag = G_define_flag();
	debug_flag->key = 'g';
	debug_flag->description = "Print out human readable horizon files for debugging";
	
	// Parse GRASS arguments
	if (G_parser(argc, argv) )
		exit(1);

	// Get values from GRASS arguments
	char* dem_name;
	dem_name = input_opt->answer;

	char* output_name;
	if (output_opt->answer != NULL) {
		output_name = output_opt->answer;
	} else {
		output_name = dem_name;
	}

	double scale_dem;
	if (scale_opt->answer != NULL) {
		scale_dem = atof(scale_opt->answer);
	} else {
		printf("No DEM scaling specified, setting to 1.0\n");
		scale_dem = 1.0; 
	}

	int save_files = file_flag->answer;
	int debug = debug_flag->answer;

	// Load the array into a double raster for use
	int maxr;
	int maxc;
	struct Cell_head	dem_head;
	double* dem = raster2array(dem_name, &dem_head, &maxr, &maxc);


	// Actual data processing part starts here
	int inx, cr, hr_inx;
	int r,c,pch;
	double x, y;
	double curr_ehr, curr_whr;
	double new_ehr, new_whr;
	double curr_elev, elev_diff;
	double maxe_elev, maxw_elev;
	// Get the cel size from the raster header. Requires square pixels, so if
	// ew_res and ns_res are different throw an error
	if ( dem_head.ew_res != dem_head.ns_res ) {
		G_fatal_error("Non square pixels found in raster map <%s>", dem_name);
		exit(1);
	}
	// Doucle check that this value is reported as 30
	double cell = dem_head.ew_res;
	printf ("Cell size: %g\n", cell);

	// Allocate memory to store the computed east and west horizons	
	double* ehr;
	double* whr;
	ehr = calloc(maxr * maxc, sizeof(double));
	whr = calloc(maxr * maxc, sizeof(double));

	curr_ehr = 0;
	curr_whr = 0;

	printf("\n Starting EW horizon \n");

	for (r=0; r< maxr; r++) {
		for (c=0; c < maxc; c++) {
			inx = r*maxc+c;
			curr_ehr = 0.0;
			curr_whr = 0.0;
			maxe_elev = 0.0;
			maxw_elev = 0.0;
			curr_elev = dem[inx]*scale_dem;

			for (cr = 0; cr < maxc; cr++) {
				hr_inx = r*maxc+cr;	
				elev_diff = dem[hr_inx]*scale_dem-curr_elev;
				if (cr < c) {
					if (elev_diff > 0.001) {
						y = (c-cr)*cell;
						x = elev_diff;
				 		new_whr = x / sqrt(x*x + y*y );
					} else {
						new_whr = 0.0;
					}
					
					if (new_whr > curr_whr) { 
						curr_whr = new_whr;
						maxw_elev = dem[hr_inx]; 
					}
				}
				
				if (cr > c) {
					if (elev_diff > 0.001) {
						y = (cr-c)*cell;
						x = elev_diff;
				 		new_ehr = x / sqrt(x*x + y*y );
					} else {
						new_ehr = 0.0;
					}
					
					if (new_ehr > curr_ehr) {
						curr_ehr = new_ehr;
						maxe_elev = dem[hr_inx]; 
					}
				}
	
			} /* end column scan */
			ehr[inx] = curr_ehr;	
			whr[inx] = curr_whr;	
		}	/* end col */	
	} /* end row */

	// Create output names
	char fnehr[MAXS];
	char fnwhr[MAXS];
	strcpy(fnehr, output_name);
	strcpy(fnwhr, output_name);
	strcat(fnehr, ".ehr");
	strcat(fnwhr, ".whr");


	// Create a new GRASS raster map for the output
	array2raster(ehr, fnehr, DCELL_TYPE, maxr, maxc);
	array2raster(whr, fnwhr, DCELL_TYPE, maxr, maxc);

	if (save_files) {
		output_ascii_double(ehr, fnehr, maxc, maxr);
		output_ascii_double(whr, fnwhr, maxc, maxr);
		printf("\n");
		printf("Writing out %s\n", fnehr);
		printf("Writing out %s\n", fnwhr);
	}

	if (debug) {
		output_ascii_double_debug(ehr, fnehr, maxc, maxr);
		output_ascii_double_debug(whr, fnwhr, maxc, maxr);
		printf("\n");
		printf("Writing out %s\n", fnehr);
		printf("Writing out %s\n", fnwhr);
	}

	free(dem);
	free(ehr);
	free(whr);

	exit(EXIT_SUCCESS);
}


