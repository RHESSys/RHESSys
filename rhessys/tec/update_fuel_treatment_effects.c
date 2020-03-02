/*--------------------------------------------------------------*/
/*					                                            */
/*		        update_fuel_treatment_effects      		        */
/*					                                            */
/*	update_fuel_treatment_effects.c - exectures fuel treatments */
/*					                                            */
/*	NAME						                                */
/*	update_fuel_treatment_effects.c - exectures fuel treatments */
/*					                                            */
/*	SYNOPSIS			                                        */
/* void update_fuel_treatment_effects(                          */
/*                  struct zone_object *zone,                   */
/*                  struct command_line_object *command_line)   */
/*										                        */
/*	OPTIONS								                    	*/
/*										                        */
/*										                        */
/*	DESCRIPTION								                    */
/*  Executes a fuels treatment basesd on internal (salience)    */
/*  or external treatment probabilities, and performs the       */
/*	treatment with the appropriate method for multiscale or     */
/*	standard patches					                        */
/*										                        */
/*	PROGRAMMER NOTES							                */
/*										                        */
/*	Feb, 2020 William Burke 						            */
/*											                    */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

void update_fuel_treatment_effects(struct zone_object *zone,
                                   struct command_line_object *command_line)
{
    /*--------------------------------------------------------------*/
    /*	Local function definition.	                          	    */
    /*--------------------------------------------------------------*/

    void	update_mortality(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		int,
		struct mortality_struct);


    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                        */
    /*--------------------------------------------------------------*/
    int p, pf, s;
    int trt, notrt;
    int gain, lose;
    int thintyp; 
    double comb_area; // combined treated and untreated areas
    double gain_area_add;
    double gain_area_old;
    double lose_pct_chg;
    double max_trt_prob;
    double new_trt_area;
    double new_notrt_area;
    double notrt_area;
    double r; // random 0-1
    double thr; // treatment threshold
    double tmp;
    double trt_int; // 0-1 treatment intensity
    double trt_area;
    bool stoch;

    struct	canopy_strata_object	*stratum;
    //struct	patch_family_object *patch_family;
    //struct	patch_object	*patch;
    struct mortality_struct mort;

    /*--------------------------------------------------------------*/
    /*	Local variable initialization   	                        */
    /*--------------------------------------------------------------*/
    r = rand()/(RAND_MAX+1.0);
    
    mort.mort_cpool = 0.0;
	mort.mort_leafc = 0.0;
	mort.mort_deadleafc = 0.0;
	mort.mort_livestemc = 0.0;
	mort.mort_deadstemc = 0.0;
	mort.mort_livecrootc = 0.0;
	mort.mort_deadcrootc = 0.0;
	mort.mort_frootc = 0.0;

    // these should be pars
    stoch = FALSE;
    thr =  0.5;
    thintyp = 1;

    // whats thintyp - should be a param
    // "redefine_world_thin_remain" 1
    // "redefine_world_thin_harvest" 2
    // "redefine_world_thin_snags"  3

