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
                                  struct command_line_object *command_line,
		                          struct date current_date)
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
    int p_ct_incl_sat, p_ct_incl_unsat;       // number of patches in patch family without skipped patches
    double wet_mean_unsat;     // mean wetness for rz+unsat, meters water
    double wet_mean_sat; // mean wetness for sat zone, meters water
    double area_sum_sat, area_sum_unsat;     // sum of areas in patch family
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
    double rz_trans, unsat_trans; //intermediate vars for rz and unsat transfer
    double sm_mean, area_sum, sm_sum;

// for testing
// if (current_date.year == 2011 && current_date.month == 3 && current_date.day == 22) {
//     command_line[0].verbose_flag = -6;
// } else {
//     command_line[0].verbose_flag = 0;
// }

    /*--------------------------------------------------------------*/
    /*	Loop through patch families in the zone   	                */
    /*--------------------------------------------------------------*/
    for (pf = 0; pf < zone[0].num_patch_families; pf++)
    {
        if (command_line[0].verbose_flag == -6) {printf("\n--- Patch Family %d ---\n", zone[0].patch_families[pf][0].family_ID);}

        /*--------------------------------------------------------------*/
        /*	Patch family definitions & initializations                  */
        /*--------------------------------------------------------------*/
        p_ct = zone[0].patch_families[pf][0].num_patches_in_fam; // for simplicity

        /* Definitions */
        int incl_sat[p_ct];          // 0 = skip, 1 = lose, 2 = gain
        int incl_unsat[p_ct];          // 0 = incl, 1 = include, for if sat_deficit is 0
        double ksat[p_ct];       // saturated conductivity - from ksat_z_curve()
        double rz_z_pct[p_ct];   // root zone depth percentage (of total)
        double un_z_pct[p_ct];   // unsat zone depth percentage (of total)
        double dL[p_ct];         // loses of water from rz+unsat from patch, vol water
        double dL_sat[p_ct];     // loses of water from sat from patch, vol water
        double dG[p_ct];         // gains of water from rz+unsat from patch, vol water
        double dG_sat[p_ct];     // gains of water from sat from patch, vol water
        double dL_adj[p_ct];     // adjustment to dL based on difference between dL_act and dG_act, vol water
        double dL_sat_adj[p_ct]; // adjustment to dL_sat based on difference between dL_sat_act and dG_sat_act, vol water
        double wet_unsat[p_ct];  // wetnes in unsat - rz+unsat/satdef
        struct  patch_object            *patches;

        /* Initializations */
        p_ct_incl_sat = 0;
        p_ct_incl_unsat = 0;
        wet_mean_unsat = 0;
        wet_mean_sat = 0;
        area_sum_sat = 0;
        area_sum_unsat = 0;
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

        area_sum = 0.0;
        sm_sum = 0.0;
        sm_mean = 0.0;

        /*--------------------------------------------------------------*/
        /* calculate the mean soil moisture of veg patches              */
        /*--------------------------------------------------------------*/

      /*  for (i =0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            patches = zone[0].patch_families[pf][0].patches[i];

            if (patches[0].sat_deficit > 0.1 && patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot != 1)
            {

                sm_sum += patches[0].theta * patches[0].area;
                area_sum += patches[0].area;

            }

        }
        if (area_sum > ZERO)
        {
            sm_mean = sm_sum/area_sum;

        }
        else
        {
            sm_mean = 0.0;
        } */

        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get mean wetness - root+unsat, sat	                */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
        {
            printf("|| Pre-transfer ||\n|  ID  | Include Unsat | Sat |   Area   |   RZ storage   |  UNSAT storage |    Wetness     |     Satdef     |  RZ FieldCap   | RZ z pct | UNSAT z pct|\n");
        }

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            patches = zone[0].patch_families[pf][0].patches[i];
            /* add diff_threshold to test the sensitivity of hotspot */
            // if soil moisture of no hotspot > diff_th;  wet season, MSR off
            // if soil moisture of no hotspot < diff_th;  dry season, MSR on
            // here the rain_threshold is for no hotspot patches

            //printf("\n [ID %d], [theta, %lf], [diff_th %lf]", patches[0].ID, patches[0].theta, patches[0].soil_defaults[0][0].diff_th);
            //TH is lower, most of the time is off; TH is higher, on all the time ; default is on the time
            //patch daily F, line 614, patch[0].rain_throughfall = zone[0].rain + irrigation; unit is m
            //update drainage land line 420 control MSR on or off; wet period, MSR is off
            if (current_date.month == 5 && current_date.day <=12 && patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1) { //dry season must on to rout water
                zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g = 0.05; //make it small but not that small, no MSR makes hotspot always saturated
                zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l = 0.9; // dry season mainly lose water very fast, gain water slow to make hotspot dry too
                //printf("\n MSR on during dry season [ID %d], [year %d], [month %d]", patches[0].ID, current_date.year, current_date.month);
            }
            else if (current_date.month == 10 && current_date.day <=3 && patches[0].theta > 0.5 && patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1){

               zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g = 0.05; //make it small but not that small, no MSR makes hotspot always saturated
               zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l = 0.9;
               printf("\n MSR on during Oct [ID %d], [year %d], [month %d]", patches[0].ID, current_date.year, current_date.month);
            }

           /*else if (sm_mean > patches[0].soil_defaults[0][0].diff_th  &&
                patches[0].sat_deficit > 1 &&
                patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1)  // when there is precipitation, MSR off, when no precip, dry, MSR on, hotspot dry too
            { // wet period, MSR is off
            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g = 0.9; //make it small but not that small, no MSR makes hotspot always saturated
            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l = 0.0001; // if no hotspot no loss no gain, the hotspot no gain no loss too
             //printf("\n MSR off during wet season [ID %d], [sm_mean %lf]", patches[0].ID, sm_mean);
            } */
            else if (patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1 && patches[0].sat_deficit > 1) { //other time disconnected and hotspot accumulates water but not reach surface

            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g = 0.9; //
            zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l = 0.00001; //

            }
            else if (patches[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1 && patches[0].sat_deficit <= 1) {//if water table reach surface release water

                zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g = 0.005; //make it small but not that small, no MSR makes hotspot always saturated
                zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l = 0.9;

            }

            /* Initializations */
            zone[0].patch_families[pf][0].patches[i][0].rz_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = 0;
            zone[0].patch_families[pf][0].patches[i][0].sat_transfer = 0;

            // check if sat deficit is >0
            if (zone[0].patch_families[pf][0].patches[i][0].sat_deficit > ZERO)
            {
                incl_unsat[i] = 1;
            }
            else
            {
                incl_unsat[i] = 0;
            }

            // if both sh coefficients are not 0, include patch
            if ((zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g > 0 ||
                 zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l > 0) &
                 zone[0].patch_families[pf][0].num_patches_in_fam > 1 )
            {
                // if sharing coefs > 0, include this patch in subsiquent analyses
                incl_sat[i] = 1;

                /* for unsat routing only include patches with sat def greater than 0 (incl_unsat > 0) */
                if (incl_unsat[i] > 0)
                {
                    // incrament mean wetness based on storage (rz+unsat or sat) * area
                    wet_unsat[i] = ((zone[0].patch_families[pf][0].patches[i][0].rz_storage + zone[0].patch_families[pf][0].patches[i][0].unsat_storage) /
                        zone[0].patch_families[pf][0].patches[i][0].sat_deficit);
                    wet_mean_unsat += wet_unsat[i] * zone[0].patch_families[pf][0].patches[i][0].area;
                    // area sum (patch fam without skipped patches)
                    area_sum_unsat += zone[0].patch_families[pf][0].patches[i][0].area;
                    p_ct_incl_unsat += 1;
                }

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

                } /* end msr_sat_transfer_flag */

                // area sum (patch fam without skipped patches)
                area_sum_sat += zone[0].patch_families[pf][0].patches[i][0].area;

                // patch count (without skipped patches)
                p_ct_incl_sat += 1;

                // percent of depth that is root zone
                if (zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z > ZERO)
                {
                    rz_z_pct[i] = min(1.0, zone[0].patch_families[pf][0].patches[i][0].rootzone.depth / zone[0].patch_families[pf][0].patches[i][0].sat_deficit_z);
                }
                else
                {
                    rz_z_pct[i] = 1.0;
                }
                // unsat depth pct
                un_z_pct[i] = 1 - rz_z_pct[i];

            }
            else
            {
                // sharing coefs are 0, don't include this patch in this and subsiquent routing loops
                incl_unsat[i] = 0;
                incl_sat[i] = 0;
            }

            if (command_line[0].verbose_flag == -6)
            {
                      //|   ID   | Include Unsat - Sat |  Area  |  RZ storage  | UNSAT storage |   Satdef   | RZ FieldCap | RZ z pct | UNSAT z pct |
                printf("| %4d |  %12d | %3d | %6.4f | %2.12f | %2.12f | %2.12f | %2.12f | %2.12f | %8f | %11f |\n",
                       zone[0].patch_families[pf][0].patches[i][0].ID,
                       incl_unsat[i],
                       incl_sat[i],
                       zone[0].patch_families[pf][0].patches[i][0].area,
                       zone[0].patch_families[pf][0].patches[i][0].rz_storage,
                       zone[0].patch_families[pf][0].patches[i][0].unsat_storage,
                       wet_unsat[i],
                       zone[0].patch_families[pf][0].patches[i][0].sat_deficit,
                       zone[0].patch_families[pf][0].patches[i][0].rootzone.field_capacity,
                       rz_z_pct[i],
                       un_z_pct[i]);
            }

        } // end loop 1

        // Get mean wetness - vol water/(total patch family) area - units are meters depth
        if (area_sum_unsat > ZERO) {
            wet_mean_unsat /= area_sum_unsat;
        }
        else {
            wet_mean_unsat = 0.0;
        }
        if (area_sum_sat > ZERO) {
            wet_mean_sat /= area_sum_sat;
        }
        else {
            wet_mean_sat = 0.0;
        }

        if (command_line[0].verbose_flag == -6)
        {
            printf("| Mean wetness (z/satdef) | UNSAT: %f | SAT: %f |\n", wet_mean_unsat, wet_mean_sat);
            printf("|| Losing ( > mean) Patches ||\n");
            printf("|  ID  | Include Unsat |   Area   | Est.Loses (m3) |\n");
        }

        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            // if - included and rz + unsat is > mean (losers)
            if ( incl_unsat[i] > 0 && (wet_unsat[i] - wet_mean_unsat) > ZERO)
            {
                dL[i] = (wet_unsat[i] - wet_mean_unsat) * zone[0].patch_families[pf][0].patches[i][0].area *
                zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_l * zone[0].patch_families[pf][0].patches[i][0].sat_deficit;
                dL_act += dL[i];
                dL_pot += (wet_unsat[i] - wet_mean_unsat) * zone[0].patch_families[pf][0].patches[i][0].area * zone[0].patch_families[pf][0].patches[i][0].sat_deficit;
            }
            else if (incl_unsat[i] > 0 && (wet_unsat[i] - wet_mean_unsat) < ZERO)
            {
                // is a gaining patch
                incl_unsat[i] = 2;
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
                if (incl_sat[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat < ZERO)
                {
                    dL_sat[i] = (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit) * zone[0].patch_families[pf][0].patches[i][0].area * ksat[i];
                    dL_sat_act += dL_sat[i];
                    dL_sat_pot += (wet_mean_sat - zone[0].patch_families[pf][0].patches[i][0].sat_deficit) * zone[0].patch_families[pf][0].patches[i][0].area;
                }
            }


            if (command_line[0].verbose_flag == -6)
            {
                printf("| %4d |  %12d | %6.4f | %2.12f |\n",
                       zone[0].patch_families[pf][0].patches[i][0].ID,
                       incl_unsat[i],
                       zone[0].patch_families[pf][0].patches[i][0].area,
                       dL[i]);
            }

        } // end loop 2

        if (command_line[0].verbose_flag == -6)
        {
            printf("| Total Estimated Losses | Actual: %f | Potential: %f |\n", dL_act, dL_pot);
            printf("|| Gaining ( < mean) Patches ||\n");
            printf("|  ID  | Include Unsat |   Area   |   Gains (m3)   |    RZ Gains    |  UNSAT Gains   |\n");
        }

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            // rz + unsat < mean wetness (gainers)
            if (incl_unsat[i] == 2 && dL_pot > ZERO)
            {
                dG[i] = (wet_mean_unsat - wet_unsat[i]) * zone[0].patch_families[pf][0].patches[i][0].area * (dL_act / dL_pot) *
                    zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].sh_g * rz_z_pct[i] * zone[0].patch_families[pf][0].patches[i][0].sat_deficit;

                dG_pot += (wet_mean_unsat - wet_unsat[i]) * zone[0].patch_families[pf][0].patches[i][0].area * zone[0].patch_families[pf][0].patches[i][0].sat_deficit;

                // Division of gaining water (dG) between rz & unsat
                // rz gain
                rz_trans = min((dG[i] / zone[0].patch_families[pf][0].patches[i][0].area),
                               (zone[0].patch_families[pf][0].patches[i][0].rootzone.field_capacity -
                                zone[0].patch_families[pf][0].patches[i][0].rz_storage));
                // if (fabs(rz_trans) < ZERO) {
                //     rz_trans = 0;
                // }
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = rz_trans;
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;

                // unsat gain - dG * unsat depth pct + max of 0 and dG_rz - rz field capacity
                unsat_trans = max(min((dG[i] / zone[0].patch_families[pf][0].patches[i][0].area) -
                                          zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                                      (zone[0].patch_families[pf][0].patches[i][0].field_capacity -
                                       zone[0].patch_families[pf][0].patches[i][0].unsat_storage)),
                                  0);
                // if (fabs(unsat_trans) < ZERO) {
                //     unsat_trans = 0;
                // }
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = unsat_trans;
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;

                dG_act += (zone[0].patch_families[pf][0].patches[i][0].unsat_transfer + zone[0].patch_families[pf][0].patches[i][0].rz_transfer) *
                          zone[0].patch_families[pf][0].patches[i][0].area;

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
                if (incl_sat[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat > ZERO && dL_sat_pot > ZERO)
                {
                    dG_sat[i] = (zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat) * zone[0].patch_families[pf][0].patches[i][0].area *
                                (dL_sat_act / dL_sat_pot) * min(ksat[i],1.0);
                    dG_sat_act += dG_sat[i];
                    dG_sat_pot += (zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat) * zone[0].patch_families[pf][0].patches[i][0].area;
                    // change in sat store ***** revisit this, make sure deficit is being updated correctly, should be in meters of water *****
                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer = dG_sat[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                    zone[0].patch_families[pf][0].patches[i][0].sat_deficit -= zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
                }
            } /* end msr_sat_transfer_flag */

            if (command_line[0].verbose_flag == -6)
            {
                printf("| %4d |  %12d | %6.4f | %2.12f | %2.12f | %2.12f |\n",
                       zone[0].patch_families[pf][0].patches[i][0].ID,
                       incl_unsat[i],
                       zone[0].patch_families[pf][0].patches[i][0].area,
                       dG[i],
                       zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                       zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);
            }

        }     // end loop 3

        if (command_line[0].verbose_flag == -6)
        {
            printf("| Total Gains | Actual: %f | Potential: %f |\n", dG_act, dG_pot);
            printf("|| Adjusted Losing ( > mean) Patches ||\n");
            printf("|  ID  | Include Unsat |   Area   |  Loses Adjust  |    RZ Loses    |  UNSAT Loses   |\n");
        }

        /*--------------------------------------------------------------*/
        /*	loop 4, loop through and reallocate for losing patches   	*/
        /*--------------------------------------------------------------*/

        // if loss>gain, allocating proportionately based on area*delta
        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            dL_adj[i] = 0.0;

            if (incl_unsat[i] == 1) //
            {
                // update loss values for gains less than potential, should have no impact if L actual = G actual
                if (dL_act > ZERO)
                {
                    dL_adj[i] = dL[i] - (dL_act - dG_act) * (dL[i] / dL_act);
                }

                // distribute dL_adj between rz and unsat and update stores
                // RZ - removes down to the mean wilting point of the gaining patches
                rz_trans = -1 * min(dL_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area,
                                    zone[0].patch_families[pf][0].patches[i][0].rz_storage -
                                        zone[0].patch_families[pf][0].patches[i][0].wilting_point);
                // if (fabs(rz_trans) < ZERO)
                // {
                //     rz_trans = 0;
                // }
                zone[0].patch_families[pf][0].patches[i][0].rz_transfer = rz_trans;
                zone[0].patch_families[pf][0].patches[i][0].rz_storage += zone[0].patch_families[pf][0].patches[i][0].rz_transfer;

                // Unsat - removes remainder (if any) thar shouldve been taken from root zone
                unsat_trans = -1 * max((dL_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area) + zone[0].patch_families[pf][0].patches[i][0].rz_transfer, 0);
                // if (fabs(unsat_trans) < ZERO)
                // {
                //     unsat_trans = 0;
                // }
                zone[0].patch_families[pf][0].patches[i][0].unsat_transfer = unsat_trans;
                zone[0].patch_families[pf][0].patches[i][0].unsat_storage += zone[0].patch_families[pf][0].patches[i][0].unsat_transfer;

            }
            if (zone[0].patch_families[pf][0].patches[i][0].landuse_defaults[0][0].msr_sat_transfer_flag > 0)
            {
                if (incl_sat[i] > 0 && zone[0].patch_families[pf][0].patches[i][0].sat_deficit - wet_mean_sat < ZERO && dL_sat_act > ZERO)
                {
                    dL_sat_adj[i] = dL_sat[i] - (dL_sat_act - dG_sat_act) * (dL_sat[i] / dL_sat_act);

                    zone[0].patch_families[pf][0].patches[i][0].sat_transfer = -dL_sat_adj[i] / zone[0].patch_families[pf][0].patches[i][0].area;
                    zone[0].patch_families[pf][0].patches[i][0].sat_deficit += zone[0].patch_families[pf][0].patches[i][0].sat_transfer;
                }
            } /* end if msr_sat_transfer_flag */

            if (command_line[0].verbose_flag == -6)
            {
                printf("| %4d |  %12d | %6.4f | %2.12f | %2.12f | %2.12f |\n",
                       zone[0].patch_families[pf][0].patches[i][0].ID,
                       incl_unsat[i],
                       zone[0].patch_families[pf][0].patches[i][0].area,
                       dL_adj[i],
                       zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                       zone[0].patch_families[pf][0].patches[i][0].unsat_transfer);
            }

        }     // end loop 4

        /*--------------------------------------------------------------*/
        /*	Testing -_-                                              	*/
        /*--------------------------------------------------------------*/
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



        if (fabs(rz_unsat_transfer_sum) > ZERO)
        {
            printf("\n===== Transfer Balance Error =====\n");
            printf("Date: %d - %d - %d || ", current_date.year, current_date.month, current_date.day);
            printf("root + unsat transfer sum = %.14f\n", rz_unsat_transfer_sum);
            printf("ID       area            rz transfer           unsat transfer          total_vol\n");
            for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam ; i++)
            {
                printf("%d     %.8f    %.14f      %.14f      %.14f\n",
                    zone[0].patch_families[pf][0].patches[i][0].ID,
                    zone[0].patch_families[pf][0].patches[i][0].area,
                    zone[0].patch_families[pf][0].patches[i][0].rz_transfer,
                    zone[0].patch_families[pf][0].patches[i][0].unsat_transfer,
                    (zone[0].patch_families[pf][0].patches[i][0].rz_transfer + zone[0].patch_families[pf][0].patches[i][0].unsat_transfer) *
                    zone[0].patch_families[pf][0].patches[i][0].area
                    );
            }

            printf("==============================\n");
        }

    } // end patch family loop

    return;
}
