/*--------------------------------------------------------------*/
/*					                                            */
/*	            	compute_shaded_kdown 	                    */
/*					                                            */
/*					                                            */
/*	NAME						                                */
/*	compute_shaded_kdown.c                                      */
/*		- Adjusts kdowns based on patch family shading          */
/*					                                            */
/*	SYNOPSIS			                                        */
/*  void    compute_shaded_kdown(                               */
/*              struct  patch_object    *patch,                 */
/*              struct  zone_object     *zone,                  */
/*              struct  command_line_object     *command_line)  */
/*										                        */
/*	OPTIONS								                    	*/
/*										                        */
/*										                        */
/*	DESCRIPTION								                    */
/*  Calculates the change to kdown_direct of each patch based   */
/*  on the patch family horizon and the base horizon            */
/*										                        */
/*	PROGRAMMER NOTES							                */
/*										                        */
/*	Jan, 2020 William Burke						            */
/*											                    */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "rhessys.h"


void    compute_shaded_kdown(   struct  patch_object    *patch, 
                                struct  zone_object     *zone, 
                                struct  command_line_object     *command_line)
{

    /*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
    int s;
    double	dayl_rad;
	double	hr_rad;
	double	adj_rad;
	double	adj_hr;
	double	adj_hr_pct;
	double	adj_pct;
	double	signa_pct[6] = { 0.08, 0.17, 0.25, 0.33, 0.42, 0.50};
	double	curve_pct[6] = { 0.01, 0.02, 0.05, 0.14, 0.32, 0.62};

    /*--------------------------------------------------------------*/
	/*  Compare horizons to patch family adjusted horizon           */
	/*--------------------------------------------------------------*/
    if (patch[0].family_horizon > asin(zone[0].w_horizon) > 0 || patch[0].family_horizon > asin(zone[0].e_horizon) > 0) {
        // 180 degrees = pi = 3.141593 rad
        // day length/sky in radians
        dayl_rad = PI - asin(zone[0].w_horizon) - asin(zone[0].e_horizon);
        // 1 hr in radians
        hr_rad = dayl_rad / (zone[0].metv.dayl / 3600);

        if (command_line[0].verbose_flag == -6) printf("\n ----- Patch family shading ----- \n");
        if (command_line[0].verbose_flag == -6) printf("| Day length hrs %f | E hor deg %f | W hor deg %f | day length radians %f | 1hr radians %f | \n", 
            zone[0].metv.dayl / 3600, asin(zone[0].w_horizon) * (180 / PI), asin(zone[0].e_horizon) * (180 / PI), dayl_rad, hr_rad);

        // if pfam horizon is 1hr+ of horizon greater than w+e horizon
        if ((patch[0].family_horizon - asin(zone[0].w_horizon)) + (patch[0].family_horizon - asin(zone[0].e_horizon)) > hr_rad) {
            // day length/sky radians for patch family
            adj_rad = PI - 2 * patch[0].family_horizon;
            // patch family day length in hrs, rounded to nearest hr
            adj_hr = round((zone[0].metv.dayl / 3600) * adj_rad/dayl_rad);
            if (command_line[0].verbose_flag == -6) printf("| Horizon radians %f -> %f | Daylight hrs %f -> %f |\n", dayl_rad, adj_rad, zone[0].metv.dayl / 3600, adj_hr);
            // pct to adjust kdowns down by
            adj_hr_pct = ((zone[0].metv.dayl / 3600) - adj_hr) / (zone[0].metv.dayl / 3600);

            // using rough 0.5 sigma intervals to percent under curve of normal distribution
            // see wikipedia on 68–95–99.7 rule
            adj_pct = 0;
            for (s = 0; s < 6; s++){
                if (adj_hr_pct > signa_pct[s]) {
                    adj_pct = curve_pct[s];
                }
                else {
                    s = 6;
                }
            }

            if (command_line[0].verbose_flag == -6) printf("| Pct reduction %f | Kdown direct %f -> %f |\n", 
                adj_pct, patch[0].Kdown_direct, patch[0].Kdown_direct * (1 - adj_pct));
            patch[0].Kdown_direct *= (1 - adj_pct);
        }
    }

    return;
    
}
