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
    //double wet_mean;     // mean wetness for rz+unsat, meters water
    //double wet_mean_sat; // mean wetness for sat zone, meters water
    double area_sum;     // sum of areas in patch family
    double area_sum_g;   // sum of gaining patches area


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
        p_ct = zone[0].patch_families[pf][0].num_patches_in_fam; // for simplicity line46

        /* Definitions */
        int skip[p_ct];          // 0 = skip, 1 = lose, 2 = gain
        int no_veg_patch[p_ct];
        double dL_c1[p_ct], dL_c2[p_ct], dL_c3[p_ct], dL_c4[p_ct];
        double dL_n1[p_ct], dL_n2[p_ct], dL_n3[p_ct], dL_n4[p_ct];
        double dG_c[p_ct];
        double dG_n[p_ct];

        int p_ct_skip;
        int p_no_veg;
        int area_sum;
        int area_sum_g; //area sum of gain patches
        int area_sum_l1, area_sum_l2, area_sum_l3, area_sum_l4; // area sum of loss patches
        int area_sum_g_act; //area sum of actual gain patches
        double dL_c_act1, dL_c_act2, dL_c_act3, dL_c_act4;
        //double dL_c_pot;
        double dL_n_act1, dL_n_act2, dL_n_act3, dL_n_act4;
        //double dL_n_pot;
        double dG_c_act; // gaining actual carbon
        double dG_c_pot;
        double dG_n_act;
        double dG_n_pot;

        //double litter_c_transfer;
        //double litter_n_transfer;
        double litter_c1_adjust, litter_c2_adjust, litter_c3_adjust, litter_c4_adjust;
        double litter_n1_adjust, litter_n2_adjust, litter_n3_adjust, litter_n4_adjust;
        double litr1c_mean, litr2c_mean, litr3c_mean, litr4c_mean;
        double litr1n_mean, litr2n_mean, litr3n_mean, litr4n_mean;
        double patch_area;
        int loss_patch;
        double litter_c1_adjust_total, litter_c2_adjust_total, litter_c3_adjust_total, litter_c4_adjust_total;
        double litter_n1_adjust_total, litter_n2_adjust_total, litter_n3_adjust_total, litter_n4_adjust_total;
        double rooting_depth_mean;

        struct  patch_object            *patches;
        /* Initializations */
        skip[p_ct];          // 0 = skip, 1 = lose, 2 = gain
        no_veg_patch[p_ct];
        dL_c1[p_ct], dL_c2[p_ct], dL_c3[p_ct], dL_c4[p_ct];// litter carbob loss
        dL_n1[p_ct], dL_n2[p_ct], dL_n3[p_ct], dL_n4[p_ct];
        dG_c[p_ct];
        dG_n[p_ct];

        p_ct_skip = 0;
        p_no_veg = 0;
        area_sum = 0;
        area_sum_g = 0;
        area_sum_g_act = 0;
        area_sum_l1 = 0, area_sum_l2 = 0, area_sum_l3 = 0, area_sum_l4 = 0;
        dL_c_act1 = 0, dL_c_act2 = 0, dL_c_act3 = 0, dL_c_act4 = 0;
       // dL_c_pot = 0;
        dL_n_act1 = 0, dL_n_act2 = 0, dL_n_act3 = 0, dL_n_act4 = 0;
       // dL_n_pot = 0;
        dG_c_act = 0;
        dG_n_act = 0;
        dG_c_pot = 0;
        dG_n_pot = 0;


        //litter_c_transfer = 0;
        //litter_n_transfer = 0;
        litter_c1_adjust = 0, litter_c2_adjust = 0, litter_c3_adjust =0, litter_c4_adjust = 0;
        litter_n1_adjust = 0, litter_n2_adjust = 0, litter_n3_adjust =0, litter_n4_adjust = 0;
        litr1c_mean = 0, litr2c_mean = 0, litr3c_mean = 0, litr4c_mean = 0;
        litr1n_mean = 0, litr2n_mean = 0, litr3n_mean = 0, litr4n_mean = 0;
        patch_area = 0;
        loss_patch = 0;
        litter_c1_adjust_total = 0.0, litter_c2_adjust_total = 0, litter_c3_adjust_total =0, litter_c4_adjust_total = 0;
        litter_n1_adjust_total = 0.0, litter_n2_adjust_total = 0, litter_n3_adjust_total =0, litter_n4_adjust_total = 0;
        rooting_depth_mean = 0.0;


        /*--------------------------------------------------------------*/
        /*	Loop 1 - Get litter for each patch family                */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Pre-litter-transfer ||\n");



        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {
            /* Initializations */
           // litter_c_transfer = 0; // do i need 1, 2, 3,4 pool
            // litter_n_transfer = 0;
            patches = zone[0].patch_families[pf][0].patches[i];

            // if both sh coefficients are not 0, include patch
            if (zone[0].patch_families[pf][0].num_patches_in_fam > 1)
            {
                // if sharing coefs > 0, include this patch in subsiquent analyses
                skip1[i] = 1;
                skip2[i] = 1;
                skip3[i] = 1;
                skip4[i] = 1;

                if (command_line[0].verbose_flag == -6)
                {
                    printf("\n||ID %d | Area %f | litr1c %f | litr1n %f ",
                           patches[0].ID,
                           patches[0].area,
                           patches[0].litter_cs.litr1c,
                           patches[0].litter_ns.litr1n);
                }
                // incrament mean wetness based on storage (rz+unsat or sat) * area
                litr1c_mean += patches[0].litter_cs.litr1c * patches[0].area;
                litr1n_mean += patches[0].litter_ns.litr1n * patches[0].area;
                // litter2c need initialize todo
                litr2c_mean += patches[0].litter_cs.litr2c * patches[0].area;
                litr2n_mean += patches[0].litter_ns.litr2n * patches[0].area;
                // litter3c todo
                litr3c_mean += patches[0].litter_cs.litr3c * patches[0].area;
                litr3n_mean += patches[0].litter_ns.litr3n * patches[0].area;
                // litter4c todo
                litr4c_mean += patches[0].litter_cs.litr4c * patches[0].area;
                litr4n_mean += patches[0].litter_ns.litr4n * patches[0].area;

                //for mineralization if no-veg patches use mean_rootdepth instead of zero
                rooting_depth_mean += patches[0].rootzone.depth * patches[0].area;
                // area sum (patch fam without skipped patches)
                area_sum += patches[0].area;

                // patch count (without skipped patches)
                p_ct_skip += 1;
                // count no veg patches
                if (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type == NON_VEG){


                        // do I need to save the ID which one is no veg patches? create a vevtor save i?
                        no_veg_patch[p_no_veg] = patches[0].ID;
                        p_no_veg +=1; //count how many no veg patches
                        // incrament gainer area
                        area_sum_g += patches[0].area;
                         if (command_line[0].verbose_flag == -6) {printf("\n No veg patches are %d | ", patches[0].ID);}
                    }

            }
            else
            {
                // sharing coefs are 0, skip this patch in this and subsiquent routing loops
                skip1[i] = 0;
                skip2[i] = 0;
                skip3[i] = 0;
                skip4[i] = 0;

            }
        } // end loop 1

        // Get mean wetness - vol water/(total patch family) area - units are meters depth
        if (area_sum > ZERO)
        {
            litr1c_mean /= area_sum, litr2c_mean /= area_sum, litr3c_mean /= area_sum, litr4c_mean /= area_sum;
            litr1n_mean /= area_sum, litr2n_mean /= area_sum, litr3n_mean /= area_sum, litr4n_mean /= area_sum;
            rooting_depth_mean /= area_sum;
        }
        else
        {
            litr1c_mean = 0.0, litr2c_mean = 0.0, litr3c_mean = 0.0, litr4c_mean = 0.0;
            litr1n_mean = 0.0, litr2n_mean = 0.0, litr3n_mean = 0.0, litr4n_mean = 0.0;
            rooting_depth_mean = 0.0;
        }

        if (command_line[0].verbose_flag == -6){
            printf("\n**|| Before redistribution Mean litter1c = %f|| Mean litter1n = %f \n", litr1c_mean, litr1n_mean);
            printf("\n**|| Before redistribution Mean litter2c = %f|| Mean litter2n = %f \n", litr2c_mean, litr2n_mean);
            printf("\n**|| Before redistribution Mean litter3c = %f|| Mean litter3n = %f \n", litr3c_mean, litr3n_mean);
            printf("\n**|| Before redistribution Mean litter4c = %f|| Mean litter4n = %f \n", litr4c_mean, litr4n_mean);
            printf("|| Mean root depth is %f \n", rooting_depth_mean);}

        /*--------------------------------------------------------------*/
        /*  loop 2, loop through losing (>mean) patches                 */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("|| Losing litter (>mean) Patches ||\n");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {

            patches = zone[0].patch_families[pf][0].patches[i];

            if (command_line[0].verbose_flag == -6)
                printf("ID %d |", patches[0].ID);
            /* litter 1 pool loop */
            // if - no skip, patches are veg patches, and have litter >zer0
            if (skip1[i] > 0 && (patches[0].litter_cs.litr1c) > ZERO &&
                 (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 (p_ct_skip > p_no_veg) && patches[0].litter_cs.litr1c > litr1c_mean &&
                  patches[0].litter_ns.litr1n > litr1n_mean)
            {
                loss_patch1 += 1;  //todo
                patch_area = patches[0].area;

                dL_c1[i] = (patches[0].litter_cs.litr1c - litr1c_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_c_act1 += dL_c1[i];// this is important to track the total litter

                // litter 1 n
                dL_n1[i] = (patches[0].litter_ns.litr1n - litr1n_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_n_act1 += dL_n1[i]; // this is important to track the total litter


                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter1 mean]%f ", dL_c1[i] / patch_area);
                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter1 sum]%f || [total cumulative losing is sum %f]", dL_c1[i], dL_c_act1);

                //remove litter from these patches
                // make them to be mean
                patches[0].litter_cs.litr1c = litr1c_mean;
                patches[0].litter_ns.litr1n = litr1n_mean;
                area_sum_l1 += patches[0].area;

            }

            else if (skip1[i] > 0 && (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                // is a gaining patch
                skip1[i] = 2; // do I need 1 ,2,3, 4 for skip
                dL_c1[i] = 0; // 1, 2, 3, 4
                dL_n1[i] = 0;
                // here set no veg patch a mean root depth for mineralization
                patches[0].rooting_depth_mean = rooting_depth_mean;
            }
            else
            {
                dL_c1[i] = 0;
                dL_n1[i] = 0;
            } // end of litter1

        /* litter 2 */
            if (skip2[i] > 0 && (patches[0].litter_cs.litr2c) > ZERO &&
                 (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 (p_ct_skip > p_no_veg) && patches[0].litter_cs.litr2c > litr2c_mean &&  // p_ct_skip number of total patches > p_no_veg number of no veg patches, mean not all are no-veg
                  patches[0].litter_ns.litr2n > litr2n_mean)
            {
                loss_patch2 += 1; //todo
                patch_area = patches[0].area;

                dL_c2[i] = (patches[0].litter_cs.litr2c - litr2c_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_c_act2 += dL_c2[i];// this is important to track the total litter

                // litter 1 n
                dL_n2[i] = (patches[0].litter_ns.litr2n - litr2n_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_n_act2 += dL_n2[i]; // this is important to track the total litter


                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter2 mean]%f ", dL_c2[i] / patch_area);
                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter2 sum]%f || [total cumulative losing is sum %f]", dL_c2[i], dL_c_act2);

                //remove litter from these patches
                // make them to be mean
                patches[0].litter_cs.litr2c = litr2c_mean;
                patches[0].litter_ns.litr2n = litr2n_mean;
                area_sum_l2 += patches[0].area;

            }

            else if (skip2[i] > 0 && (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                // is a gaining patch
                skip2[i] = 2; // to do
                dL_c2[i] = 0; // 1, 2, 3, 4
                dL_n2[i] = 0;
                // here set no veg patch a mean root depth for mineralization
                //patches[0].rooting_depth_mean = rooting_depth_mean;
            }
            else
            {
                dL_c2[i] = 0;
                dL_n2[i] = 0;
            } // end of litter2


        /* litter 3 */
            if (skip3[i] > 0 && (patches[0].litter_cs.litr3c) > ZERO &&
                 (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 (p_ct_skip > p_no_veg) && patches[0].litter_cs.litr3c > litr3c_mean &&  // p_ct_skip number of total patches > p_no_veg number of no veg patches, mean not all are no-veg
                  patches[0].litter_ns.litr3n > litr3n_mean)
            {
                loss_patch3 += 1; //todo
                patch_area = patches[0].area;

                dL_c3[i] = (patches[0].litter_cs.litr3c - litr3c_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_c_act3 += dL_c3[i];// this is important to track the total litter

                // litter 1 n
                dL_n3[i] = (patches[0].litter_ns.litr3n - litr3n_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_n_act3 += dL_n3[i]; // this is important to track the total litter


                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter3 mean]%f ", dL_c3[i] / patch_area);
                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter3 sum]%f || [total cumulative losing is sum %f]", dL_c3[i], dL_c_act3);

                //remove litter from these patches
                // make them to be mean
                patches[0].litter_cs.litr3c = litr3c_mean;
                patches[0].litter_ns.litr3n = litr3n_mean;
                area_sum_l3 += patches[0].area;

            }

            else if (skip3[i] > 0 && (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                // is a gaining patch
                skip3[i] = 2; // to do
                dL_c3[i] = 0; // 1, 2, 3, 4
                dL_n3[i] = 0;
                // here set no veg patch a mean root depth for mineralization
                //patches[0].rooting_depth_mean = rooting_depth_mean;
            }
            else
            {
                dL_c3[i] = 0;
                dL_n3[i] = 0;
            } // end of litter3

        /* litter 4 */
            if (skip4[i] > 0 && (patches[0].litter_cs.litr4c) > ZERO &&
                 (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 (p_ct_skip > p_no_veg) && patches[0].litter_cs.litr4c > litr4c_mean &&  // p_ct_skip number of total patches > p_no_veg number of no veg patches, mean not all are no-veg
                  patches[0].litter_ns.litr4n > litr4n_mean)
            {
                loss_patch4 += 1; //todo
                patch_area = patches[0].area;

                dL_c4[i] = (patches[0].litter_cs.litr4c - litr4c_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_c_act4 += dL_c4[i];// this is important to track the total litter

                // litter 4n
                dL_n4[i] = (patches[0].litter_ns.litr4n - litr4n_mean) * patch_area;// make sure p_ct_skip > p_no_veg
                         //* zone[0].defaults[0][0].sh_litter;//only veg patch lose litter
                dL_n_act4 += dL_n4[i]; // this is important to track the total litter


                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter4 mean]%f ", dL_c4[i] / patch_area);
                if (command_line[0].verbose_flag == -6)
                    printf("||[losing litter4 sum]%f || [total cumulative losing is sum %f]", dL_c4[i], dL_c_act4);

                //remove litter from these patches
                // make them to be mean
                patches[0].litter_cs.litr4c = litr4c_mean;
                patches[0].litter_ns.litr4n = litr4n_mean;
                area_sum_l4 += patches[0].area;

            }

            else if (skip4[i] > 0 && (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG)
            {
                // is a gaining patch
                skip4[i] = 2; // to do
                dL_c4[i] = 0; // 1, 2, 3, 4
                dL_n4[i] = 0;
                // here set no veg patch a mean root depth for mineralization
                //patches[0].rooting_depth_mean = rooting_depth_mean;
            }
            else
            {
                dL_c4[i] = 0;
                dL_n4[i] = 0;
            } // end of litter4


        } // end loop 2
        if (command_line[0].verbose_flag == -6)
            printf("\n ||Litter 1C Loses: [act]%f, Litter 1N loses: [act]%f", dL_c_act1, dL_n_act1);
            printf("\n ||Litter 2C Loses: [act]%f, Litter 2N loses: [act]%f", dL_c_act2, dL_n_act2);
            printf("\n ||Litter 3C Loses: [act]%f, Litter 3N loses: [act]%f", dL_c_act3, dL_n_act3);
            printf("\n ||Litter 4C Loses: [act]%f, Litter 4N loses: [act]%f", dL_c_act4, dL_n_act4);

        /*--------------------------------------------------------------*/
        /*  loop 3, loop through gaining (<mean) patches              	*/
        /*--------------------------------------------------------------*/

        if (command_line[0].verbose_flag == -6)
            printf("\n|| Litter Gaining (No-veg) Patches || ");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {

            patches = zone[0].patch_families[pf][0].patches[i];
            patch_area = patches[0].area;
            // route the litter to no-veg patches(gainers)
            if (skip[i] == 2 &&
                (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) == NON_VEG &&
                 patches[0].litter_cs.litr1c < litr1c_mean &&
                 patches[0].litter_ns.litr1n < litr1n_mean)
            {
                if (command_line[0].verbose_flag == -6)
                    printf("\n || Litter gaining ID %d", patches[0].ID);
                // the gain is equally distributed among no-veg patches
                // what if one patch has litter > mean, so the number of gain patches is different from # of no-veg patches

                dG_c[i] = dL_c_act /area_sum_g * patch_area * zone[0].defaults[0][0].sh_litter;// this the mean for each patch
                dG_c_act += dG_c[i];
                dG_c_pot +=  dL_c_act /area_sum_g * patch_area;

                dG_n[i] = dL_n_act/area_sum_g * patch_area  * zone[0].defaults[0][0].sh_litter;
                dG_n_act += dG_n[i];
                dG_n_pot += dL_n_act/area_sum_g * patch_area ;


                if (command_line[0].verbose_flag == -6){
                    printf("\n||[actual litter Gain]%f \n", dG_c[i]);
                    printf("|| [cumulative litter Gain]%f \n", dG_c_act);
                    printf("|| [before add litter] the litr1c is %f \n", patches[0].litter_cs.litr1c);}

                // litter carbon gain
                //litter_c_transfer = dG_c[i] / patch_area;
                patches[0].litter_cs.litr1c += (dG_c[i] / patch_area);
                //litter_n_transfer = dG_n[i] / patch_area;
                patches[0].litter_ns.litr1n += (dG_n[i] / patch_area);


                if (command_line[0].verbose_flag == -6){
                    printf("\n||[litter c gain]%f ", (dG_c[i] / patch_area));
                    printf("\n||[after add litter], [the litr1c is] %f, [the amount of litter is transfered] %f\n", patches[0].litter_cs.litr1c, (dG_c[i] / patch_area));}

                 // here needs to check if the gaining patches have litter more than mean, then adjust it

                 if (patches[0].litter_cs.litr1c > litr1c_mean ||
                     patches[0].litter_ns.litr1n > litr1n_mean ){


                     litter_c_adjust_total += (patches[0].litter_cs.litr1c - litr1c_mean)*patch_area;
                     litter_n_adjust_total += (patches[0].litter_ns.litr1n - litr1n_mean)*patch_area;
                     // make the no-veg gain patches litter mean
                     patches[0].litter_cs.litr1c = litr1c_mean;
                     patches[0].litter_ns.litr1n = litr1n_mean;

                    }

                  //if the act gain is smaller than potential gain due to parameters, adjust it too ??
                  if (dG_c_pot > dG_c_act && dG_n_pot > dG_n_act)  {

                  litter_c_adjust_total += dG_c_pot - dG_c_act;
                  litter_n_adjust_total += dG_n_pot - dG_n_act;

                  }

                // incrament gainer area
                area_sum_g_act += patches[0].area;
            }// end skip==2 and no veg
            else
            {
                dG_c[i] = 0;
                dG_n[i] = 0;
            }

        }     // end loop 3


        /*--------------------------------------------------------------*/
        /* Loop 4                                                       */
        /*--------------------------------------------------------------*/
        if (command_line[0].verbose_flag == -6)
            printf("\n|| Adjust litter gain patches loop 4|| ");

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {

            patches = zone[0].patch_families[pf][0].patches[i];
            patch_area = patches[0].area;
            // adjust the litter to no-veg patches(gainers)
            if (skip[i] == 1 &&
                (patches[0].canopy_strata[0][0].defaults[0][0].epc.veg_type) != NON_VEG &&
                 patches[0].litter_cs.litr1c >= litr1c_mean &&
                 patches[0].litter_ns.litr1n >= litr1n_mean)
            {
                if (command_line[0].verbose_flag == -6)
                    printf("\n ID %d", patches[0].ID);
                // the gain is equally distributed among no-veg patches

                litter_c_adjust = litter_c_adjust_total/area_sum_l* patch_area;
                litter_n_adjust = litter_n_adjust_total/area_sum_l* patch_area;


                if (command_line[0].verbose_flag == -6){
                    printf("\n||loop4 adjust [total litter adjust*area]%f \n", litter_c_adjust_total);
                    printf("|| [litter c adjust for this patch]%f \n", litter_c_adjust);
                    printf("|| [before adjust litter] the litr1c is %f \n",patches[0].litter_cs.litr1c);
                    }

                // litter carbon gain
                //litter_c_transfer = litter_c_adjust / patch_area;
                patches[0].litter_cs.litr1c += (litter_c_adjust / patch_area);
               // litter_n_transfer = litter_n_adjust / patch_area;
                patches[0].litter_ns.litr1n += (litter_n_adjust / patch_area);




                }
            }

        /*--------------------------------------------------------------*/
        /*	Testing -_-                                              	*/
        /*--------------------------------------------------------------*/
                    /* Initializations */
            litr1c_mean = 0; // do i need 1, 2, 3,4 pool
            litr1n_mean = 0;
            area_sum = 0;  //Important, ths initialization should be outside of the loop!

        for (i = 0; i < zone[0].patch_families[pf][0].num_patches_in_fam; i++)
        {


            // if both sh coefficients are not 0, include patch
            if (zone[0].patch_families[pf][0].num_patches_in_fam > 1)
            {

                patches = zone[0].patch_families[pf][0].patches[i];
                if (command_line[0].verbose_flag == -6)
                {
                    printf("\n ||After redistribute litter: ID %d | Area %f | litr1c %f | litr1n %f ",
                           patches[0].ID,
                           patches[0].area,
                           patches[0].litter_cs.litr1c,
                           patches[0].litter_ns.litr1n);
                }
                // incrament mean wetness based on storage (rz+unsat or sat) * area
                litr1c_mean += patches[0].litter_cs.litr1c * patches[0].area;
                litr1n_mean += patches[0].litter_ns.litr1n * patches[0].area;


                // area sum (patch fam without skipped patches)
                area_sum += patches[0].area;



            }

        } // end loop 1

        // Get mean wetness - vol water/(total patch family) area - units are meters depth
        if (area_sum > ZERO)
        {
            litr1c_mean /= area_sum;
            litr1n_mean /= area_sum;
            rooting_depth_mean /= area_sum;
        }
        else
        {
            litr1c_mean = 0.0;
            litr1n_mean = 0.0;
            rooting_depth_mean = 0.0;
        }

        if (command_line[0].verbose_flag == -6){
            printf("\n**||After lateral redistribution Mean litter1c = %f|| Mean litter1n = %f \n", litr1c_mean, litr1n_mean);
            printf("\n ||Mean root depth is %f \n", rooting_depth_mean);}
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
