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
#include "rhessys.h"

void  compute_patch_family_routing(struct zone_object *zone)
{

    /*--------------------------------------------------------------*/
    /*	Local function definition.	                          	    */
    /*--------------------------------------------------------------*/

    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                        */
    /*--------------------------------------------------------------*/

    int pf, i;

    double wet_mean;
    double wet_mean_sat;
    double area_sum;
    double wp_mean;
    double k_z_mean;
    double area_sum_g;

    double delta_L_actual;
    double delta_L_potential;
    double delta_L_sa_actual;
    double delta_L_sa_potential;

    double delta_G_actual;
    double delta_G_potential;
    double delta_G_sa_actual;
    double delta_G_sa_potential;
    double rz_z_pct;
    double delta_G_rz;
    double un_z_pct;
    double delta_G_un;

    double delta_L_adj_rz;

    /*--------------------------------------------------------------*/
    /*	initializations						*/
    /*--------------------------------------------------------------*/
    
    // i think nothing here outside of patch family loop

    /*--------------------------------------------------------------*/
    /*	Loop through patch families in the zone   	                */
    /*--------------------------------------------------------------*/

    for (pf = 0; pf < zone[0].num_patch_families ; pf++)
    {

    /*--------------------------------------------------------------*/
    /*	Patch family-specific definitions & initializations         */
    /*--------------------------------------------------------------*/

        // should patch be skipped/included(is it connected?)? 1 no, 0 yes - changes w every patch family so re allocate
        int skip[zone[0].patch_families[pf][0].num_patches_in_fam];

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
        k_z_mean = 0;


        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get mean wetness - root+unsat, sat	                */
        /*--------------------------------------------------------------*/

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // More initializations here that depend on patch loops

            zone[0].patch_families[pf][0].patches[i][0].rz_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].sat_transfer = 0;

            // if both sh coefficients are not 0, include patch
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g > 0 && 
            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l > 0) 
            {        
                // if there is a sharing coeff > 0, this allows skipping of patches with no sharing
                skip[i] = 1;
                
                // mean wetness based on root and unsat zone *area
                wet_mean = wet_mean + (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                    zone[0].patch_families[pf][0].patches[i][0].area;
                
                // mean saturated wetness ***** this could use sat_zone_storage? *****
                wet_mean_sat = wet_mean_sat + (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area);
                
                // mean k, k with depth ***** come back to this, this needs to be added into the saturated zone code ****
                // k_z_mean = k_z_mean + patch_family[patch,"k"] * exp(-patch_family[patch,"sa_q"]*patch_family[patch,"m"]) * patch_family[patch,"area"];

                // area sum (patch fam without skipped patches)
                area_sum = area_sum + zone[0].patch_families[pf][0].patches[i][0].area;
            }
            else 
            {
                // sharing coeffs area 0, skip this patch in this and subsiquent routing loops
                skip[i] = 0;
            }
        } // end loop 1
        
        // Get means instead of sums
        // Already weighted by area
        wet_mean = wet_mean / zone[0].patch_families[pf][0].num_patches_in_fam;
        wet_mean_sat = wet_mean_sat / zone[0].patch_families[pf][0].num_patches_in_fam;
        
        // Need to be area weighted
        //k_z_mean = k_z_mean / area_sum;

        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/

        

        // Initializations for loop 2
        delta_L_actual = 0;
        delta_L_potential = 0;
        delta_L_sa_actual = 0;
        delta_L_sa_potential = 0;

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
                delta_L_actual = delta_L_actual + delta_L[i];
                delta_L_potential = delta_L_potential + ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                    zone[0].patch_families[pf][0].patches[i][0].area - wet_mean);
            }
            else
            {
                delta_L[i] = 0;
            }
            // sat losers
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area > wet_mean_sat)
            { 
                delta_L_sa[i] = (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area - wet_mean_sat) * 
                    zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l;
                delta_L_sa_actual = delta_L_sa_actual + delta_L_sa[i];
                delta_L_sa_potential = delta_L_sa_potential + (zone[0].patch_families[pf][0].patches[i][0].sat_deficit * 
                    zone[0].patch_families[pf][0].patches[i][0].area - wet_mean_sat);
            }
            else
            {
                delta_L_sa[i] = 0;
            }
        } // end loop 2

        

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        

        delta_G_actual = 0;
        delta_G_potential = 0;
        delta_G_sa_actual = 0;
        delta_G_sa_potential = 0;

        area_sum_g = 0;
        

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            // rz + unsat > mean wetness (gainers)
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
            zone[0].patch_families[pf][0].patches[i][0].area < wet_mean)
            {
                delta_G[i] = (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage)) * 
                    (delta_L_actual / delta_L_potential) * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g;
                delta_G_actual = delta_G_actual + delta_G[i];
                delta_G_potential = delta_G_potential + (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * zone[0].patch_families[pf][0].patches[i][0].area);
                
                // Division of water (delta_G) between rz & unsat

                // percent of depth that is root zone - ***** double check vars here *****
                rz_z_pct = zone[0].patch_families[pf][0].patches[i][0].rootzone.depth / zone[0].patch_families[pf][0].patches[i][0].soil_defaults[0][0].soil_depth;                
                
                //root zone gain is minimum of delta_G * root zone depth percent and field capacity
                delta_G_rz = min(delta_G[i] * rz_z_pct, zone[0].patch_families[pf][0].patches[i][0].field_capacity);

                //root zone store update (area removed, so just a depth)
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = delta_G_rz / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].rz_storage = zone[0].patch_families[pf][0].patches[i][0].rz_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].rz_transfer;    
                
                // unsat depth pct
                un_z_pct = 1 - rz_z_pct;
                
                // unsat gain is delta_G * unsat depth pct + max of 0 and delta_G_rz - rz field capacity
                delta_G_un = delta_G[i] * un_z_pct + max(delta_G_rz - zone[0].patch_families[pf][0].patches[i][0].field_capacity, 0);

                // unsat store update (depth)
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = delta_G_un / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage = zone[0].patch_families[pf][0].patches[i][0].unsat_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;

                // wilting point mean (of gainers)
                wp_mean = wp_mean + (zone[0].patch_families[pf][0].patches[i][0].wilting_point * zone[0].patch_families[pf][0].patches[i][0].area);
                // incrament gainer count
                area_sum_g = area_sum_g + zone[0].patch_families[pf][0].patches[i][0].area;

            }
            else
            {
                delta_G[i] = 0;
            }
            // sat gainers
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area < wet_mean_sat)
            {
                delta_G_sa[i] = (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area) * 
                    delta_L_sa_actual/delta_L_sa_potential * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g;

                delta_G_sa_actual = delta_G_sa_actual + delta_G_sa[i];
                
                delta_G_sa_potential = delta_G_sa_potential + (wet_mean - zone[0].patch_families[pf][0].patches[i][0].sat_deficit * 
                    zone[0].patch_families[pf][0].patches[i][0].area);
            
                // change in sat store ***** revisit this, make sure deficit is being updated correctly, should be in meters of water *****
                zone[0].patch_families[pf][0].patches[i][0].sat_transfer = delta_G_sa[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].sat_deficit = zone[0].patch_families[pf][0].patches[i][0].sat_deficit + 
                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
            } 
            else
            {
                delta_G_sa[i] = 0; // idk if this is needed, since the actual patch object value is updated above
            }
            
        } // end loop 3

        // Get mean wilting point of gainers
        wp_mean = wp_mean / area_sum_g;

        
        /*--------------------------------------------------------------*/
        /*	loop 4, loop through and reallocate to losing patches     	*/
        /*--------------------------------------------------------------*/

        // if loss>gain, allocating proportionately based on area*delta
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
            zone[0].patch_families[pf][0].patches[i][0].area > wet_mean)
            {
                // update loss values for gains less than potential, should have no impact if L actual = G actual, but tiny rounding values show up for some reason.
                delta_L_adj[i] = delta_L[i] - (delta_L_actual - delta_G_actual) * (delta_L[i]/delta_L_actual);
                
                // distribute delta_L_adj between rz and unsat
                // RZ - removes down to the mean wilting point of the gaining patches
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = - min(delta_L_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area, 
                    zone[0].patch_families[pf][0].patches[i][0].rz_storage - wp_mean );

                // update rz store
                zone[0].patch_families[pf][0].patches[i][0].rz_storage = zone[0].patch_families[pf][0].patches[i][0].rz_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].rz_transfer;

                // Unsat - removes remainder (if any) thar shouldve been taken from root zone
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = - max(delta_L_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area - delta_L_adj_rz ,0);
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage = zone[0].patch_families[pf][0].patches[i][0].unsat_storage + 
                    zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;
            }
            if (skip[i] == 1 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area > wet_mean_sat)
            {
                delta_L_sa_adj[i] = delta_L_sa[i] - (delta_L_sa_actual - delta_G_sa_actual) * (delta_L_sa[i]/delta_L_sa_actual);

                zone[0].patch_families[pf][0].patches[i][0].sat_transfer = - delta_L_sa_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                zone[0].patch_families[pf][0].patches[i][0].sat_deficit = zone[0].patch_families[pf][0].patches[i][0].sat_deficit + 
                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
            }
        } // end loop 4

        
    } // end patch family loop

return;

}
