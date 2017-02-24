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
						double pspread)

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

	struct canopy_strata_object *canopy_strata_upper;
	struct canopy_strata_object *canopy_strata_lower;
	struct mortality_struct mort;
	struct fire_litter_soil_loss_struct fire_loss;
	int c, layer;
	int thin_type;
	double m_cwdc_to_atmos, m_cwdn_to_atmos;
	double understory_litter_c;
	double layer_upper_height,layer_lower_height;
	double layer_upper_height_adj, layer_lower_height_adj;
	double layer_lower_c;
	double layer_lower_c_loss_percent, understory_c_loss;
	double layer_upper_c_loss_percent, layer_upper_c_loss_percent_understory_comp;
	double layer_upper_c_loss_percent_adj1, layer_upper_c_loss_percent_adj2;
	double loss_vapor_percent, c_loss_vapor_percent;
	double c_loss_remain_percent, c_loss_remain_percent_alt;


	/*--------------------------------------------------------------*/
	/*	Compute litter and soil removed.			*/
	/*--------------------------------------------------------------*/


	if (pspread > 0){

	/* Calculate litter biomass for use later in canopy effects */
	understory_litter_c = patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c + patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c;

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


	update_litter_soil_mortality(
		 &(patch[0].cdf),
		 &(patch[0].ndf),
		 &(patch[0].soil_cs),
		 &(patch[0].soil_ns),
		 &(patch[0].litter_cs),
		 &(patch[0].litter_ns),
		 fire_loss);

printf("\n -------------------");
printf("\n understory_litter_c = %lf", understory_litter_c);
printf("\n -------------------");


	/*--------------------------------------------------------------*/
	/*		Compute vegetation effects.			*/
	/*--------------------------------------------------------------*/


	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){

			/* Calculates metrics for targer layer */
			canopy_strata_upper = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
			layer_upper_height = canopy_strata_upper[0].epv.height;

			/* Calculates metrics for next lowest layer (May be redone to calculate all remaining lower layers) */
			if (patch[0].num_layers > (layer+1)){
				canopy_strata_lower = patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])];
				layer_lower_height = canopy_strata_lower[0].epv.height;
				layer_lower_c = canopy_strata_lower[0].cs.leafc + canopy_strata_lower[0].cs.live_stemc + canopy_strata_lower[0].cs.dead_stemc;
			} else {
				layer_lower_height = 0;
				layer_lower_c = 0;
			}

printf("\n -------------------");
printf("\n layer_upper_height = %lf", layer_upper_height);
printf("\n layer_lower_height = %lf", layer_lower_height);
printf("\n layer_lower_c = %lf", layer_lower_c);
printf("\n -------------------");


			/*--------------------------------------------------------------*/
			/* Calculate coarse woody debris removed			*/
			/*--------------------------------------------------------------*/

			/* Litter consumption is approximated based CONSUME model outputs */
			/* Consumption 1000hr-fuel (Mg/ha) = 2.735 + 0.3285 * 1000hr-fuel (Mg/ha) - 0.0457 * Fuel Moisture (e.g 80%) */
			/* Consumption 1000hr-fuel (Mg/ha) = 0.33919 * 1000hr-fuel (Mg/ha) (Modified eqn to exclude moisture and intercept through zero) */
			m_cwdc_to_atmos = canopy_strata_upper[0].cs.cwdc * .339;
			m_cwdn_to_atmos = canopy_strata_upper[0].ns.cwdn * .339;
			canopy_strata_upper[0].cs.cwdc -= m_cwdc_to_atmos;
			canopy_strata_upper[0].ns.cwdn -= m_cwdn_to_atmos;


			/*--------------------------------------------------------------*/
			/* Calculate effects when upper layer is tall			*/
			/*--------------------------------------------------------------*/

			// Note that when this code requires that height thresholds for differnt canopies be the same or anomolous behavior may occur.

			if (layer_upper_height > canopy_strata_upper[0].defaults[0][0].overstory_height_thresh){

				if (layer_lower_height > canopy_strata_upper[0].defaults[0][0].overstory_height_thresh){

					understory_c_loss = understory_litter_c;

				} else if (layer_lower_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_lower_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

					/* Determines the percent of loss attributed to understory. 1-layer_lower_height_adj gives loss attributed to overstory) */
					layer_lower_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_lower_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);

					/* Determine the amount of carbon lost in the understory component of the lower layer */
					if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
						fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
    						exit(EXIT_FAILURE);
					} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
						layer_lower_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
					} else {
						layer_lower_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
					}
					understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent * layer_lower_height_adj) + understory_litter_c;	// layer_lower_height_adj accounts for adjustment for lower layer height

				} else if (layer_lower_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

					/* Determine the amount of carbon lost in the lower layer */
					if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
						fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
    						exit(EXIT_FAILURE);
					} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
						layer_lower_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
					} else {
						layer_lower_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
					}
					understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent) + understory_litter_c;
				}

