/*--------------------------------------------------------------*/
/* 																*/
/*					treat_patch								*/
/*																*/
/*	treat_patch.c - identifies patches to be treated and calls treatment function					*/
/*																*/
/*	NAME														*/
/*	treat_patch.c */
/*																*/
/*	SYNOPSIS													*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/

/*																*/
		/*--------------------------------------------------------------*/
		/*	fuel treatment parameters 				*/
		/*	fuel treatment target				*/
		/* 1 - understory only						*/
		/* 2 - overstory only						*/
		/* 3 - both						*/
		/* 4 - understory + litter 		*/
		/* 5  - everything (litter, understory, overstory ) 	*/
		/*--------------------------------------------------------------*/
/*																*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

void treat_patch(
									 struct	patch_object *patch)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


    void    update_mortality(
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

void    update_litter_soil_mortality(
                struct cdayflux_patch_struct *,
                struct ndayflux_patch_struct *,
                struct soil_c_object *,
                struct soil_n_object *,
                struct litter_c_object *,
                struct litter_n_object *,
                struct fire_litter_soil_loss_struct);


	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int     layer;
        int stratum, thintyp, treat, treat_litter;
	double biomass_removal_percent;
 	struct  mortality_struct mort;
	struct  canopy_strata_object    *strata;
	struct fire_litter_soil_loss_struct litter_loss;
	/*--------------------------------------------------------------*/
	/* cycle through layers top to bottom */
	/*-------------------------------------------------------------*/

	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){


		
	/*	Cycle through the canopy strata				*/
	for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){

		/* determine which canopies to treat */
		/* 1 - understory only						*/
		/* 2 - overstory only						*/
		/* 3 - both						*/
		/* 4 - understory + litter 		*/
		/* 5  - everything (litter, understory, overstory ) 	*/

		/* we start by assuming everything is treated (fuel treatment type is 3 or 5) */
		treat = 1;
		switch(patch[0].landuse_defaults[0][0].fuel_treatment_type) {
			case 2: if(layer < patch[0].num_layers -1)
						treat = 1; else treat = 0;break;
			case 1:  if(layer > 0 || patch[0].num_layers==1)
						treat = 1; else treat = 0;break;
			case 4: if(layer < patch[0].num_layers -1)
						treat = 1; else treat = 0;break;
			}
	
		if (treat == 1) {	
		/* for now we remove everything for a treated stratum - we may have less intense scenarios later */
		/* however only part of the patch family is generally treated so this can still be a low intensity thin */
		biomass_removal_percent = 1.0; 


		mort.mort_cpool=biomass_removal_percent;
		mort.mort_leafc=biomass_removal_percent;
		mort.mort_deadleafc=biomass_removal_percent;
		mort.mort_deadstemc=biomass_removal_percent;
		mort.mort_livestemc=biomass_removal_percent;
		mort.mort_deadcrootc=biomass_removal_percent;
		mort.mort_livecrootc=biomass_removal_percent;
		mort.mort_frootc=biomass_removal_percent;


		/* for now thintype is 2 - which means harvest - material is removed from the site */
		/* if we are treating litter we don't leave material */
		/* we can add thinning that leaves biomass later */
		thintyp = 2;

		strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])];
					update_mortality(strata[0].defaults[0][0].epc,
						&(strata[0].cs),
						&(strata[0].cdf),
						&(patch[0].cdf),
						&(strata[0].ns),
						&(strata[0].ndf),
						&(patch[0].ndf),
						&(patch[0].litter_cs),
						&(patch[0].litter_ns),
						thintyp,
						mort);

					} /* end treat */


		  /* if we are treating litter then also remove coarse woody debris */
		if(patch[0].landuse_defaults[0][0].fuel_treatment_type > 3) {
			strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])];
			strata[0].cs.cwdc = 0.0;
			strata[0].ns.cwdn = 0.0;
			}

				} /* end stratum */
			} /* end layer */



	/*--------------------------------------------------------------*/
		/* now deal with litter */
	/*--------------------------------------------------------------*/

		if(patch[0].landuse_defaults[0][0].fuel_treatment_type > 3)
				treat_litter =  1 ;
		else
				treat_litter = 0;

		if (treat_litter > 0) { 

		/* for now remove all the litter and a little bit of the top organic small soil */

		litter_loss.loss_litr1c = 1;
        	litter_loss.loss_litr2c = 1;
        	litter_loss.loss_litr3c = 1;
        	litter_loss.loss_litr4c = 1;
        	litter_loss.loss_soil1c = 0.2;
        	litter_loss.loss_soil2c = 0;
        	litter_loss.loss_soil3c = 0;
        	litter_loss.loss_soil4c = 0;
        	litter_loss.loss_litr1n = 1;
        	litter_loss.loss_litr2n = 1;
        	litter_loss.loss_litr3n = 1;
        	litter_loss.loss_litr4n = 1;
        	litter_loss.loss_soil1n = 0.2;
        	litter_loss.loss_soil2n = 0;
        	litter_loss.loss_soil3n = 0;
        	litter_loss.loss_soil4n = 0;

 		update_litter_soil_mortality(
                 &(patch[0].cdf),
                 &(patch[0].ndf),
                 &(patch[0].soil_cs),
                 &(patch[0].soil_ns),
                 &(patch[0].litter_cs),
                 &(patch[0].litter_ns),
                 litter_loss);
		}

		
		
	return;

} /*end treat_patch.c*/

