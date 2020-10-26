/*--------------------------------------------------------------*/
/* 													*/
/*			compute_fire_effects								*/
/*													*/
/*	NAME												*/
/*	compute_fire_effects.c 										*/
/*													*/
/*	SYNOPSIS											*/
/*													*/
/* 													*/
/*													*/
/*	OPTIONS												*/
/*													*/
/*													*/
/*	DESCRIPTION											*/
/*	Determines vegetation loss following fire							*/
/*													*/
/*													*/
/*													*/
/*	PROGRAMMER NOTES										*/
/*													*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

void compute_fire_effects(

						struct patch_object *patch,
						double pspread,
						struct command_line_object *command_line)

{

	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/

	void	update_litter_soil_mortality(
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct soil_c_object *,
		struct soil_n_object *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct fire_litter_soil_loss_struct);

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
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/

	struct canopy_strata_object *canopy_target;
	struct canopy_strata_object *canopy_subtarget;
	struct mortality_struct mort;
	struct fire_litter_soil_loss_struct fire_loss;
	int c, layer;
	int thin_type;
	double litter_c_consumed;

	/*--------------------------------------------------------------*/
	/*	Compute litter and soil removed.			*/
	/*--------------------------------------------------------------*/

	if (pspread > 0){

	/* Litter consumption is approximated based CONSUME model outputs */
	/* Consumption 1hr-fuel = 1 * 1hr-fuel */
	/* Consumption 10hr-fuel = 0.8469 * 10hr-fuel */
	/* Consumption 100hr-fuel = 0.7127 * 100hr-fuel */

	fire_loss.loss_litr1c = 1;
	fire_loss.loss_litr2c = 1;
	fire_loss.loss_litr3c = 0.85;
	fire_loss.loss_litr4c = 0.71;
	fire_loss.loss_soil1c = 0.71;
	fire_loss.loss_soil2c = 0;
	fire_loss.loss_soil3c = 0;
	fire_loss.loss_soil4c = 0;
	fire_loss.loss_litr1n = 1;
	fire_loss.loss_litr2n = 1;
	fire_loss.loss_litr3n = 0.85;
	fire_loss.loss_litr4n = 0.71;
	fire_loss.loss_soil1n = 0.71;
	fire_loss.loss_soil2n = 0;
	fire_loss.loss_soil3n = 0;
	fire_loss.loss_soil4n = 0;

	/* Calculate litter consumed for use later in canopy effects */
	litter_c_consumed = patch[0].litter_cs.litr1c * fire_loss.loss_litr1c +
			patch[0].litter_cs.litr2c * fire_loss.loss_litr2c +
			patch[0].litter_cs.litr3c * fire_loss.loss_litr3c +
			patch[0].litter_cs.litr4c * fire_loss.loss_litr4c;

    patch[0].litterc_burned = litter_c_consumed;//new NREN

	update_litter_soil_mortality(
		 &(patch[0].cdf),
		 &(patch[0].ndf),
		 &(patch[0].soil_cs),
		 &(patch[0].soil_ns),
		 &(patch[0].litter_cs),
		 &(patch[0].litter_ns),
		 fire_loss);

	/*--------------------------------------------------------------*/
	/*		Compute vegetation effects.			*/
	/*--------------------------------------------------------------*/

	/* For each patch that burns (pspread > 0), fire effects is computed
	for each canopy starting with the tallest and proceeding down
	though the canopies. The canopy being evaluated for fire effects for
	any given iteration is referred to as the target canopy. Fire effects
	in the target canopy depend on the height of the target canopy. For
	short target canopies (height < understory_height_thresh), mortality
	is a function of pspread. For tall target canopies (height >
	overstory_height_thresh), fire effects are a function of the litter
	and understory biomass consumed by the fire. In this situation, it
	is necessary to additionally compute mortality and consumption for
	canopies below the target canopy. While in theory the fire effects
	code should account for all understory canopies below target
	canopy, the current code only computes mortality/consumption for next
	lowest canopy. Hence, code may need to be revised if working with more
	than two canopies. */


	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){

			/* Calculates metrics for targer canopy */
			canopy_target = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
			canopy_target[0].fe.canopy_target_height = canopy_target[0].epv.height;

			/* Calculates metrics for next lowest canopy (subtarget canopy) */
			if (patch[0].num_layers > (layer+1)){
				canopy_subtarget = patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])];
				canopy_target[0].fe.canopy_subtarget_height = canopy_subtarget[0].epv.height;

				canopy_target[0].fe.canopy_subtarget_biomassc = canopy_subtarget[0].cs.leafc + canopy_subtarget[0].cs.dead_leafc + // for output
						canopy_subtarget[0].cs.live_stemc + canopy_subtarget[0].cs.dead_stemc +
						canopy_subtarget[0].cs.live_crootc + canopy_subtarget[0].cs.dead_crootc +
						canopy_subtarget[0].cs.frootc + canopy_subtarget[0].cs.cpool;

  				canopy_target[0].fe.canopy_subtarget_c = canopy_subtarget[0].cs.leafc + // for calculating the overstory pburn
						canopy_subtarget[0].cs.live_stemc +
						canopy_subtarget[0].cs.dead_stemc;

                canopy_target[0].fe.canopy_subtarget_leafc = canopy_subtarget[0].cs.leafc + canopy_subtarget[0].cs.dead_leafc;
                canopy_target[0].fe.canopy_subtarget_stemc = canopy_subtarget[0].cs.live_stemc + canopy_subtarget[0].cs.dead_stemc;
                canopy_target[0].fe.canopy_subtarget_rootc = canopy_subtarget[0].cs.live_crootc + canopy_subtarget[0].cs.dead_crootc + canopy_subtarget[0].cs.frootc;

                // add the over story NREN why do i need overstory
                canopy_target[0].fe.canopy_target_biomassc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc +
                    canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc +
                    canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc +
                    canopy_target[0].cs.frootc + canopy_target[0].cs.cpool;


                canopy_target[0].fe.canopy_target_leafc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc;
                canopy_target[0].fe.canopy_target_stemc = canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc;
                canopy_target[0].fe.canopy_target_rootc = canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc + canopy_target[0].cs.frootc;


			} else {
				canopy_target[0].fe.canopy_subtarget_height = 0.0;
				canopy_target[0].fe.canopy_subtarget_c = 0.0;
				canopy_target[0].fe.canopy_subtarget_biomassc = 0.0;
				canopy_target[0].fe.canopy_subtarget_leafc = 0.0;
				canopy_target[0].fe.canopy_subtarget_stemc = 0.0;
				canopy_target[0].fe.canopy_subtarget_rootc = 0.0;

            // add the over story NREN why do i need overstory why not put it in previous section too
                canopy_target[0].fe.canopy_target_biomassc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc +
                    canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc +
                    canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc +
                    canopy_target[0].cs.frootc + canopy_target[0].cs.cpool;


                canopy_target[0].fe.canopy_target_leafc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc;
                canopy_target[0].fe.canopy_target_stemc = canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc;
                canopy_target[0].fe.canopy_target_rootc = canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc + canopy_target[0].cs.frootc;


			}


			/*--------------------------------------------------------------*/
			/* Calculate coarse woody debris removed			*/
			/*--------------------------------------------------------------*/

			/* Litter consumption is approximated based CONSUME model outputs */
			/* Consumption 1000hr-fuel (Mg/ha) = 2.735 + 0.3285 * 1000hr-fuel (Mg/ha) - 0.0457 * Fuel Moisture (e.g 80%) (Original CONSUME eqn) */
			/* Consumption 1000hr-fuel (Mg/ha) = 0.33919 * 1000hr-fuel (Mg/ha) (Modified CONSUME eqn to exclude moisture and have intercept through zero) */
			canopy_target[0].fe.m_cwdc_to_atmos = canopy_target[0].cs.cwdc * .339;
			canopy_target[0].fe.m_cwdn_to_atmos = canopy_target[0].ns.cwdn * .339;
			canopy_target[0].cs.cwdc -= canopy_target[0].fe.m_cwdc_to_atmos;
			canopy_target[0].ns.cwdn -= canopy_target[0].fe.m_cwdn_to_atmos;


			/*--------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is tall			*/
			/*--------------------------------------------------------------*/

			if (canopy_target[0].fe.canopy_target_height > patch[0].soil_defaults[0][0].overstory_height_thresh){

				/* Determine the amount of understory carbon consumed, which is used to */
				/* compute how well fire is propogated to overstory */

				/* Is subtarget canopy tall? */
				if (canopy_target[0].fe.canopy_subtarget_height > patch[0].soil_defaults[0][0].overstory_height_thresh){

					canopy_target[0].fe.understory_c_consumed = litter_c_consumed; // If every canopy > overstory_height_thresh means on understory or understory is litter

				/* Is subtarget canopy of intermediate or short height? Then calculate mortality/consumption of understory */
				} else if (canopy_target[0].fe.canopy_subtarget_height <= patch[0].soil_defaults[0][0].overstory_height_thresh){

					/* Determine the proportion of carbon mortality in the subtarget canopy */
					if (canopy_target[0].defaults[0][0].understory_mort <= 0){
						fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].understory_mort must be greater than 0.\n");
    						exit(EXIT_FAILURE);
					} else if (canopy_target[0].defaults[0][0].understory_mort == 1){
						canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].defaults[0][0].understory_mort * pspread;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort = (pow(canopy_target[0].defaults[0][0].understory_mort,pspread)-1)/(canopy_target[0].defaults[0][0].understory_mort-1);
					}

					/* For intermediate height subtarget canopy, adjust canopy_subtarget_prop_mort to only account for understory component */
					if (canopy_target[0].fe.canopy_subtarget_height <= patch[0].soil_defaults[0][0].overstory_height_thresh && canopy_target[0].fe.canopy_subtarget_height >= patch[0].soil_defaults[0][0].understory_height_thresh){

						/* Determine the proportion of subtarget canopy attributed to understory. Proportion overstory is 1 - canopy_subtarget_height_u_prop */
						canopy_target[0].fe.canopy_subtarget_height_u_prop = (patch[0].soil_defaults[0][0].overstory_height_thresh - canopy_target[0].fe.canopy_subtarget_height)/(patch[0].soil_defaults[0][0].overstory_height_thresh-patch[0].soil_defaults[0][0].understory_height_thresh);

						canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_height_u_prop;
					}

					/* Determine the proportion of subtarget canopy mortality consumed */
					if (canopy_target[0].defaults[0][0].consumption == 1){
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_subtarget_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
					}

					/* Determine the proportion of subtarget canopy carbon consumed */
					canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;

					/* Determine the amount of carbon consumed in the understory (subtarget canopy and litter) */
					canopy_target[0].fe.understory_biomassc_consumed = (canopy_target[0].fe.canopy_subtarget_biomassc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed) + litter_c_consumed;
                    canopy_target[0].fe.understory_c_consumed = (canopy_target[0].fe.canopy_subtarget_c * canopy_target[0].fe.canopy_subtarget_prop_c_consumed) + litter_c_consumed;
					//new outputs
					canopy_target[0].fe.understory_leafc_consumed = canopy_target[0].fe.canopy_subtarget_leafc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					canopy_target[0].fe.understory_stemc_consumed = canopy_target[0].fe.canopy_subtarget_stemc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					canopy_target[0].fe.understory_rootc_consumed = canopy_target[0].fe.canopy_subtarget_rootc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;

					// accumulate understory here?
					if(command_line[0].f !=NULL && command_line[0].output_flags.yearly ==1 ){

                        canopy_target[0].fe.acc_year.m_cwdc_to_atmos += canopy_target[0].fe.m_cwdc_to_atmos;
                        canopy_target[0].fe.acc_year.m_cwdn_to_atmos += canopy_target[0].fe.m_cwdn_to_atmos;

                    // litter
                        canopy_target[0].fe.acc_year.litter_c_consumed += litter_c_consumed; //should be here so not double counts
                    // understory
                        canopy_target[0].fe.acc_year.understory_biomassc_consumed +=  canopy_target[0].fe.understory_biomassc_consumed;// including litter
                        canopy_target[0].fe.acc_year.understory_leafc_consumed += canopy_target[0].fe.understory_leafc_consumed;
                        canopy_target[0].fe.acc_year.understory_stemc_consumed += canopy_target[0].fe.understory_stemc_consumed;
                        canopy_target[0].fe.acc_year.understory_rootc_consumed += canopy_target[0].fe.understory_rootc_consumed;
                        canopy_target[0].fe.acc_year.length_understory +=1;

					}
				}

				/* Determine the proportion of target canopy mortality based on the amount of understory consumed (sigmoidal relationship) */
				canopy_target[0].fe.canopy_target_prop_mort = 1 - (1/(1+exp(-(canopy_target[0].defaults[0][0].overstory_mort_k1*(canopy_target[0].fe.understory_c_consumed - canopy_target[0].defaults[0][0].overstory_mort_k2)))));

				/* Determine the proportion of target canopy mortality consumed */
				if (canopy_target[0].defaults[0][0].consumption <= 0){
					fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].consumption must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}


			/*--------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is an intermediate height	*/
			/*--------------------------------------------------------------*/

			} else if (canopy_target[0].fe.canopy_target_height <= patch[0].soil_defaults[0][0].overstory_height_thresh && canopy_target[0].fe.canopy_target_height >= patch[0].soil_defaults[0][0].understory_height_thresh){

				/* Determine the proportion of target canopy attributed to understory. Proportion overstory is 1 - canopy_target_height_u_prop */
				canopy_target[0].fe.canopy_target_height_u_prop = (patch[0].soil_defaults[0][0].overstory_height_thresh - canopy_target[0].fe.canopy_target_height)/(patch[0].soil_defaults[0][0].overstory_height_thresh-patch[0].soil_defaults[0][0].understory_height_thresh);



				/* ------- Determine mortality/consumption for understory component of target canopy ------- */

				/* This involves computing the mortality/consumption of the target canopy based on pspread. */

				/* Determine the proportion of carbon mortality in the target canopy */
				if (canopy_target[0].defaults[0][0].understory_mort <= 0){
					fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].understory_mort must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_target[0].defaults[0][0].understory_mort == 1){
					canopy_target[0].fe.canopy_target_prop_mort = canopy_target[0].defaults[0][0].understory_mort * pspread;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort = (pow(canopy_target[0].defaults[0][0].understory_mort,pspread)-1)/(canopy_target[0].defaults[0][0].understory_mort-1);
				}

				/* Adjust canopy_target_prop_mort to only account for understory component */
				canopy_target[0].fe.canopy_target_prop_mort_u_component = canopy_target[0].fe.canopy_target_prop_mort * canopy_target[0].fe.canopy_target_height_u_prop;



				/* ------- Determine mortality/consumption for overstory component of target canopy ------- */

				/* This involves computing the consumption of the subtarget canopy, which is used to determine target
				canopy mortality/consumption. */

				/* Determine the proportion of carbon mortality in the subtarget canopy */
				if (canopy_target[0].defaults[0][0].understory_mort <= 0){
					fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].understory_mort must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_target[0].defaults[0][0].understory_mort == 1){
					canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].defaults[0][0].understory_mort * pspread;
				} else {
					canopy_target[0].fe.canopy_subtarget_prop_mort = (pow(canopy_target[0].defaults[0][0].understory_mort,pspread)-1)/(canopy_target[0].defaults[0][0].understory_mort-1);
				}

				/* For intermediate height subtarget canopy, adjust canopy_subtarget_prop_mort to only account for understory component */
				if (canopy_target[0].fe.canopy_subtarget_height <= patch[0].soil_defaults[0][0].overstory_height_thresh && canopy_target[0].fe.canopy_subtarget_height >= patch[0].soil_defaults[0][0].understory_height_thresh){

					/* Determine the proportion of subtarget canopy attributed to understory. Proportion overstory is 1 - canopy_subtarget_height_u_prop */
					canopy_target[0].fe.canopy_subtarget_height_u_prop = (patch[0].soil_defaults[0][0].overstory_height_thresh - canopy_target[0].fe.canopy_subtarget_height)/(patch[0].soil_defaults[0][0].overstory_height_thresh-patch[0].soil_defaults[0][0].understory_height_thresh);

					canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_height_u_prop;
				}

				/* Determine the proportion of subtarget canopy mortality consumed */
				if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
				} else {
					canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_subtarget_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}

				/* Determine the proportion of subtarget canopy carbon consumed */
				canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;

				/* Determine the amount of carbon consumed in the understory (subtarget canopy and litter) */
				//canopy_target[0].fe.understory_c_consumed = (canopy_target[0].fe.canopy_subtarget_c * canopy_target[0].fe.canopy_subtarget_prop_c_consumed) + litter_c_consumed;

                /* Determine the amount of carbon consumed in the understory subtarget canopy */
					canopy_target[0].fe.understory_c_consumed = (canopy_target[0].fe.canopy_subtarget_biomassc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed) + litter_c_consumed;

					//new outputs
					canopy_target[0].fe.understory_leafc_consumed = canopy_target[0].fe.canopy_subtarget_leafc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					canopy_target[0].fe.understory_stemc_consumed = canopy_target[0].fe.canopy_subtarget_stemc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					canopy_target[0].fe.understory_rootc_consumed = canopy_target[0].fe.canopy_subtarget_rootc * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;

					// accumulate understory here?
					if(command_line[0].f !=NULL && command_line[0].output_flags.yearly ==1 ){

                        canopy_target[0].fe.acc_year.m_cwdc_to_atmos += canopy_target[0].fe.m_cwdc_to_atmos;
                        canopy_target[0].fe.acc_year.m_cwdn_to_atmos += canopy_target[0].fe.m_cwdn_to_atmos;

                    // litter
                        canopy_target[0].fe.acc_year.litter_c_consumed += litter_c_consumed; //should be here so not double counts
                    // understory
                        canopy_target[0].fe.acc_year.understory_biomassc_consumed +=  canopy_target[0].fe.understory_biomassc_consumed;// including litter
                        canopy_target[0].fe.acc_year.understory_leafc_consumed += canopy_target[0].fe.understory_leafc_consumed;
                        canopy_target[0].fe.acc_year.understory_stemc_consumed += canopy_target[0].fe.understory_stemc_consumed;
                        canopy_target[0].fe.acc_year.understory_rootc_consumed += canopy_target[0].fe.understory_rootc_consumed;
                        canopy_target[0].fe.acc_year.length_understory +=1;

					}
				/* Determine the proportion of target canopy mortality based on the amount of understory consumed (sigmoidal relationship) and then account for target canopy height allocation */
				canopy_target[0].fe.canopy_target_prop_mort_o_component = (1 - (1/(1+exp(-(canopy_target[0].defaults[0][0].overstory_mort_k1*(canopy_target[0].fe.understory_c_consumed - canopy_target[0].defaults[0][0].overstory_mort_k2)))))) * (1-canopy_target[0].fe.canopy_target_height_u_prop);


				/* ------------------------------------------------------------------------ */
				/* Combine target canopy mortality from overstory and understory components */
				canopy_target[0].fe.canopy_target_prop_mort = max(min(canopy_target[0].fe.canopy_target_prop_mort_u_component + canopy_target[0].fe.canopy_target_prop_mort_o_component,1.0),0);

				/* Determine the proportion of target canopy mortality consumed */
				if (canopy_target[0].defaults[0][0].consumption <= 0){
					fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].consumption must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}


			/*--------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is short			*/
			/*--------------------------------------------------------------*/

			} else if (canopy_target[0].fe.canopy_target_height < patch[0].soil_defaults[0][0].understory_height_thresh) { //line 282

				/* Determine the proportion of carbon mortality in the target canopy */
				if (canopy_target[0].defaults[0][0].understory_mort <= 0){
					fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].understory_mort must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_target[0].defaults[0][0].understory_mort == 1){
					canopy_target[0].fe.canopy_target_prop_mort = canopy_target[0].defaults[0][0].understory_mort * pspread;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort = (pow(canopy_target[0].defaults[0][0].understory_mort,pspread)-1)/(canopy_target[0].defaults[0][0].understory_mort-1);
				}

				/* Determine the proportion of target canopy mortality consumed */
				if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}
			}


			/*--------------------------------------------------------------*/
			/* Compute effects						*/
			/*--------------------------------------------------------------*/

			/* Determine the proportion of total target canopy carbon that is consumed by fire */
			canopy_target[0].fe.canopy_target_prop_c_consumed = canopy_target[0].fe.canopy_target_prop_mort * canopy_target[0].fe.canopy_target_prop_mort_consumed;

			mort.mort_cpool = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_leafc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_deadstemc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_livestemc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_frootc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_deadcrootc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_livecrootc = canopy_target[0].fe.canopy_target_prop_c_consumed;
			mort.mort_deadleafc = canopy_target[0].fe.canopy_target_prop_c_consumed;


            /* this is for burn snag pool don't delet 20201022 */
             if (canopy_target[0].defaults[0][0].epc.veg_type == TREE && canopy_target[0].defaults[0][0].epc.phenology_type == EVERGREEN) //To make sure only save the overstory consumption NREN 20190914
                { patch[0].overstory_burn = canopy_target[0].fe.canopy_target_prop_c_consumed;
               //   printf("\n the fire consuption is %lf",mort.mort_cpool);
                                                                    }


			thin_type =2;	/* Harvest option */
			update_mortality(
				canopy_target[0].defaults[0][0].epc,
				&(canopy_target[0].cs),
				&(canopy_target[0].cdf),
				&(patch[0].cdf),
				&(canopy_target[0].ns),
				&(canopy_target[0].ndf),
				&(patch[0].ndf),
				&(patch[0].litter_cs),
				&(patch[0].litter_ns),
				thin_type,
				mort);


			/* Compute proportion of total target canopy carbon that is killed but remains as litter/cwd */
			canopy_target[0].fe.canopy_target_prop_c_remain = canopy_target[0].fe.canopy_target_prop_mort - canopy_target[0].fe.canopy_target_prop_c_consumed;

			/* Adjust canopy_target_prop_c_remain since update mortality is run twice. Vegetation carbon */
			/* stores on the second call to update_mortality have already been altered during the first call. */
			/* The following adjustment accounts for this change. */
			if (fabs(canopy_target[0].fe.canopy_target_prop_c_remain - 1.0) < ZERO){
				canopy_target[0].fe.canopy_target_prop_c_remain_adjusted = 0;
			} else {
				canopy_target[0].fe.canopy_target_prop_c_remain_adjusted = canopy_target[0].fe.canopy_target_prop_c_remain / (1 - canopy_target[0].fe.canopy_target_prop_c_remain);
			}


			/* For understory vegetation, complete mortality of leaves was assumed if a patch was burned, regardless of pspread */
			/* Following code adjusts canopy_target_prop_c_remain_adjusted to be 1 when canopy is understory */
			/* Also note that leafc_transfer and leafc_storage pools are not killed by fire */
			canopy_target[0].fe.canopy_target_height_u_prop = max(min((patch[0].soil_defaults[0][0].overstory_height_thresh - canopy_target[0].fe.canopy_target_height)/(patch[0].soil_defaults[0][0].overstory_height_thresh-patch[0].soil_defaults[0][0].understory_height_thresh),1.0),0);
			canopy_target[0].fe.canopy_target_prop_c_remain_adjusted_leafc = (canopy_target[0].fe.canopy_target_prop_c_remain_adjusted * (1 - canopy_target[0].fe.canopy_target_height_u_prop)) + canopy_target[0].fe.canopy_target_height_u_prop;

			/* Determine the portion of mortality that remains on landscape */
			mort.mort_cpool = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_leafc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted_leafc;
			mort.mort_deadstemc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_livestemc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_frootc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_deadcrootc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_livecrootc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
			mort.mort_deadleafc = canopy_target[0].fe.canopy_target_prop_c_remain_adjusted_leafc;

			// track the overstory c consumed
            if (canopy_target[0].fe.canopy_target_height > patch[0].soil_defaults[0][0].overstory_height_thresh && patch[0].soil_defaults[0][0].overstory_height_thresh >0){
            canopy_target[0].fe.overstory_c_consumed = canopy_target[0].fe.canopy_target_biomassc * canopy_target[0].fe.canopy_target_prop_c_consumed;
            canopy_target[0].fe.overstory_leafc_consumed = canopy_target[0].fe.canopy_target_leafc * canopy_target[0].fe.canopy_target_prop_c_consumed;
            canopy_target[0].fe.overstory_stemc_consumed = canopy_target[0].fe.canopy_target_stemc * canopy_target[0].fe.canopy_target_prop_c_consumed;
            canopy_target[0].fe.overstory_rootc_consumed = canopy_target[0].fe.canopy_target_rootc * canopy_target[0].fe.canopy_target_prop_c_consumed;

			// track overstory c mortality

            //new outputs
            canopy_target[0].fe.overstory_leafc_mortality = canopy_target[0].fe.canopy_target_leafc * canopy_target[0].fe.canopy_target_prop_c_remain_adjusted_leafc;
            canopy_target[0].fe.overstory_stemc_mortality = canopy_target[0].fe.canopy_target_stemc * canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
            canopy_target[0].fe.overstory_rootc_mortality = canopy_target[0].fe.canopy_target_rootc * canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;

            canopy_target[0].fe.overstory_c_mortality = canopy_target[0].fe.overstory_leafc_mortality + canopy_target[0].fe.overstory_stemc_mortality + canopy_target[0].fe.overstory_rootc_mortality;

                 if(command_line[0].f !=NULL && command_line[0].output_flags.yearly ==1 ){

                    //overstory
                    canopy_target[0].fe.acc_year.overstory_c_consumed +=  canopy_target[0].fe.overstory_c_consumed;
                    canopy_target[0].fe.acc_year.overstory_leafc_consumed += canopy_target[0].fe.overstory_leafc_consumed;
                    canopy_target[0].fe.acc_year.overstory_stemc_consumed += canopy_target[0].fe.overstory_stemc_consumed;
                    canopy_target[0].fe.acc_year.overstory_rootc_consumed += canopy_target[0].fe.overstory_rootc_consumed;

                    canopy_target[0].fe.acc_year.overstory_c_mortality+=  canopy_target[0].fe.overstory_c_mortality;
                    canopy_target[0].fe.acc_year.overstory_leafc_mortality += canopy_target[0].fe.overstory_leafc_mortality;
                    canopy_target[0].fe.acc_year.overstory_stemc_mortality += canopy_target[0].fe.overstory_stemc_mortality;
                    canopy_target[0].fe.acc_year.overstory_rootc_mortality += canopy_target[0].fe.overstory_rootc_mortality;

                    canopy_target[0].fe.acc_year.length_overstory +=1;
                 }
            }

			thin_type =1;
			update_mortality(
				canopy_target[0].defaults[0][0].epc,
				&(canopy_target[0].cs),
				&(canopy_target[0].cdf),
				&(patch[0].cdf),
				&(canopy_target[0].ns),
				&(canopy_target[0].ndf),
				&(patch[0].ndf),
				&(patch[0].litter_cs),
				&(patch[0].litter_ns),
				thin_type,
				mort);



            /*----------------------------------------------------------------------------------------*/
            /* accumulate the monthly fire effects output to yearly by for yearly fire output         */
            /*----------------------------------------------------------------------------------------*/

            if(command_line[0].f !=NULL && command_line[0].output_flags.yearly ==1 ){

                //canopy_target.fe.acc_year.canopy_target_height +=
                canopy_target[0].fe.acc_year.canopy_target_height_u_prop += canopy_target[0].fe.canopy_target_height_u_prop;
                canopy_target[0].fe.acc_year.canopy_target_prop_mort += canopy_target[0].fe.canopy_target_prop_mort;
                canopy_target[0].fe.acc_year.canopy_target_prop_mort_consumed += canopy_target[0].fe.canopy_target_prop_mort_consumed;
                canopy_target[0].fe.acc_year.canopy_target_prop_mort_u_component += canopy_target[0].fe.canopy_target_prop_mort_u_component;
                canopy_target[0].fe.acc_year.canopy_target_prop_mort_o_component += canopy_target[0].fe.canopy_target_prop_mort_o_component;
                canopy_target[0].fe.acc_year.canopy_target_prop_c_consumed += canopy_target[0].fe.canopy_target_prop_c_consumed;
                canopy_target[0].fe.acc_year.canopy_target_prop_c_remain += canopy_target[0].fe.canopy_target_prop_c_remain;
                canopy_target[0].fe.acc_year.canopy_target_prop_c_remain_adjusted += canopy_target[0].fe.canopy_target_prop_c_remain_adjusted;
                canopy_target[0].fe.acc_year.canopy_target_prop_c_remain_adjusted_leafc += canopy_target[0].fe.canopy_target_prop_c_remain_adjusted_leafc;
                //canopy_target.fe.acc_year.canopy_subtarget_height +=
                canopy_target[0].fe.acc_year.canopy_subtarget_height_u_prop += canopy_target[0].fe.canopy_subtarget_height_u_prop;
                canopy_target[0].fe.acc_year.canopy_subtarget_prop_mort += canopy_target[0].fe.canopy_subtarget_prop_mort;
                canopy_target[0].fe.acc_year.canopy_subtarget_prop_mort_consumed += canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;
                canopy_target[0].fe.acc_year.canopy_subtarget_prop_c_consumed += canopy_target[0].fe.canopy_subtarget_prop_c_consumed;


                canopy_target[0].fe.acc_year.length +=1;


            } //end accumulation



			} // end for at line 137 c
		}
	} /* end if(pspread > 0 ) */
    else {
            for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
            for ( c=0 ; c<patch[0].layers[layer].count; c++ ){

			// Calculates metrics for targer canopy
			canopy_target = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
			canopy_target[0].fe.canopy_target_height = canopy_target[0].epv.height;

			// Calculates metrics for next lowest canopy (subtarget canopy)
			if (patch[0].num_layers > (layer+1)){
				canopy_subtarget = patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])];
				canopy_target[0].fe.canopy_subtarget_height = canopy_subtarget[0].epv.height;

				canopy_target[0].fe.canopy_subtarget_biomassc = canopy_subtarget[0].cs.leafc + canopy_subtarget[0].cs.dead_leafc +
						canopy_subtarget[0].cs.live_stemc + canopy_subtarget[0].cs.dead_stemc +
						canopy_subtarget[0].cs.live_crootc + canopy_subtarget[0].cs.dead_crootc +
						canopy_subtarget[0].cs.frootc + canopy_subtarget[0].cs.cpool;

                canopy_target[0].fe.canopy_subtarget_leafc = canopy_subtarget[0].cs.leafc + canopy_subtarget[0].cs.dead_leafc;
                canopy_target[0].fe.canopy_subtarget_stemc = canopy_subtarget[0].cs.live_stemc + canopy_subtarget[0].cs.dead_stemc;
                canopy_target[0].fe.canopy_subtarget_rootc = canopy_subtarget[0].cs.live_crootc + canopy_subtarget[0].cs.dead_crootc + canopy_subtarget[0].cs.frootc;

                            // add the over story NREN
            canopy_target[0].fe.canopy_target_biomassc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc +
                    canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc +
                    canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc +
                    canopy_target[0].cs.frootc + canopy_target[0].cs.cpool;


            canopy_target[0].fe.canopy_target_leafc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc;
            canopy_target[0].fe.canopy_target_stemc = canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc;
            canopy_target[0].fe.canopy_target_rootc = canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc + canopy_target[0].cs.frootc;




                } else {
				canopy_target[0].fe.canopy_subtarget_height = 0;
				canopy_target[0].fe.canopy_subtarget_biomassc = 0;
				canopy_target[0].fe.canopy_subtarget_leafc = 0.0;
				canopy_target[0].fe.canopy_subtarget_stemc = 0.0;
				canopy_target[0].fe.canopy_subtarget_rootc = 0.0;

            // add the over story NREN
            canopy_target[0].fe.canopy_target_biomassc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc +
                    canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc +
                    canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc +
                    canopy_target[0].cs.frootc + canopy_target[0].cs.cpool;


            canopy_target[0].fe.canopy_target_leafc = canopy_target[0].cs.leafc + canopy_target[0].cs.dead_leafc;
            canopy_target[0].fe.canopy_target_stemc = canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc;
            canopy_target[0].fe.canopy_target_rootc = canopy_target[0].cs.live_crootc + canopy_target[0].cs.dead_crootc + canopy_target[0].cs.frootc;



			}

           } // end for c=0
        } //end for layer =0

    }



	return;
} /*end compute_fire_effects.c*/



