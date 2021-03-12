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

void compute_patch_family_litter_routing(struct zone_object *zone,
                                  struct command_line_object *command_line)
{



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
        int no_veg_patch[p_ct];
        double dL_c[p_ct]; // litter carbob loss
        double dL_n[p_ct]; // litter nitrogen loss
        double dG_c[p_ct];
        double dG_n[p_ct];

        int p_ct_skip;
        int p_no_veg;
        int area_sum;
        int area_sum_g;
        double dL_c_act;
        double dL_c_pot;
        double dL_n_act;
        double dL_n_pot;
        double dG_c_act;
        double dG_c_pot;
        double dG_n_act;
        double dG_n_pot;

        double litter_c_transfer;
        double litter_n_transfer;
        double litter_c_adjust;
        double litter_n_adjust;
        double litr1c_mean;
        double litr1n_mean;

        /* Initializations */
        skip[p_ct];          // 0 = skip, 1 = lose, 2 = gain
        no_veg_patch[p_ct];
        dL_c[p_ct]; // litter carbob loss
        dL_n[p_ct]; // litter nitrogen loss
        dG_c[p_ct];
        dG_n[p_ct];

        p_ct_skip = 0;
        p_no_veg = 0;
        area_sum = 0;
        area_sum_g = 0;
        dL_c_act = 0;
        dL_c_pot = 0;
        dL_n_act = 0;
        dL_n_pot = 0;
        litter_c_transfer = 0;
        litter_n_transfer = 0;
        litter_c_adjust = 0;
        litter_n_adjust = 0;
        litr1c_mean = 0;
        litr1n_mean = 0;


        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get mean wetness - root+unsat, sat	                */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Pre-litter-transfer ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            /* Initializations */
            litter_c_transfer = 0;
            litter_n_transfer = 0;


