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
#include <stdlib.h> // i think not needed?
#include <math.h>
#include "rhessys.h"

void compute_patch_family_routing(struct zone_object *zone,
                                  struct command_line_object *command_line)
{

    /*--------------------------------------------------------------*/
    /*	Local function definition.	                          	    */
    /*--------------------------------------------------------------*/

    double Ksat_z_curve(
        int,
        double,
        double,
        double);

    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                        */
    /*--------------------------------------------------------------*/

    int pf;
    int i;
    int p_ct;            // number of patches in patch family
    int p_ct_skip;       // number of patches in patch family without skipped patches
    double wet_mean;     // mean wetness for rz+unsat, meters water
    double wet_mean_sat; // mean wetness for sat zone, meters water
    double area_sum;     // sum of areas in patch family
    double wp_mean;      // mean wilting point, meters
    double area_sum_g;   // sum of gaining patches area
    double dL_act;       // sum of (actual) rz+unsat zone loses over family w/ sharing coefs
    double dL_pot;       // sum of (potential) rz+unsat zone loses over family w/o sharing coefs
    double dL_sat_act;   // sum of (actual) sat zone loses over family w/ sharing coefs
    double dL_sat_pot;   // sum of (potential) sat zone loses over family w/ sharing coefs
    double dG_act;       // sum of (actual) rz+unsat zone gains over family w/ sharing coefs
    double dG_pot;       // sum of (potential) rz+unsat zone gains over family w/o sharing coefs
    double dG_sat_act;   // sum of (actual) sat zone gains over family w/ sharing coefs
    double dG_sat_pot;   // sum of (potential) sat zone gains over family w/ sharing coefs
    //double dG_rz;           // delta of gainers root zone, vol water
    //double dG_un;           // delta of gainers unsat zone, vol water

    /*--------------------------------------------------------------*/
    /*	Loop through patch families in the zone   	                */
    /*--------------------------------------------------------------*/
    for (pf = 0; pf < zone[0].num_patch_families; pf++)
    {
        if (command_line[0].verbose_flag == -6)
            printf("\n--- Patch Family %d ---\n", zone[0].patch_families[pf][0].family_ID);

        /*--------------------------------------------------------------*/
        /*	Patch family definitions & initializations                  */
        /*--------------------------------------------------------------*/
        p_ct = zone[0].patch_families[pf][0].num_patches_in_fam; // for simplicity

        /* Definitions */
        int skip[p_ct];          // 0 = skip, 1 = lose, 2 = gain
        double ksat[p_ct];       // saturated conductivity - from ksat_z_curve()
        double rz_z_pct[p_ct];   // root zone depth percentage (of total)
        double un_z_pct[p_ct];   // unsat zone depth percentage (of total)
        double dL[p_ct];         // loses of water from rz+unsat from patch, vol water
        double dL_sat[p_ct];     // loses of water from sat from patch, vol water
        double dG[p_ct];         // gains of water from rz+unsat from patch, vol water
        double dG_sat[p_ct];     // gains of water from sat from patch, vol water
        double dL_adj[p_ct];     // adjustment to dL based on difference between dL_act and dG_act, vol water
        double dL_sat_adj[p_ct]; // adjustment to dL_sat based on difference between dL_sat_act and dG_sat_act, vol water

        /* Initializations */
        p_ct_skip = 0;
        wet_mean = 0;
        wet_mean_sat = 0;
        area_sum = 0;
        wp_mean = 0;

        dL_act = 0;
        dL_pot = 0;
        dL_sat_act = 0;
        dL_sat_pot = 0;

        dG_act = 0;
        dG_pot = 0;
        dG_sat_act = 0;
        dG_sat_pot = 0;
        area_sum_g = 0;

        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get mean wetness - root+unsat, sat	                */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Pre-transfer ||\n");
            
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            /* Initializations */
            zone[0].patch_families[pf][0].patches[i][0].rz_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].sat_transfer = 0;

