/*--------------------------------------------------------------*/
/*					                                            */
/*		compute_patch_family_routing		                    */
/*					                                            */
/*	compute_patch_family_routing.c - routes within patch family	*/
/*					                                            */
/*	NAME						                                */
/*	compute_patch_family_routing.c - routes within patch family	*/
/*					                                            */
/*	SYNOPSIS			                                        */
/*	void compute_patch_family_routing( 	                        */
/*						    struct zone_object *zone)           */
/*										                        */
/*	OPTIONS								                    	*/
/*										                        */
/*										                        */
/*	DESCRIPTION								                    */
/*  For all patch families in a zone, routes water between      */
/*	patches in each patch family. Routing is based on speudo-   */
/*	root access, and has distinct rules for root, unsaturated,  */
/*	and saturaetd zones, and transfers between patches are      */
/*  modified by sh_l and sh_g coefficients.                     */
/*                                                              */
/*	PROGRAMMER NOTES							                */
/*										                        */
/*	July, 2019 William Burke						            */
/*											                    */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

void  compute_patch_family_routing( struct zone_object *zone,
                                    struct command_line_object *command_line)
{

    /*--------------------------------------------------------------*/
    /*	Local function definition.	                          	    */
    /*--------------------------------------------------------------*/

	double	Ksat_z_curve(
		int,
		double,
		double,
		double);

    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                        */
    /*--------------------------------------------------------------*/

    int pf;
    int i;

    double wet_mean;            // mean wetness for rz and unsat, area weighted
    double wet_mean_sat;        // mean wetness for saturated zoen
    double area_sum;            // sum of areas in patch family
    double wp_mean;             // mean wilting point 
    double k_z_mean;  // set but not used
    double area_sum_g;

    double delta_L_actual;
    double delta_L_potential;
    double delta_L_sa_actual;
    double delta_L_sa_potential;

    double delta_G_actual;
    double delta_G_potential;
    double delta_G_sa_actual;
    double delta_G_sa_potential;
 
    double delta_G_rz;
    double delta_G_un;

    double delta_L_adj_rz;

    double rz_unsat_transfer_sum;   // set not used
    double sat_transfer_sum;        // set not used

    /*--------------------------------------------------------------*/
    /*	Loop through patch families in the zone   	                */
    /*--------------------------------------------------------------*/

    // Taking -6 as multiscale verbose flag number, can change if needed