printf("\n -------------------");
printf("\n layer_lower_c_loss_percent = %lf", layer_lower_c_loss_percent);
printf("\n understory_c_loss = %lf", understory_c_loss);
printf("\n -------------------");


				/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
				layer_upper_c_loss_percent = 1 - (1/(1+exp(-(canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))));

				/* Determine the portion of c_loss_percent in the upper layer that is vaporized */
				if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
					fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
					loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
				} else {
					loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
				}

//leafc_loss_percent = layer_upper_c_loss_percent
//leafc_vapor_percent = loss_vapor_percent

			/*--------------------------------------------------------------*/
			/* Calculate effects when upper layer is an intermediate height	*/
			/*--------------------------------------------------------------*/

			} else if (layer_upper_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_upper_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

				/* Determines the percent of upper canopy loss attributed to understory. 1-layer_upper_height_adj gives loss attributed to overstory) */
				layer_upper_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_upper_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);

				if (layer_lower_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_lower_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

					/* Determines the percent of lower canopy loss attributed to understory. 1-layer_lower_height_adj gives loss attributed to overstory) */
					layer_lower_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_lower_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);


					/* ---Determine loss for understory component of upper layer--- */

					/* Function to relate pspread to percent loss in the upper layer */
					if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
						fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
    						exit(EXIT_FAILURE);
					} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
						layer_upper_c_loss_percent_understory_comp = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
					} else {
						layer_upper_c_loss_percent_understory_comp = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
					}

					layer_upper_c_loss_percent_adj1 = layer_upper_c_loss_percent_understory_comp * layer_lower_height_adj * layer_upper_height_adj;	// layer_lower_height_adj and layer_upper_height_adj account for height adjustments to both layers 


					/* ---Determine loss for overstory component of upper layer--- */

					layer_lower_c_loss_percent = layer_upper_c_loss_percent_understory_comp;		/* percent c lost is same for upper and lower layer since lost is based on pspread for both */
					understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent * layer_lower_height_adj) + understory_litter_c;	// layer_lower_height_adj accounts for height adjustment for lower layer

					/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
					layer_upper_c_loss_percent_adj2 = (1 - 1/(1+exp(-canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))) * (1-layer_upper_height_adj);		// layer_upper_height_adj accounts for upper layer height adjustment

					/* Combine losses due to overstory and understory components of upper layer */
					layer_upper_c_loss_percent = layer_upper_c_loss_percent_adj1 + layer_upper_c_loss_percent_adj2;		// This may need to be capped at 1.


				} else if (layer_lower_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

					/* ---Determine loss for understory component of upper layer--- */

					/* Function to relate pspread to percent loss in the upper layer */
					if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
						fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
    						exit(EXIT_FAILURE);
					} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
						layer_upper_c_loss_percent_understory_comp = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
					} else {
						layer_upper_c_loss_percent_understory_comp = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
					}

					layer_upper_c_loss_percent_adj1 = layer_upper_c_loss_percent_understory_comp * layer_upper_height_adj;		// layer_upper_height_adj accounts for upper layer height adjustment


					/* ---Determine loss for overstory component of upper layer--- */

					layer_lower_c_loss_percent = layer_upper_c_loss_percent_understory_comp;		/* percent c lost is same for upper and lower layer since lost is based on pspread for both */
					understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent) + understory_litter_c;

					/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
					layer_upper_c_loss_percent_adj2 = (1 - 1/(1+exp(-canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))) * (1-layer_upper_height_adj);		// layer_upper_height_adj accounts for upper layer height adjustment

					/* Combine losses due to overstory and understory components of upper layer */
					layer_upper_c_loss_percent = layer_upper_c_loss_percent_adj1 + layer_upper_c_loss_percent_adj2;		// This may need to be capped at 1.
				}


				/* Determine the portion of c_loss_percent that is vaporized from strata */
				if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
					fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
					loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
				} else {
					loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
				}


