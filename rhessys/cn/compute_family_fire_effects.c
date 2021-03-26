/*--------------------------------------------------------------*/
/* 																*/
/*			compute_family_fire_effects							*/
/*																*/
/*	NAME														*/
/*	compute_family_fire_effects.c 								*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*	Determines vegetation loss following fire					*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	This function builds on the standard compute_fire_effects	*/
/*	written by Ryan Bart, but restructures it to function on 	*/
/*	more than 2 layers, and using aggreate under and			*/
/*	intermedaite stories										*/
/*																*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

void compute_family_fire_effects(
	struct patch_family_object *patch_family,
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
	struct patch_object *patch;
	// struct fire_effects_object fe_under;
	// struct fire_effects_object fe_int;
	int p, c, layer;
	int thin_type, sev;
	int under_ct, intr_ct;
	double litter_c_consumed;
	double agg_under_height, agg_under_carbon, under_pct_area;
	double agg_intr_height, agg_intr_carbon, intr_pct_area;
	double intr_height_u_prop, intr_c_consumed, under_c_consumed;

	/*--------------------------------------------------------------*/
	/*	Compute litter and soil removed.							*/
	/*--------------------------------------------------------------*/

	if (pspread > 0) {

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
	// iterate thru patches in fam, weight + accumulate
	litter_c_consumed = 0;

	for (p = 0; p < patch_family[0].num_patches_in_fam; p++)
	{
		litter_c_consumed += (patch_family[0].patches[p][0].litter_cs.litr1c * fire_loss.loss_litr1c +
							  patch_family[0].patches[p][0].litter_cs.litr2c * fire_loss.loss_litr2c +
							  patch_family[0].patches[p][0].litter_cs.litr3c * fire_loss.loss_litr3c +
							  patch_family[0].patches[p][0].litter_cs.litr4c * fire_loss.loss_litr4c) *
							 patch_family[0].patches[p][0].family_pct_cover;

		update_litter_soil_mortality(
		 &(patch_family[0].patches[p][0].cdf),
		 &(patch_family[0].patches[p][0].ndf),
		 &(patch_family[0].patches[p][0].soil_cs),
		 &(patch_family[0].patches[p][0].soil_ns),
		 &(patch_family[0].patches[p][0].litter_cs),
		 &(patch_family[0].patches[p][0].litter_ns),
		 fire_loss);
	}

	/*--------------------------------------------------------------*/
	/*		Fire Effects Outline									*/
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

	// Example layers for FAMILY fire effects - NOT using subtarget, just checking if in X story then looks to the appropriate aggregate layer
	// TOP 
	// layer 0 -> uses aggregate of understory + understory component of all intr stories 
	// layer 1 -> uses aggregate of understory + understory component of all intr stories 
	// --- overstory threshold
	// layer 2 -> (uses aggregate of understory + understory component of all intr stories) + understory consumption (function of pspread)
	// layer 3 -> (uses aggregate of understory + understory component of all intr stories) + understory consumption (function of pspread)
	// --- understory threshold
	// layer 4 -> understory consumption (function of pspread)
	// layer 5 -> understory consumption (function of pspread)
	// BOTTOM


	/*--------------------------------------------------------------*/
	/*		Create aggregate layers for under & intermediate		*/
	/*--------------------------------------------------------------*/
	// build aggregate understory layer to serve as universal canopy subtarget
	// just need to create a aggregate canopy height, and C, (leaf+live stem +dead stem)
	agg_under_height = 0;
	agg_under_carbon = 0;
	under_pct_area = 0;
	agg_intr_height = 0;
	agg_intr_carbon = 0;
	intr_pct_area = 0;
	// count for layer groups
	under_ct = 0;
	intr_ct = 0;

	for (layer = 0; layer < patch_family[0].num_layers; layer++)
	{
		for (c = 0; c < patch_family[0].layers[layer].count; c++)
		{
			canopy_target = patch_family[0].canopy_strata[(patch_family[0].layers[layer].strata[c])];
			if (canopy_target[0].epv.height <= patch_family[0].overstory_height_thresh)
			{
				// understory height + carbon
				if (canopy_target[0].epv.height <= patch_family[0].understory_height_thresh)
				{
					under_ct++;
					agg_under_height += canopy_target[0].epv.height * patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
					agg_under_carbon += (canopy_target[0].cs.leafc + canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc) *
							   patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
					under_pct_area += patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
				}

				// intermediate height + carbon
				if (canopy_target[0].epv.height <= patch_family[0].overstory_height_thresh &&
					canopy_target[0].epv.height > patch_family[0].understory_height_thresh)
				{
					intr_ct++;
					agg_intr_height += canopy_target[0].epv.height * patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
					agg_intr_carbon += (canopy_target[0].cs.leafc + canopy_target[0].cs.live_stemc + canopy_target[0].cs.dead_stemc) *
							  patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
					intr_pct_area += patch_family[0].patches[canopy_target[0].fam_patch_ind][0].family_pct_cover;
				}
			}
		}
	}

	// normalize by coverage
	agg_under_height /= under_pct_area;
	agg_under_carbon /= under_pct_area;
	agg_intr_height /= intr_pct_area;
	agg_intr_carbon /= intr_pct_area;

	/* Determine the proportion of subtarget canopy attributed to understory. Proportion overstory is 1 - canopy_subtarget_height_u_prop */
	// since using agg intr story height, calc this for all stories
	intr_height_u_prop = (patch_family[0].overstory_height_thresh - agg_intr_height) /
						 (patch_family[0].overstory_height_thresh - patch_family[0].understory_height_thresh);
	

	/*--------------------------------------------------------------*/
	/*		Compute effects for each layer							*/
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer < patch_family[0].num_layers; layer++ ){
		for ( c=0 ; c < patch_family[0].layers[layer].count; c++ ){

			/* Calculates metrics for targer canopy */
			canopy_target = patch_family[0].canopy_strata[(patch_family[0].layers[layer].strata[c])];
			canopy_target[0].fe.canopy_target_height = canopy_target[0].epv.height;
			
			// set aggregate values for each layer - mainly for tracking (heights aren't used after this point anyways)
			canopy_target[0].fe.agg_under_height = agg_under_height;
			canopy_target[0].fe.agg_under_carbon = agg_under_carbon;
			canopy_target[0].fe.agg_under_pct_cover = under_pct_area;
			canopy_target[0].fe.agg_intr_height = agg_intr_height;
			canopy_target[0].fe.agg_intr_carbon = agg_intr_carbon;
			canopy_target[0].fe.agg_intr_pct_cover = intr_pct_area;

			/*--------------------------------------------------------------*/
			/* Calculate coarse woody debris removed						*/
			/*--------------------------------------------------------------*/
			/* Litter consumption is approximated based CONSUME model outputs */
			/* Consumption 1000hr-fuel (Mg/ha) = 2.735 + 0.3285 * 1000hr-fuel (Mg/ha) - 0.0457 * Fuel Moisture (e.g 80%) (Original CONSUME eqn) */
			/* Consumption 1000hr-fuel (Mg/ha) = 0.33919 * 1000hr-fuel (Mg/ha) (Modified CONSUME eqn to exclude moisture and have intercept through zero) */
			canopy_target[0].fe.m_cwdc_to_atmos = canopy_target[0].cs.cwdc * .339;
			canopy_target[0].fe.m_cwdn_to_atmos = canopy_target[0].ns.cwdn * .339;
			canopy_target[0].cs.cwdc -= canopy_target[0].fe.m_cwdc_to_atmos;
			canopy_target[0].ns.cwdn -= canopy_target[0].fe.m_cwdn_to_atmos;

			/*--------------------------------------------------------------*/
			/* Param checks, calc initial mortality							*/
			/*--------------------------------------------------------------*/
			// get initial prop_mort
			if (canopy_target[0].defaults[0][0].understory_mort <= ZERO)
			{
				fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].understory_mort must be greater than 0.\n");
				exit(EXIT_FAILURE);
			}
			else if (canopy_target[0].defaults[0][0].understory_mort == 1)
			{
				canopy_target[0].fe.canopy_target_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].defaults[0][0].understory_mort * pspread;
			}
			else
			{
				canopy_target[0].fe.canopy_target_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort =
					(pow(canopy_target[0].defaults[0][0].understory_mort, pspread) - 1) / (canopy_target[0].defaults[0][0].understory_mort - 1);
			}

			/* Determine the proportion of target canopy mortality consumed */
			if (canopy_target[0].defaults[0][0].consumption <= ZERO)
			{
				fprintf(stderr, "ERROR: canopy_target[0].defaults[0][0].consumption must be greater than 0.\n");
				exit(EXIT_FAILURE);
			} 

			/*--------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is tall			*/
			/*--------------------------------------------------------------*/
			if (canopy_target[0].fe.canopy_target_height > patch_family[0].overstory_height_thresh)
			{
				/* Determine the amount of understory carbon consumed understory_c_consumed, which is used to compute how well fire is propogated to overstory */
				if (under_ct == 0 && intr_ct == 0) // if no under or intr stories
				{
					canopy_target[0].fe.understory_c_consumed = litter_c_consumed;
				}
				if (under_ct > 0) // there is at least 1 understory
				{
					// find consumption
					if (canopy_target[0].defaults[0][0].consumption == 1) {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed =
							(pow(canopy_target[0].defaults[0][0].consumption, canopy_target[0].fe.canopy_subtarget_prop_mort) - 1) / (canopy_target[0].defaults[0][0].consumption - 1);
					}
					/* Determine the proportion of understory canopy carbon consumed */
					canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;
					// Determine the amount of carbon consumed in the understory based on aggregate understory
					under_c_consumed = (canopy_target[0].fe.agg_under_carbon * canopy_target[0].fe.canopy_subtarget_prop_c_consumed);
					// combine with litter and set to understory_c_consumed, if both under and intr stories are present, will be overwritten.
					canopy_target[0].fe.understory_c_consumed = under_c_consumed + litter_c_consumed;
				}
				if (intr_ct > 0) // there's at least 1 intr story
				{
					/* For intermediate height subtarget canopy, adjust canopy_subtarget_prop_mort to only account for understory component, based on aggregate intrmediate height */
					canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort * intr_height_u_prop;
					// need to recalc prop mort consumption
					if (canopy_target[0].defaults[0][0].consumption == 1) {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed =
							(pow(canopy_target[0].defaults[0][0].consumption, canopy_target[0].fe.canopy_subtarget_prop_mort) - 1) / (canopy_target[0].defaults[0][0].consumption - 1);
					}
					/* Determine the proportion of subtarget canopy carbon consumed */
					canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;
					/* Determine the amount of carbon consumed in the intmediate story */
					intr_c_consumed = canopy_target[0].fe.agg_intr_carbon * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					// combine with litter and set to understory_c_consumed, if both under and intr stories are present, will be overwritten.
					canopy_target[0].fe.understory_c_consumed = intr_c_consumed + litter_c_consumed;
				}
				if (under_ct > 0 && intr_ct > 0)
				{
					// Combine understory and intermediate story consumptions, weighted by area(?), add litter in
					canopy_target[0].fe.understory_c_consumed = (under_c_consumed * canopy_target[0].fe.agg_under_pct_cover / (canopy_target[0].fe.agg_under_pct_cover + canopy_target[0].fe.agg_intr_pct_cover)) +
																(intr_c_consumed * canopy_target[0].fe.agg_intr_pct_cover / (canopy_target[0].fe.agg_under_pct_cover + canopy_target[0].fe.agg_intr_pct_cover)) + litter_c_consumed;
				}

				/* Determine the proportion of target canopy mortality based on the amount of understory consumed (sigmoidal relationship) */
				canopy_target[0].fe.canopy_target_prop_mort = 1 - (1/(1+exp(-(canopy_target[0].defaults[0][0].overstory_mort_k1*(canopy_target[0].fe.understory_c_consumed - 
				canopy_target[0].defaults[0][0].overstory_mort_k2)))));

				/* Determine the proportion of target canopy mortality consumed */
				 if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = 
					(pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}

			/*----------------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is an intermediate height	*/
			/*----------------------------------------------------------------------*/
			}
			else if (canopy_target[0].fe.canopy_target_height <= patch_family[0].overstory_height_thresh &&
					 canopy_target[0].fe.canopy_target_height >= patch_family[0].overstory_height_thresh)
			{
				/* Determine the proportion of target canopy attributed to understory. Proportion overstory is 1 - canopy_target_height_u_prop */
				canopy_target[0].fe.canopy_target_height_u_prop = (patch_family[0].overstory_height_thresh - canopy_target[0].fe.canopy_target_height) /
																  (patch_family[0].overstory_height_thresh - patch_family[0].understory_height_thresh);
				/* Adjust canopy_target_prop_mort to only account for understory component */
				canopy_target[0].fe.canopy_target_prop_mort_u_component = canopy_target[0].fe.canopy_target_prop_mort * canopy_target[0].fe.canopy_target_height_u_prop;

				if (under_ct > 0) // there is at least 1 understory
				{
					// find consumption
					if (canopy_target[0].defaults[0][0].consumption == 1) {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed =
							(pow(canopy_target[0].defaults[0][0].consumption, canopy_target[0].fe.canopy_subtarget_prop_mort) - 1) / (canopy_target[0].defaults[0][0].consumption - 1);
					}
					/* Determine the proportion of understory canopy carbon consumed */
					canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;
					// Determine the amount of carbon consumed in the understory based on aggregate understory
					under_c_consumed = (canopy_target[0].fe.agg_under_carbon * canopy_target[0].fe.canopy_subtarget_prop_c_consumed);
					// combine with litter and set to understory_c_consumed, if both under and intr stories are present, will be overwritten.
					canopy_target[0].fe.understory_c_consumed = under_c_consumed + litter_c_consumed;
				}
				if (intr_ct > 0) // there's at least 1 intr story
				{
					/* For intermediate height subtarget canopy, adjust canopy_subtarget_prop_mort to only account for understory component, based on aggregate intrmediate height */
					canopy_target[0].fe.canopy_subtarget_prop_mort = canopy_target[0].fe.canopy_subtarget_prop_mort * intr_height_u_prop;
					// need to recalc prop mort consumption
					if (canopy_target[0].defaults[0][0].consumption == 1) {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_subtarget_prop_mort;
					} else {
						canopy_target[0].fe.canopy_subtarget_prop_mort_consumed =
							(pow(canopy_target[0].defaults[0][0].consumption, canopy_target[0].fe.canopy_subtarget_prop_mort) - 1) / (canopy_target[0].defaults[0][0].consumption - 1);
					}
					/* Determine the proportion of subtarget canopy carbon consumed */
					canopy_target[0].fe.canopy_subtarget_prop_c_consumed = canopy_target[0].fe.canopy_subtarget_prop_mort * canopy_target[0].fe.canopy_subtarget_prop_mort_consumed;
					/* Determine the amount of carbon consumed in the intmediate story */
					intr_c_consumed = canopy_target[0].fe.agg_intr_carbon * canopy_target[0].fe.canopy_subtarget_prop_c_consumed;
					// combine with litter and set to understory_c_consumed, if both under and intr stories are present, will be overwritten.
					canopy_target[0].fe.understory_c_consumed = intr_c_consumed + litter_c_consumed;
				}
				if (under_ct > 0 && intr_ct > 0)
				{
					// Combine understory and intermediate story consumptions, weighted by area(?), add litter in
					canopy_target[0].fe.understory_c_consumed = (under_c_consumed * canopy_target[0].fe.agg_under_pct_cover / (canopy_target[0].fe.agg_under_pct_cover + canopy_target[0].fe.agg_intr_pct_cover)) +
																(intr_c_consumed * canopy_target[0].fe.agg_intr_pct_cover / (canopy_target[0].fe.agg_under_pct_cover + canopy_target[0].fe.agg_intr_pct_cover)) + litter_c_consumed;
				}

				/* Determine the proportion of target canopy mortality based on the amount of understory consumed (sigmoidal relationship)
				and then account for target canopy height allocation */
				canopy_target[0].fe.canopy_target_prop_mort_o_component =
					(1 - (1 / (1 + exp(-(canopy_target[0].defaults[0][0].overstory_mort_k1 *
										 (canopy_target[0].fe.understory_c_consumed -
										  canopy_target[0].defaults[0][0].overstory_mort_k2)))))) *
					(1 - canopy_target[0].fe.canopy_target_height_u_prop);

				/* ------------------------------------------------------------------------ */
				/* Combine target canopy mortality from overstory and understory components */
				canopy_target[0].fe.canopy_target_prop_mort = max(min(canopy_target[0].fe.canopy_target_prop_mort_u_component + canopy_target[0].fe.canopy_target_prop_mort_o_component,1.0),0);

				if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = 
						(pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}

			/*--------------------------------------------------------------*/
			/* Calculate fire effects when target canopy is short			*/
			/*--------------------------------------------------------------*/
			} else if (canopy_target[0].fe.canopy_target_height < patch_family[0].understory_height_thresh) {

				/* Determine the proportion of target canopy mortality consumed */
				if (canopy_target[0].defaults[0][0].consumption == 1){
					canopy_target[0].fe.canopy_target_prop_mort_consumed = canopy_target[0].defaults[0][0].consumption * canopy_target[0].fe.canopy_target_prop_mort;
				} else {
					canopy_target[0].fe.canopy_target_prop_mort_consumed = (pow(canopy_target[0].defaults[0][0].consumption,canopy_target[0].fe.canopy_target_prop_mort)-1)/(canopy_target[0].defaults[0][0].consumption-1);
				}
			}

			/*--------------------------------------------------------------*/
			/* Compute effects												*/
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

			thin_type =2;	/* Harvest option */
			
			// to get the matching patch for update mortality
			//canopy_target = patch_family[0].canopy_strata[(patch_family[0].layers[layer].strata[c])]; // this is redundant i think
			patch = patch_family[0].patches[canopy_target[0].fam_patch_ind];

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
			canopy_target[0].fe.canopy_target_height_u_prop = max(min((patch_family[0].overstory_height_thresh - canopy_target[0].fe.canopy_target_height)/(patch_family[0].overstory_height_thresh-patch_family[0].understory_height_thresh),1.0),0);
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
                canopy_target[0].fe.acc_year.m_cwdc_to_atmos += canopy_target[0].fe.m_cwdc_to_atmos;
                canopy_target[0].fe.acc_year.m_cwdn_to_atmos += canopy_target[0].fe.m_cwdn_to_atmos;
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
                //canopy_target.fe.acc_year.canopy_subtarget_c +=
                canopy_target[0].fe.acc_year.understory_c_consumed +=  canopy_target[0].fe.understory_c_consumed;
				// added these here since subtarget kind of loses meaning with the new MSR fire effects method
				canopy_target[0].fe.acc_year.agg_under_height += canopy_target[0].fe.agg_under_height; 
				canopy_target[0].fe.acc_year.agg_under_carbon += canopy_target[0].fe.agg_under_carbon;
        		canopy_target[0].fe.acc_year.agg_intr_height += canopy_target[0].fe.agg_intr_height;
        		canopy_target[0].fe.acc_year.agg_intr_carbon += canopy_target[0].fe.agg_intr_carbon;


                canopy_target[0].fe.acc_year.length +=1;
            }
		} // end for at line 137 c
	}
	/*--------------------------------------------------------------*/
	/* compute fire severity class					*/
	/* for now just look at overstory to decide			*/
	/* and just look at first overstory (if two or more of same height) */
	/*--------------------------------------------------------------*/
	layer=0;
	c=0;
	canopy_target = patch_family[0].canopy_strata[(patch_family[0].layers[layer].strata[c])];
	if (canopy_target[0].fe.canopy_target_prop_mort < patch_family[0].patches[canopy_target[0].fam_patch_ind][0].landuse_defaults[0][0].mid_fire_severity_p)
		sev = LOW_FIRE;
	else if (canopy_target[0].fe.canopy_target_prop_mort < patch_family[0].patches[canopy_target[0].fam_patch_ind][0].landuse_defaults[0][0].high_fire_severity_p)
		sev = MID_FIRE;
	else
		sev = HIGH_FIRE;

	for (p = 0; p < patch_family[0].num_patches_in_fam; p++)
	{
		patch_family[0].patches[p][0].fire.severity = sev;
	}

	} /* end if(pspread > 0 ) */
    else 
	{
		for (layer = 0; layer < patch_family[0].num_layers; layer++)
		{
			for (c = 0; c < patch_family[0].layers[layer].count; c++)
			{
				/* Calculates metrics for targer canopy */
				canopy_target = patch_family[0].canopy_strata[(patch_family[0].layers[layer].strata[c])];
				canopy_target[0].fe.canopy_target_height = canopy_target[0].epv.height;

				/* Calculates metrics for next lowest canopy (subtarget canopy) */
				if (patch_family[0].num_layers > (layer + 1))
				{
					canopy_subtarget = patch_family[0].canopy_strata[(patch_family[0].layers[layer + 1].strata[c])];
					canopy_target[0].fe.canopy_subtarget_height = canopy_subtarget[0].epv.height;
					canopy_target[0].fe.canopy_subtarget_c = canopy_subtarget[0].cs.leafc +
															 canopy_subtarget[0].cs.live_stemc +
															 canopy_subtarget[0].cs.dead_stemc;
				}
				else
				{
					canopy_target[0].fe.canopy_subtarget_height = 0;
					canopy_target[0].fe.canopy_subtarget_c = 0;
				}
			} // end for c=0
		} //end for layer =0

		for (p = 0; p < patch_family[0].num_patches_in_fam; p++)
		{
			patch_family[0].patches[p][0].fire.severity = NO_FIRE;
		}

	} // end if pspread = 0

	return;
} /*end compute_family_fire_effects.c*/