            // if both sh coefficients are not 0, include patch
            if (zone[0].patch_families[pf][0].num_patches_in_fam > 1)
            {
                // if sharing coefs > 0, include this patch in subsiquent analyses
                skip[i] = 1;

                if (command_line[0].verbose_flag == -6)
                {
                    printf("ID %d | Area %f | litr1c %f | litr1n %f ",
                           zone[0].patch_families[pf][0].patches[i][0].ID,
                           zone[0].patch_families[pf][0].patches[i][0].area,
                           zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c,
                           zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n);
                }
                // incrament mean wetness based on storage (rz+unsat or sat) * area
                litr1c_mean += zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c * zone[0].patch_families[pf][0].patches[i][0].area;
                litr1n_mean += zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n * zone[0].patch_families[pf][0].patches[i][0].area;



                // area sum (patch fam without skipped patches)
                area_sum += zone[0].patch_families[pf][0].patches[i][0].area;

                // patch count (without skipped patches)
                p_ct_skip += 1;
                // count no veg patches
                if (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].defaults[0][0].epc.veg_type == NON_VEG){


                        // do I need to save the ID which one is no veg patches? create a vevtor save i?
                        no_veg_patch[p_no_veg] = zone[0].patch_families[pf][0].patches[i][0].ID;
                        p_no_veg +=1; //count how many no veg patches
                         if (command_line[0].verbose_flag == -6) {printf("\n No veg patches are %d | ", zone[0].patch_families[pf][0].patches[i][0].ID);}
                    }

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
            litr1c_mean /= area_sum;
            litr1n_mean /= area_sum;
        }
        else
        {
            litr1c_mean = 0.0;
            litr1n_mean = 0.0;
        }

        if (command_line[0].verbose_flag == -6)
            printf("Mean litter1c = %f|| Mean litter1n = %f \n", litr1c_mean, litr1n_mean);

        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Losing litter (>mean) Patches ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            if (command_line[0].verbose_flag == -6)
                printf("ID %d |", zone[0].patch_families[pf][0].patches[i][0].ID);
            // if - no skip, patches are veg patches, and have litter >zer0
            if (skip[i] > 0 && (zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c) > ZERO &&
                 (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 (p_ct_skip > p_no_veg))
            {
                dL_c[i] = litr1c_mean*p_no_veg/(p_ct_skip - p_no_veg) *// make sure p_ct_skip > p_no_veg
                        zone[0].patch_families[pf][0].patches[i][0].area * zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_c_act += dL_c[i];
                dL_c_pot += litr1c_mean*p_no_veg/(p_ct_skip - p_no_veg) *// make sure p_ct_skip > p_no_veg
                        zone[0].patch_families[pf][0].patches[i][0].area;

                // litter 1 n
                dL_n[i] = litr1n_mean*p_no_veg/(p_ct_skip - p_no_veg) *// make sure p_ct_skip > p_no_veg
                        zone[0].patch_families[pf][0].patches[i][0].area * zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_n_act += dL_n[i];
                dL_n_pot += litr1n_mean*p_no_veg/(p_ct_skip - p_no_veg) *// make sure p_ct_skip > p_no_veg
                        zone[0].patch_families[pf][0].patches[i][0].area;

                if (command_line[0].verbose_flag == -6)
                    printf("[losing litter mean]%f ", dL_c[i] / zone[0].patch_families[pf][0].patches[i][0].area);
                if (command_line[0].verbose_flag == -6)
                    printf("[losing litter sum]%f ", dL_c[i]);

                //remove litter from these patches

                litter_c_transfer = dL_c[i];
                zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c -= litter_c_transfer;
                litter_n_transfer = dL_n[i];
                zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n -= litter_n_transfer;

                // if the patch doesn't have enough litter to share, then skip this patch
                if( zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c < ZERO ||
                    zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n < ZERO ){
                     zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c += litter_c_transfer;//add it back
                     zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n += litter_n_transfer;
                     litter_c_adjust += litter_c_transfer;
                     litter_n_adjust += litter_n_transfer;

                    }


            }
            else if (skip[i] > 0 && (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                // is a gaining patch
                skip[i] = 2;
                dL_c[i] = 0;
                dL_n[i] = 0;
            }
            else
            {
                dL_c[i] = 0;
                dL_n[i] = 0;
            }

        } // end loop 2
        if (command_line[0].verbose_flag == -6)
            printf("Litter C Loses: [act]%f [pot]%f ", dL_c_act, dL_c_pot);

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        if (command_line[0].verbose_flag == -6)
            printf("\n|| Litter Gaining (No-veg) Patches ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            // route the litter to no-veg patches(gainers)
            if (skip[i] == 2 &&
                (zone[0].patch_families[pf][0].patches[i][0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                if (command_line[0].verbose_flag == -6)
                    printf("ID %d", zone[0].patch_families[pf][0].patches[i][0].ID);

                dG_c[i] = litr1c_mean * // this the mean for each patch
                        zone[0].patch_families[pf][0].patches[i][0].area *
                        zone[0].defaults[0][0].sh_litter;

                dG_c_pot += litr1c_mean * // this the mean for each patch
                        zone[0].patch_families[pf][0].patches[i][0].area ;

                dG_n[i] = litr1n_mean * // this the mean for each patch
                        zone[0].patch_families[pf][0].patches[i][0].area *
                        zone[0].defaults[0][0].sh_litter;

                dG_n_pot += litr1n_mean * // this the mean for each patch
                        zone[0].patch_families[pf][0].patches[i][0].area ;



                if (command_line[0].verbose_flag == -6)
                    printf("||[actual litter Gain]%f \n", dG_c[i]);

                // Division of gaining water (dG) between rz & unsat
                // litter carbon gain
                litter_c_transfer = dG_c[i] - litter_c_adjust/p_no_veg; // do I need to consider area transformation
                zone[0].patch_families[pf][0].patches[i][0].litter_cs.litr1c += litter_c_transfer;
                litter_n_transfer = dG_n[i] - litter_n_adjust/p_no_veg;
                zone[0].patch_families[pf][0].patches[i][0].litter_ns.litr1n += litter_n_transfer;


                if (command_line[0].verbose_flag == -6)
                    printf("[litter c gain]%f ", litter_c_transfer);


                // incrament gainer area
                area_sum_g += zone[0].patch_families[pf][0].patches[i][0].area;
            }// end skip==2 and no veg
            else
            {
                dG_c[i] = 0;
                dG_n[i] = 0;
            }

        }     // end loop 3


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