    /*--------------------------------------------------------------*/
    /*	Multiscale treatment method                                 */
    /*--------------------------------------------------------------*/
    if (command_line[0].multiscale_flag == 1)
    {
        for (pf = 0; pf < zone[0].num_patch_families; pf++)
        {
            /*--------------------------------------------------------------*/
            /*	Check treatment probabilities                               */
            /*--------------------------------------------------------------*/
            trt = -1;
            notrt = -1;
            max_trt_prob = -1;
            
            for (p = 0; p < zone[0].patch_families[pf][0].num_patches_in_fam; p++)
            {
                // if patch has a treat prob > 0, check if its the treated or untreated (ignore the gap)
                if (zone[0].patch_families[pf][0].patches[p][0].fuel_treatment.effective_fuel_treatment_prob > 0 || 
                zone[0].patch_families[pf][0].patches[p][0].fuel_treatment.external_fuel_treatment_prob > 0)
                {
                    // try to build in flexibility to allow multiple of each TREATED or UNTREATED, and areas of them can be scaled appropriately
                    // later..
                    // could do this when initing the patch family obj by adding seperate pointer(s) to the treated and untreated patches
                    if (zone[0].patch_families[pf][0].patches[p][0].family_role == "TREATED") {
                        trt = p;
                        // taking the probs from the treated patch specifically, though it shouldn't matter
                        // im assuming we want to use the max of these two? could sum them maybe to allow for some interactions, could be confusing
                        max_trt_prob = max(zone[0].patch_families[pf][0].patches[trt][0].fuel_treatment.effective_fuel_treatment_prob, 
                            zone[0].patch_families[pf][0].patches[trt][0].fuel_treatment.external_fuel_treatment_prob);
                    } 
                    else if (zone[0].patch_families[pf][0].patches[p][0].family_role == "UNTREATED") {
                        notrt = p;
                    }
                }
            } // end patches in fam loop

            /*--------------------------------------------------------------*/
            /*	Test/check if treatment occurs                              */
            /*--------------------------------------------------------------*/
            if (stoch && max_trt_prob > r) {
                // stochastic option
                // currently will assume there are only 1 TREATED and 1 UNTREATED patch

                // transform intensity to appropriate patch coverage/area change - IF ITS NOT ALREADY THE RIGHT SIZE

                // with this method the intesity for over and understory has to be the same - using overstory
                trt_int = zone[0].patch_families[pf][0].patches[trt][0].fuel_treatment.fuel_treatment_intensity.overstory;

                zone[0].patch_families[pf][0].patches[trt][0].fuel_treatment.salience_prob;
                // IF salience prob 1-9 > som threshold area, -> some static probability

                // ignoring the gap area since it's not really treatable
                trt_area = zone[0].patch_families[pf][0].patches[trt][0].area;
                notrt_area = zone[0].patch_families[pf][0].patches[notrt][0].area;
                comb_area = trt_area + notrt_area;

                // check if areas need to be changed - change and update stores if so
                if (trt_area/comb_area != trt_int) {
                    new_trt_area = trt_int * comb_area;
                    new_notrt_area = comb_area - trt_area;

                    // change could be in either direction
                    if (new_trt_area > trt_area) {
                        // treated area is growing
                        gain = trt;
                        lose = notrt;
                        gain_area_old = zone[0].patch_families[pf][0].patches[gain][0].area;
                        gain_area_add = new_trt_area - gain_area_old;
                        // if there's any vol stores to adjust, would need to change loser stores too
                        //lose_pct_chg = (notrt_area - new_notrt_area) / notrt_area;
                    } else {
                        // untreated area is growing
                        gain  = notrt;
                        lose = trt;
                        gain_area_old = zone[0].patch_families[pf][0].patches[gain][0].area;
                        gain_area_add = new_notrt_area - gain_area_old;
                        //lose_pct_chg = (trt_area - new_trt_area) / trt_area;
                    }

                    // gamma change?

                    // stores to change

                    // example - assuming it's a store with depth or length units
                    // loser has no change
                    // gainer gets a weighted mean: old area * old store + additional area * store of the loser / new area
                    zone[0].patch_families[pf][0].patches[gain][0].rz_storage = (zone[0].patch_families[pf][0].patches[gain][0].rz_storage * gain_area_old) + 
                        (zone[0].patch_families[pf][0].patches[lose][0].rz_storage * gain_area_add);

                    // actually change areas
                    zone[0].patch_families[pf][0].patches[trt][0].area = new_trt_area;
                    zone[0].patch_families[pf][0].patches[notrt][0].area = new_notrt_area;
                    

                } // end area change


                // call update mortality - reduce c completely on appropriate patch
                
                // can actually leave these at 0
                mort.mort_cpool = 0.0;
                mort.mort_leafc = 0.0;
                mort.mort_deadleafc = 0.0;
                mort.mort_livestemc = 0.0;
                mort.mort_deadstemc = 0.0;
                mort.mort_livecrootc = 0.0;
                mort.mort_deadcrootc = 0.0;
                mort.mort_frootc = 0.0;

                for (s = 0; s < zone[0].patch_families[pf][0].patches[trt][0].num_canopy_strata; s++) {
                    stratum = zone[0].patch_families[pf][0].patches[trt][0].canopy_strata[s];

                    update_mortality(stratum[0].defaults[0][0].epc,
                        &(stratum[0].cs),
                        &(stratum[0].cdf),
                        &(zone[0].patch_families[pf][0].patches[trt][0].cdf),
                        &(stratum[0].ns),
                        &(stratum[0].ndf),
                        &(zone[0].patch_families[pf][0].patches[trt][0].ndf),
                        &(zone[0].patch_families[pf][0].patches[trt][0].litter_cs),
                        &(zone[0].patch_families[pf][0].patches[trt][0].litter_ns),
                        thintyp,
                        mort);
                }

                // update litter stores

                // update density

                // maybe do other updates like in input_new_strata ? - LAI etc

            } else if (!stoch && max_trt_prob > thr) {
                // non-stochastic/threshold option

            }
            
        }     // end patch fam loop
    }
    else
    {
        /*--------------------------------------------------------------*/
        /*	Standard treatment method                                   */
        /*--------------------------------------------------------------*/
        for (p = 0; p < zone[0].num_patches; p++)
        {
            /*--------------------------------------------------------------*/
            /*	Check treatment probabilities                               */
            /*--------------------------------------------------------------*/


            // zone[0].patches[p][0].fuel_treatment.effective_fuel_treatment_prob;
            // zone[0].patches[p][0].fuel_treatment.external_fuel_treatment_prob;
           

            // call update mortality - use intensity to reduce C

        } // end std patch loop
    }

    return;

} // end function