//leafc_loss_percent = (layer_upper_height_adj) + (layer_upper_c_loss_percent_adj2) /* Leaf loss attributed to understory component (assumed to 100%) plus leaf loss attributed to overstory component (assumed to be based on understory biomass))  */
//leafc_vapor_percent = loss_vapor_percent


			/*--------------------------------------------------------------*/
			/* Calculate effects when upper layer is short			*/
			/*--------------------------------------------------------------*/

			} else if (layer_upper_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

				/* Upper layer acts as a understory. Loss is a function of pspread */
				if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
					fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
					layer_upper_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
				} else {
					layer_upper_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
				}

//leafc_loss_percent = 1
//leafc_vapor_percent = loss_vapor_percent

				/* Determine the portion of c_loss_percent that is vaporized from landscape */
				if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
					fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
    					exit(EXIT_FAILURE);
				} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
					loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
				} else {
					loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
				}
			}

			/*--------------------------------------------------------------*/
			/* Compute effects						*/
			/*--------------------------------------------------------------*/

printf("\n layer_upper_c_loss_percent = %lf", layer_upper_c_loss_percent);
printf("\n loss_vapor_percent = %lf", loss_vapor_percent);

			/* Compute percent of total carbon that is vaporized */
			c_loss_vapor_percent = loss_vapor_percent * layer_upper_c_loss_percent;
printf("\n c_loss_vapor_percent = %lf", c_loss_vapor_percent);

			/* Compute percent of total carbon that remains as litter/cwd */
			c_loss_remain_percent = layer_upper_c_loss_percent - c_loss_vapor_percent;
printf("\n c_loss_remain_percent = %lf", c_loss_remain_percent);

			/* Adjust c_loss_remain_percent since update mortality is run twice, with vaporized C removed first */
			c_loss_remain_percent_alt = c_loss_remain_percent / (1 - c_loss_vapor_percent);
printf("\n c_loss_remain_percent_alt = %lf", c_loss_remain_percent_alt);

			mort.mort_cpool = c_loss_vapor_percent;
			mort.mort_leafc = c_loss_vapor_percent;
			mort.mort_deadstemc = c_loss_vapor_percent;
			mort.mort_livestemc = c_loss_vapor_percent;
			mort.mort_frootc = c_loss_vapor_percent;
			mort.mort_deadcrootc = c_loss_vapor_percent;
			mort.mort_livecrootc = c_loss_vapor_percent;

printf("\n upper_leafc1 = %lf", canopy_strata_upper[0].cs.leafc);
printf("\n upper_cwdc1 = %lf", canopy_strata_upper[0].cs.cwdc);

			thin_type =2;	/* Harvest option */
			update_mortality(
				canopy_strata_upper[0].defaults[0][0].epc,
				&(canopy_strata_upper[0].cs),
				&(canopy_strata_upper[0].cdf),
				&(patch[0].cdf),
				&(canopy_strata_upper[0].ns),
				&(canopy_strata_upper[0].ndf),
				&(patch[0].ndf),
				&(patch[0].litter_cs),
				&(patch[0].litter_ns),
				thin_type,
				mort);

			/* Determine the portion of loss that remains on landscape */
			mort.mort_cpool = c_loss_remain_percent_alt;
			mort.mort_leafc = c_loss_remain_percent_alt;
			mort.mort_deadstemc = c_loss_remain_percent_alt;
			mort.mort_livestemc = c_loss_remain_percent_alt;
			mort.mort_frootc = c_loss_remain_percent_alt;
			mort.mort_deadcrootc = c_loss_remain_percent_alt;
			mort.mort_livecrootc = c_loss_remain_percent_alt;

printf("\n upper_leafc2 = %lf", canopy_strata_upper[0].cs.leafc);
printf("\n upper_cwdc2 = %lf", canopy_strata_upper[0].cs.cwdc);

			thin_type =3;
			update_mortality(
				canopy_strata_upper[0].defaults[0][0].epc,
				&(canopy_strata_upper[0].cs),
				&(canopy_strata_upper[0].cdf),
				&(patch[0].cdf),
				&(canopy_strata_upper[0].ns),
				&(canopy_strata_upper[0].ndf),
				&(patch[0].ndf),
				&(patch[0].litter_cs),
				&(patch[0].litter_ns),
				thin_type,
				mort);

printf("\n upper_leafc3 = %lf", canopy_strata_upper[0].cs.leafc);
printf("\n upper_cwdc3 = %lf", canopy_strata_upper[0].cs.cwdc);

			}
		}
	}

	return;
} /*end compute_fire_effects.c*/



