/*--------------------------------------------------------------*/
/*					                                            */
/*		compute_family_shading      		                    */
/*					                                            */
/*	compute_family_shading.c - calcs shading from patch family  */
/*					                                            */
/*	NAME						                                */
/*	compute_family_shading.c - calcs shading from patch family  */
/*					                                            */
/*	SYNOPSIS			                                        */
/*	void compute_family_shading( 	                        */
/*						    struct zone_object *zone)           */
/*										                        */
/*	OPTIONS								                    	*/
/*										                        */
/*										                        */
/*	DESCRIPTION								                    */
/*  Calculates the shading, if any, created by patches within   */
/*  a patch family                                              */
/*										                        */
/*	PROGRAMMER NOTES							                */
/*										                        */
/*	Sept, 2019 William Burke						            */
/*											                    */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void compute_family_shading(    struct zone_object *zone,
                                struct command_line_object *command_line)
{
    /*--------------------------------------------------------------*/
    /*	Local function definition.	                          	    */
    /*--------------------------------------------------------------*/

    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                        */
    /*--------------------------------------------------------------*/
    int     pf, i, s, maxs;
    double  avg_density;
    double  dist;
    double  height;
    double  strata_max_height;
    double  max_height;
    double  avg_height;
    double  d_height;

    /*--------------------------------------------------------------*/
    /*	Loop through patch families and patches                     */
    /*--------------------------------------------------------------*/
    if (command_line[0].verbose_flag == -6) printf("\n --- Patch family horizons ---\n");

    for (pf = 0; pf < zone[0].num_patch_families; pf++) 
    {
        if (command_line[0].verbose_flag == -6) printf("Patch family %d\n", zone[0].patch_families[pf][0].family_ID);

        // Initialize
        avg_density = 0;
        max_height = 0;
        avg_height = 0;
        height = 0;

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            /*--------------------------------------------------------------*/
            /*	Get mean family density and avg/max height                  */
            /*--------------------------------------------------------------*/
            // IF HEIGHTS GO HERE, CANNOT COMPARE JUST OTHER PATCHES, HAVE TO LUMP ITSELF IN
            // max height works fine, only avg gets skewed by inlcuding itself
            //aheight += strata->cover_fraction * (strata->epv.height) ;
            //height = zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].cover_fraction * zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].epv.height;

            // get max height of strata
	    maxs=0;
	    height=0.0;
	    strata_max_height=0;
            for (s = 0; s < zone[0].patch_families[pf][0].patches[i][0].num_canopy_strata; s++)
            {
                height = max(height, zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height);
                strata_max_height = max(strata_max_height, zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height);
		if (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height >= strata_max_height)
			maxs = s;
                if (command_line[0].verbose_flag == -6) printf("Strata %d | height %f |\n", s, zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height);
		
            }

            // max/avg of family
            max_height = max(max_height, height);
            avg_height += height;
            // if density is 0, either error or come up with a workaround.
            if (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[maxs][0].cs.stem_density <= 0) {
                fprintf(stderr, "ERROR: cs.stem_density must be set and greater than 0.\n");
                exit(EXIT_FAILURE);
            }
            avg_density += zone[0].patch_families[pf][0].patches[i][0].canopy_strata[maxs][0].cs.stem_density;
        } // end patch loop

        avg_height /= zone[0].patch_families[pf][0].num_patches_in_fam;
        avg_density /= zone[0].patch_families[pf][0].num_patches_in_fam;

        /*--------------------------------------------------------------*/
        /*	Find distance between trees/stems			                */
        /*--------------------------------------------------------------*/
        // assumes an even distribution of stems and a a square area - this could also be done patch by patch and then averaged, 
        dist = sqrt(zone[0].patch_families[pf][0].area) / sqrt(avg_density *zone[0].patch_families[pf][0].area);
        if (command_line[0].verbose_flag == -6) printf("Avg stem density %f | Distance %f \n", avg_density, dist);

        /*--------------------------------------------------------------*/
        /*	Calculate angle for each patch		                        */
        /*--------------------------------------------------------------*/
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // get index of  highest  strata
	    maxs=0;
	    strata_max_height=0.0;
            for (s = 0; s < zone[0].patch_families[pf][0].patches[i][0].num_canopy_strata; s++)
            {
                strata_max_height = max(strata_max_height, zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height);
		if (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[s][0].epv.height >= strata_max_height)
			maxs = s;
		
            }

            d_height = avg_height - 0.75 * zone[0].patch_families[pf][0].patches[i][0].canopy_strata[maxs][0].epv.height;

	    if (d_height > ZERO)
            	zone[0].patch_families[pf][0].patches[i][0].family_horizon = atan(d_height/dist); // angle to family horizon in radians
	    else
            	zone[0].patch_families[pf][0].patches[i][0].family_horizon = 0; // angle to family horizon in radians
		    


            if (command_line[0].verbose_flag == -6) printf("height diff = %f | ", d_height);
            if (command_line[0].verbose_flag == -6) printf("family horizon deg = %f\n", zone[0].patch_families[pf][0].patches[i][0].family_horizon * (180/PI));
            
        } // end patch loop

    } // end patch family loop

    return;

} // end function
