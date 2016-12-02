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

	fire_loss.loss_litr1c = 1;
	fire_loss.loss_litr2c = 1;
	fire_loss.loss_litr3c = 1;
	fire_loss.loss_litr4c = 1;
	fire_loss.loss_soil1c = 1;
	fire_loss.loss_soil2c = 0;
	fire_loss.loss_soil3c = 0;
	fire_loss.loss_soil4c = 0;
	fire_loss.loss_litr1n = 1;
	fire_loss.loss_litr2n = 1;
	fire_loss.loss_litr3n = 1;
	fire_loss.loss_litr4n = 1;
	fire_loss.loss_soil1n = 1;
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



	/*--------------------------------------------------------------*/
	/* Calculate effects when upper layer is tall			*/
	/*--------------------------------------------------------------*/

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

	/*--------------------------------------------------------------*/
	/* Calculate effects when upper layer is an intermediate height	*/
	/*--------------------------------------------------------------*/

			} else if (layer_upper_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_upper_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

				/* Determines the percent of loss attributed to understory. 1-layer_upper_height_adj gives loss attributed to overstory) */
				layer_upper_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_upper_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);


				if (layer_lower_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_lower_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

					/* Determines the percent of loss attributed to understory. 1-layer_lower_height_adj gives loss attributed to overstory) */
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

			/* Compute percent of total carbon that is vaporized */
			c_loss_vapor_percent = loss_vapor_percent * layer_upper_c_loss_percent;
			/* Compute percent of total carbon that remains as litter/cwd */
			c_loss_remain_percent = layer_upper_c_loss_percent - c_loss_vapor_percent;
			/* Adjust c_loss_remain_percent since update mortality is run twice, with vaporized C removed first */
			c_loss_remain_percent_alt = c_loss_remain_percent / (1 - c_loss_vapor_percent);

			mort.mort_cpool = c_loss_vapor_percent;
			mort.mort_leafc = c_loss_vapor_percent;
			mort.mort_deadstemc = c_loss_vapor_percent;
			mort.mort_livestemc = c_loss_vapor_percent;
			mort.mort_frootc = c_loss_vapor_percent;
			mort.mort_deadcrootc = c_loss_vapor_percent;
			mort.mort_livecrootc = c_loss_vapor_percent;

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

			}
		}
	}

	return;
} /*end compute_fire_effects.c*/