            // if both sh coefficients are not 0, include patch
            if ((zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g > 0 ||
                 zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l > 0) &
                zone[0].patch_families[pf][0].num_patches_in_fam > 1)
            {
                // if sharing coefs > 0, include this patch in subsiquent analyses
                skip[i] = 1;

                if (command_line[0].verbose_flag == -6)
                {
                    printf("ID %d | Area %f | RZ stor %f | UNSAT stor %f | Satdef %f | RZ FieldCap %f | ", zone[0].patch_families[pf][0].patches[i][0].ID,
                           zone[0].patch_families[pf][0].patches[i][0].area, zone[0].patch_families[pf][0].patches[i][0].rz_storage,
                           zone[0].patch_families[pf][0].patches[i][0].unsat_storage,
                           zone[0].patch_families[pf][0].patches[i][0].sat_deficit,
                           zone[0].patch_families[pf][0].patches[i][0].rootzone.field_capacity);
                }
                // incrament mean wetness based on storage (rz+unsat or sat) * area
                wet_mean += (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) * 
                zone[0].patch_families[pf][0].patches[i][0].area;
                wet_mean_sat += zone[0].patch_families[pf][0].patches[i][0].sat_deficit * zone[0].patch_families[pf][0].patches[i][0].area;

                if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].msr_sat_transfer_flag > 0)
                {
                    // get ksat -- using sat_deficit_z since that (or rootzone.depth) is what's used as input to compute_unsat_zone_drainage elsewhere
                    ksat[i] = Ksat_z_curve(
                        command_line[0].verbose_flag,
                        zone[0].patch_families[pf][0].patches[i][0].m,
                        zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z,
                        zone[0].patch_families[pf][0].patches[i][0].soil_defaults[0][0].Ksat_0);
                    /* don't share sat if water table at soil depth */
                    if ((zone[0].patch_families[pf][0].patches[i][0].soil_defaults[0][0].soil_depth - zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z) < ZERO)
                        ksat[i] = 0.0;
                    if (command_line[0].verbose_flag == -6)
                        printf("ksat  %f | m %f | satdefz %f | ksat0 %f |",
                               ksat[i],
                               zone[0].patch_families[pf][0].patches[i][0].m,
                               zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z,
                               zone[0].patch_families[pf][0].patches[i][0].soil_defaults[0][0].Ksat_0);
                } /* end msr_sat_transfer_flag */

                // area sum (patch fam without skipped patches)
                area_sum += zone[0].patch_families[pf][0].patches[i][0].area;

                // patch count (without skipped patches)
                p_ct_skip += 1;

                // percent of depth that is root zone
                if (zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z > ZERO)
                    rz_z_pct[i] = max(1.0, zone[0].patch_families[pf][0].patches[i][0].rootzone.depth / zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z);
                else
                    rz_z_pct[i] = 1.0;
                // unsat depth pct
                un_z_pct[i] = 1 - rz_z_pct[i];
                if (command_line[0].verbose_flag == -6) printf("\nRZ/UNSAT z pct %f/%f | ",  rz_z_pct[i], un_z_pct[i]);
            }
            else
            {
                // sharing coefs are 0, skip this patch in this and subsiquent routing loops
                skip[i] = 0;
            }
        } // end loop 1

        // Get mean wetness - vol water/(total patch family) area - units are meters depth
        if (area_sum > ZERO)
        {
            wet_mean /= area_sum;
            wet_mean_sat /= area_sum;
        }
        else
        {
            wet_mean = 0.0;
            wet_mean_sat = 0.0;
        }

        if (command_line[0].verbose_flag == -6)
            printf("Mean wetness (z) = %f,\n", wet_mean);

        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Losing (>mean) Patches ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            if (command_line[0].verbose_flag == -6) 
                printf("ID %d |", zone[0].patch_families[pf][0].patches[i][0].ID);
            // if - no skip and rz + unsat is > mean (losers)
            if (skip[i] > 0 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) > wet_mean)
            {
                dL[i] = ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) - wet_mean) *
                        zone[0].patch_families[pf][0].patches[i][0].area * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l;
                dL_act += dL[i];
                dL_pot += ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) - wet_mean) *
                          zone[0].patch_families[pf][0].patches[i][0].area;
                if (command_line[0].verbose_flag == -6)
                    printf("[z]%f ", dL[i] / zone[0].patch_families[pf][0].patches[i][0].area);
                if (command_line[0].verbose_flag == -6)
                    printf("[v]%f ", dL[i]);
            }
            else if (skip[i] > 0 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) < wet_mean)
            {
                // is a gaining patch
                skip[i] = 2;
                dL[i] = 0;
            }
            else
            {
                dL[i] = 0;
            }
            // sat losers
            dL_sat[i] = 0;
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].msr_sat_transfer_flag > 0)
            {
                if (skip[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit < wet_mean_sat)
                {
                    dL_sat[i] = (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit) * zone[0].patch_families[pf][0].patches[i][0].area * ksat[i];
                    dL_sat_act += dL_sat[i];
                    dL_sat_pot += (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit) * zone[0].patch_families[pf][0].patches[i][0].area;
                }
            }
            if (command_line[0].verbose_flag == -6) 
                printf("<skip> %d \n", skip[i]);
        } // end loop 2
        if (command_line[0].verbose_flag == -6)
            printf("Loses: [act]%f [pot]%f ", dL_act, dL_pot);

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        if (command_line[0].verbose_flag == -6)
            printf("\n|| Gaining (<mean) Patches ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            // rz + unsat < mean wetness (gainers)
            if (skip[i] == 2 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) < wet_mean)
            {
                if (command_line[0].verbose_flag == -6) 
                    printf("ID %d", zone[0].patch_families[pf][0].patches[i][0].ID);

                dG[i] = (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage)) *
                        zone[0].patch_families[pf][0].patches[i][0].area * (dL_act / dL_pot) * zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g *
                        rz_z_pct[i];
                dG_pot += (wet_mean - (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage)) *
                          zone[0].patch_families[pf][0].patches[i][0].area;
                if (command_line[0].verbose_flag == -6)
                    printf("[rz+un v]%f ", dG[i]);

                // Division of gaining water (dG) between rz & unsat
                // rz gain
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = min((dG[i] / zone[0].patch_families[pf][0].patches[i][0].area),
                                                                              (zone[0].patch_families[pf][0].patches[i][0].rootzone.field_capacity - 
                                                                              zone[0].patch_families[pf][0].patches[i][0].rz_storage));
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;
                if (command_line[0].verbose_flag == -6)
                    printf("[rz z]%f ", zone[0].patch_families[pf][0].patches[i][0].rz_transfer);

                // unsat gain - dG * unsat depth pct + max of 0 and dG_rz - rz field capacity
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = max(min((dG[i] / zone[0].patch_families[pf][0].patches[i][0].area) -
                                                                                         zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                                                                                     (zone[0].patch_families[pf][0].patches[i][0].field_capacity - 
                                                                                     zone[0].patch_families[pf][0].patches[i][0].unsat_storage)),
                                                                                 0);
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;
                if (command_line[0].verbose_flag == -6)
                    printf("[unsat z]%f \n", zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);

                dG_act += (zone[0].patch_families[pf][0].patches[i][0].unsat_transfer + zone[0].patch_families[pf][0].patches[i][0].rz_transfer) *
                          zone[0].patch_families[pf][0].patches[i][0].area;

                // wilting point mean (of gainers)
                // wp_mean += (zone[0].patch_families[pf][0].patches[i][0].wilting_point) * zone[0].patch_families[pf][0].patches[i][0].area;
                // incrament gainer area
                area_sum_g += zone[0].patch_families[pf][0].patches[i][0].area;
            }
            else
            {
                dG[i] = 0;
            }
            // sat gainers
            dG_sat[i] = 0; // idk if this is needed, since the actual patch object value is updated above
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].msr_sat_transfer_flag > 0)
            {
                if (skip[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit > wet_mean_sat && dL_sat_pot > ZERO)
                {
                    dG_sat[i] = (zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat) * zone[0].patch_families[pf][0].patches[i][0].area *
                                (dL_sat_act / dL_sat_pot) * ksat[i];
                    dG_sat_act += dG_sat[i];
                    dG_sat_pot += (zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat) * zone[0].patch_families[pf][0].patches[i][0].area;
                    // change in sat store ***** revisit this, make sure deficit is being updated correctly, should be in meters of water *****
                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer = dG_sat[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                    zone[0].patch_families[pf][0].patches[i][0].sat_deficit -= zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
                }
            } /* end msr_sat_transfer_flag */
        }     // end loop 3

        // Get (area) mean wilting point of gainers
 /*        wp_mean /= area_sum_g;
        if (command_line[0].verbose_flag == -6)
            printf("Mean gainers wilting point %f\n", wp_mean); */

        /*--------------------------------------------------------------*/
        /*	loop 4, loop through and reallocate for losing patches   	*/
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Adjusted Losing (>mean) Patches ||\n");

        // if loss>gain, allocating proportionately based on area*delta
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            if (skip[i] == 1 && (zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) > wet_mean)
            {
                if (command_line[0].verbose_flag == -6) 
                    printf("ID %d", zone[0].patch_families[pf][0].patches[i][0].ID);
                // update loss values for gains less than potential, should have no impact if L actual = G actual
                if (dL_act > ZERO)
                    dL_adj[i] = dL[i] - (dL_act - dG_act) * (dL[i] / dL_act);
                else
                    dL_adj[i] = 0.0;

                if (command_line[0].verbose_flag == -6)
                    printf("[rz+un v]%f, ", dL_adj[i]);

                // distribute dL_adj between rz and unsat and update stores
                // RZ - removes down to the mean wilting point of the gaining patches
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = -1 * min(dL_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area,
                                                                                   zone[0].patch_families[pf][0].patches[i][0].rz_storage - 
                                                                                   zone[0].patch_families[pf][0].patches[i][0].wilting_point);
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;
                if (command_line[0].verbose_flag == -6)
                    printf("[rz z]%f ", zone[0].patch_families[pf][0].patches[i][0].rz_transfer);

                // Unsat - removes remainder (if any) thar shouldve been taken from root zone
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = -1 * max((dL_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area) +
                                                                                          zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                                                                                      0);
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;
                if (command_line[0].verbose_flag == -6)
                    printf("[unsat z]%f \n", zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);
            }
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].msr_sat_transfer_flag > 0)
            {
                if (skip[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit > wet_mean_sat && dL_sat_act > ZERO)
                {
                    dL_sat_adj[i] = dL_sat[i] - (dL_sat_act - dG_sat_act) * (dL_sat[i] / dL_sat_act);

                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer = -dL_sat_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                    zone[0].patch_families[pf][0].patches[i][0].sat_deficit += zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
                }
            } /* end if msr_sat_transfer_flag */
        }     // end loop 4
        //if (command_line[0].verbose_flag == -6) printf("\n");

        /*--------------------------------------------------------------*/
        /*	Testing -_-                                              	*/
        /*--------------------------------------------------------------*/
        /*
        double rz_unsat_transfer_sum;   // vol
        double sat_transfer_sum;        // vol
        rz_unsat_transfer_sum = 0;
        sat_transfer_sum = 0;
       
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
        {
            rz_unsat_transfer_sum += (zone[0].patch_families[pf][0].patches[i][0].rz_transfer + zone[0].patch_families[pf][0].patches[i][0].unsat_transfer) * 
                zone[0].patch_families[pf][0].patches[i][0].area;

            sat_transfer_sum += zone[0].patch_families[pf][0].patches[i][0].sat_transfer * zone[0].patch_families[pf][0].patches[i][0].area;
        }

        if (rz_unsat_transfer_sum != 0)
        {
            printf("\n===== Transfer Balance Error =====\nroot + unsat transfer sum = %f\n", rz_unsat_transfer_sum);
            printf("rz transfer     unsat transfer\n");
            for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
            {
                printf("%f          %f\n",
                    zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                    zone[0].patch_families[pf][0].patches[i][0].unsat_transfer
                    );
            }

            printf("==============================\n");
        }
        */

    } // end patch family loop

    return;
}