    for (pf = 0; pf < zone[0].num_patch_families; pf++)
    {
        if (command_line[0].verbose_flag == -6) printf("--- Patch Family %d ---\n", zone[0].patch_families[pf][0].family_ID);

    /*--------------------------------------------------------------*/
    /*	Patch family specific definitions + initializations         */
    /*--------------------------------------------------------------*/

        int skip[zone[0].patch_families[pf][0].num_patches_in_fam];         // 1 no 0 yes
        double ksat[zone[0].patch_families[pf][0].num_patches_in_fam];
        double rz_z_pct[zone[0].patch_families[pf][0].num_patches_in_fam];
        double un_z_pct[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_L[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_L_sa[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_G[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_G_sa[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_L_adj[zone[0].patch_families[pf][0].num_patches_in_fam];
        double delta_L_sa_adj[zone[0].patch_families[pf][0].num_patches_in_fam];

        // Initializations
        wet_mean = 0;
        wet_mean_sat = 0;
        area_sum = 0;
        wp_mean = 0;

        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get mean wetness - root+unsat, sat	                */
        /*--------------------------------------------------------------*/

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // Patch family & patch specific initializations
            zone[0].patch_families[pf][0].patches[i][0].rz_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].sat_transfer = 0;

            // if both sh coefficients are not 0, include patch
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g > 0 && 
            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l > 0) 
            {        
                // if there is a sharing coeff > 0, this allows skipping of patches with no sharing
                skip[i] = 1;

                if (command_line[0].verbose_flag == -6)
                {
                    printf("patch %d <pre transfers> ", zone[0].patch_families[pf][0].patches[i][0].ID);
                    printf("rz stor %f,",zone[0].patch_families[pf][0].patches[i][0].rz_storage);
                    printf("unsat stor %f",zone[0].patch_families[pf][0].patches[i][0].unsat_storage);
                }
                
                // incrament mean wetness based on root and unsat zone *area
                wet_mean += (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                    zone[0].patch_families[pf][0].patches[i][0].area;
                
                // mean saturated wetness ***** this could use sat_zone_storage? *****
                wet_mean_sat += (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area);
                
                // get ksat -- using sat_deficit_z since that (or rootzone.depth) is what's used as input to compute_unsat_zone_drainage elsewhere
                ksat[i] = Ksat_z_curve(
                    command_line[0].verbose_flag,
                    zone[0].patch_families[pf][0].patches[i][0].m,
                    zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z,
                    zone[0].patch_families[pf][0].patches[i][0].Ksat_0);
                if (command_line[0].verbose_flag == -6) printf("ksat %f\n", ksat[i]);

                // area sum (patch fam without skipped patches)
                area_sum += zone[0].patch_families[pf][0].patches[i][0].area;

                // percent of depth that is root zone
                rz_z_pct[i] = zone[0].patch_families[pf][0].patches[i][0].rootzone.depth / zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z;

                // unsat depth pct
                un_z_pct[i] = 1 - rz_z_pct[i];
                if (command_line[0].verbose_flag == -6) printf("rz pct %f unsat pct %f",  rz_z_pct[i], un_z_pct[i]);
                

            }
            else 
            {
                // sharing coeffs area 0, skip this patch in this and subsiquent routing loops
                skip[i] = 0;
            }
        } // end loop 1
        
        // Get means instead of sums

        if (command_line[0].verbose_flag == -6) {
            printf("Wetness sum = %f, ", wet_mean);
            printf("Area sum = %f, ", area_sum);
        } 

        // Already weighted by area, vol per patch
        wet_mean /= zone[0].patch_families[pf][0].num_patches_in_fam;
        if (command_line[0].verbose_flag == -6) printf("Mean wetness = %f,\n", wet_mean);

        wet_mean_sat /= zone[0].patch_families[pf][0].num_patches_in_fam;


        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/

        // Initializations for loop 2
        delta_L_actual = 0;
        delta_L_potential = 0;
        delta_L_sa_actual = 0;
        delta_L_sa_potential = 0;

        if (command_line[0].verbose_flag == -6) printf("Delta L ");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // if - no skip and rz + unsat is > mean (losers)
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
            zone[0].patch_families[pf][0].patches[i][0].area > wet_mean)
            {
                // could update skip to simplify IFs later on and make an index of losers vs gainers vs no sharing
                // skip[i] = 2 // skip == 0 is no sharing, skip == 1 is gaining, skip == 2 is losing
                delta_L[i] = ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                    zone[0].patch_families[pf][0].patches[i][0].area - wet_mean) * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l;
                delta_L_actual += delta_L[i];
                delta_L_potential += ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                    zone[0].patch_families[pf][0].patches[i][0].area - wet_mean);
                if (command_line[0].verbose_flag == -6) printf("%f ", delta_L[i]);
            }
            else
            {
                delta_L[i] = 0;
                if (command_line[0].verbose_flag == -6) printf("<skip>%f ", delta_L[i]);
            }
            // sat losers
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area > wet_mean_sat)
            { 
                delta_L_sa[i] = (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area - wet_mean_sat) * 
                    zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l;
                delta_L_sa_actual += delta_L_sa[i];
                delta_L_sa_potential += (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * 
                    zone[0].patch_families[pf][0].patches[i][0].area - wet_mean_sat);
            }
            else
            {
                delta_L_sa[i] = 0;
            }
        } // end loop 2

        if (command_line[0].verbose_flag == -6) printf("\n");

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        delta_G_actual = 0;
        delta_G_potential = 0;
        delta_G_sa_actual = 0;
        delta_G_sa_potential = 0;

        area_sum_g = 0;

        if (command_line[0].verbose_flag == -6) printf("Delta G ");
        
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // rz + unsat > mean wetness (gainers)
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
            zone[0].patch_families[pf][0].patches[i][0].area < wet_mean)
            {
                delta_G[i] = (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage)) * 
                    (delta_L_actual / delta_L_potential) * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g;
                delta_G_actual += delta_G[i];
                delta_G_potential += (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * zone[0].patch_families[pf][0].patches[i][0].area);
                if (command_line[0].verbose_flag == -6) printf("<tot>%f ", delta_G[i]);
                
                // Division of gaining water (delta_G) between rz & unsat
                //root zone gain is minimum of delta_G * root zone depth percent and field capacity
                delta_G_rz = min(delta_G[i] * rz_z_pct[i], zone[0].patch_families[pf][0].patches[i][0].field_capacity);
                if (command_line[0].verbose_flag == -6) printf("<rz vol>%f ", delta_G_rz);

                //root zone store update (area removed, so just a depth)
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = delta_G_rz / zone[0].patch_families[pf][0].patches[i][0].area;
                if (command_line[0].verbose_flag == -6) printf("<rz>%f ", zone[0].patch_families[pf][0].patches[i][0].rz_transfer);
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;    
                
                // unsat gain is delta_G * unsat depth pct + max of 0 and delta_G_rz - rz field capacity
                delta_G_un = delta_G[i] * un_z_pct[i] + max(delta_G_rz - zone[0].patch_families[pf][0].patches[i][0].field_capacity, 0);
                if (command_line[0].verbose_flag == -6) printf("<unsat vol>%f ", delta_G_un);

                // unsat store update (depth)
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = delta_G_un / zone[0].patch_families[pf][0].patches[i][0].area;
                if (command_line[0].verbose_flag == -6) printf("<unsat>%f ", zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;

                // wilting point mean (of gainers)
                wp_mean += (zone[0].patch_families[pf][0].patches[i][0].wilting_point * zone[0].patch_families[pf][0].patches[i][0].area);
                // incrament gainer count
                area_sum_g += zone[0].patch_families[pf][0].patches[i][0].area;

            }
            else
            {
                delta_G[i] = 0;
                if (command_line[0].verbose_flag == -6) printf("<skip>%f ", delta_G[i]);
            }
            // sat gainers
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area < wet_mean_sat)
            {
                delta_G_sa[i] = (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area) * 
                    delta_L_sa_actual/delta_L_sa_potential * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g;

                delta_G_sa_actual += delta_G_sa[i];
                
                delta_G_sa_potential += (wet_mean - zone[0].patch_families[pf][0].patches[i][0].sat_deficit * 
                    zone[0].patch_families[pf][0].patches[i][0].area);
            
                // change in sat store ***** revisit this, make sure deficit is being updated correctly, should be in meters of water *****
                zone[0].patch_families[pf][0].patches[i][0].sat_transfer = delta_G_sa[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].sat_deficit += zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
            } 
            else
            {
                delta_G_sa[i] = 0; // idk if this is needed, since the actual patch object value is updated above
            }
            
        } // end loop 3

        if (command_line[0].verbose_flag == -6) printf("\n");

        // Get mean wilting point of gainers
        wp_mean /= area_sum_g;
        if (command_line[0].verbose_flag == -6) printf("gainers wp%f\n", wp_mean);
        
        /*--------------------------------------------------------------*/
        /*	loop 4, loop through and reallocate to losing patches     	*/
        /*--------------------------------------------------------------*/

        if (command_line[0].verbose_flag == -6) printf("Losers adj ");

        // if loss>gain, allocating proportionately based on area*delta
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
            zone[0].patch_families[pf][0].patches[i][0].area > wet_mean)
            {
                // update loss values for gains less than potential, should have no impact if L actual = G actual, but tiny rounding values show up for some reason.
                delta_L_adj[i] = delta_L[i] - (delta_L_actual - delta_G_actual) * (delta_L[i]/delta_L_actual);
                if (command_line[0].verbose_flag == -6) printf("<tot>%f, ", delta_L_adj[i]);
                
                // distribute delta_L_adj between rz and unsat
                // RZ - removes down to the mean wilting point of the gaining patches
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = - min(delta_L_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area, 
                    zone[0].patch_families[pf][0].patches[i][0].rz_storage - wp_mean );
                if (command_line[0].verbose_flag == -6) printf("<rz>%f ", zone[0].patch_families[pf][0].patches[i][0].rz_transfer);

                // update rz store
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;

                // Unsat - removes remainder (if any) thar shouldve been taken from root zone
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = - max(delta_L_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area - delta_L_adj_rz ,0);
                if (command_line[0].verbose_flag == -6) printf("<unsat>%f ", zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);
                
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;
            }
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area > wet_mean_sat)
            {
                delta_L_sa_adj[i] = delta_L_sa[i] - (delta_L_sa_actual - delta_G_sa_actual) * (delta_L_sa[i]/delta_L_sa_actual);

                zone[0].patch_families[pf][0].patches[i][0].sat_transfer = - delta_L_sa_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].sat_deficit += zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
            }
        } // end loop 4
        
        /*--------------------------------------------------------------*/
        /*	Testing -_-                                              	*/
        /*--------------------------------------------------------------*/
       
        /*
        rz_unsat_transfer_sum = 0;
        sat_transfer_sum = 0;

        // testing
       
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            rz_unsat_transfer_sum += zone[0].patch_families[pf][0].patches[i][0].rz_transfer + zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;
            sat_transfer_sum += zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
        }

        if (rz_unsat_transfer_sum != 0)
        {

            for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
            {
                printf("\nrz transfer = %f \nrz store = \nunsat transfer = %f\nunsat store = \n",
                    zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                    //zone[0].patch_families[pf][0].patches[i][0].rz_storage,
                    zone[0].patch_families[pf][0].patches[i][0].unsat_transfer
                    //zone[0].patch_families[pf][0].patches[i][0].unsat_storage
                    );
            }

            printf("----------\n");
        }
        */

        // add if in case sum != 0, print all transfers for patch family if so
        
    } // end patch family loop

return;

}
