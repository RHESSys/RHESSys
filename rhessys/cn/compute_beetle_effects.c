
/*--------------------------------------------------------------*/
/* 												            	*/
/*			compute_beetle_effects								*/
/*				            									*/
/*	NAME		           										*/
/*	compute_beetle_effects.c 									*/
/*				            									*/
/*	SYNOPSIS	        										*/
/*				            									*/
/* 				              									*/
/*					  			            					*/
/*	OPTIONS					        							*/
/*						            							*/
/*							            						*/
/*	DESCRIPTION				        							*/
/*	Determines vegetation loss following fire					*/
/*											            		*/
/*											            		*/
/*										   	            		*/
/*	PROGRAMMER NOTES	N.REN 20180629		    				*/
/*										            			*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

void compute_beetle_effects(

						struct patch_object *patch,
						int inx,
						//double min_abc,
						//int root_alive,
						//int harvest_dead_root,
						double attack_mortality,
						struct beetle_default *beetle)

{

	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


	void	update_beetle_attack_mortality(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct snag_sequence_object *,  // track the multiple snag pool caused by multiple attack NREN 20180630
		struct snag_sequence_object *, // this is for leaf
		int, //this is the index
		int, // this is thin type
		//int, //root_alive
		//int, //harvest_dead_root
		struct mortality_struct,
		struct beetle_default *);

    void *alloc(size_t, char *, char *);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/

	struct canopy_strata_object *strata; // this for calculating above ground carbon
	struct canopy_strata_object *canopy_strata;  // this is for calculating the mortality
	struct snag_sequence_object *snag_sequence; //NREN 20180630
	struct snag_sequence_object *redneedle_sequence; // this is for leaf
	struct mortality_struct mort;
	struct fire_litter_soil_loss_struct fire_loss;
	int c, layer;
	double abc; // the total biomass in current patch
	int thin_type;
    double loss;

    abc=0.0;
    /* start calculating the total biomass in each patch */

    for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
				/*	printf("Layers: %d\n c: %d\n, count: %d\n patch_num_layers %d\n",layer, c, patch[0].layers[layer].count, patch[0].num_layers);*/

					strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];

				    abc += (strata[0].cs.leafc + strata[0].cs.leafc_store + strata[0].cs.leafc_transfer +
                                                  strata[0].cs.live_stemc + strata[0].cs.livestemc_store + strata[0].cs.livestemc_transfer +
                                                  strata[0].cs.dead_stemc + strata[0].cs.deadstemc_store + strata[0].cs.deadstemc_transfer);
				// above is calculate the total biomass in the beetle grid for higher outbreak
				//   printf("the total above ground biomass in current patch ID=%d, are %lf kg/m2\n", patch[0].ID, abc);

				}
			}


    /* build a simple mortality model  once the carbon is above  55Mg/ha is converted to 5500g/m2*/


	if (abc *1000 >= beetle[0].min_abc) {  // make it as an input parameter
	   loss = attack_mortality;
        if(patch[0].ID == 7788) {
	   printf("\n updating the beetle mortality (abc>min_abc) for inx %d,  [abc %lf kg/m2], [min_abc %lf g/m2], [mort %lf]\n", inx, abc, beetle[0].min_abc, loss);}// the index is the time series of beetle attack mortality 0 is the first one 24 is the second
	}
	else if (abc*1000 < beetle[0].min_abc) {
	   loss=0; // if the total carbon is less the 55Mg/ha, then there is no mortality in that patch
       if(patch[0].ID == 7788) {
	   printf("\n updating the beetle mortality (abc<min_abc) for inx %d,  [abc %lf kg/m2], [min_abc %lf g/m2], [mort %lf]\n", inx, abc,  beetle[0].min_abc, loss);}// the index is the time series of beetle attack mortality 0 is the first one 24 is the second
	}


	// assign values from loss to the mortality structure
            mort.mort_cpool = loss;
			mort.mort_leafc = loss;
			mort.mort_deadleafc =loss; //why no deadleafc ?
			mort.mort_frootc = loss;
			mort.mort_deadstemc = loss;
			mort.mort_livestemc = loss;
			mort.mort_deadcrootc = loss;
			mort.mort_livecrootc = loss;

			//snag_sequence = &(patch[0].snag_sequence); // it is full structure have all the time series, that is why we need index
			//redneedle_sequence= &(patch[0].redneedle_sequence);
            if (loss >0) {
            thin_type =5; //create a new thintype for beetle attack
			//for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				//	for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					//canopy_strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];

					/* if only apply the beetle attack mortality to the overstory trees, then just out of the loop */
					canopy_strata = patch[0].canopy_strata[patch[0].layers[0].strata[0]];
					/* track the snag pool sequences NREN 20180630*/

                  //  printf("\n updating the beetle mortality for inx %d \n", inx);// the index is the time series of beetle attack mortality 0 is the first one 24 is the second

					update_beetle_attack_mortality(canopy_strata[0].defaults[0][0].epc,
						 &(canopy_strata[0].cs),
						 &(canopy_strata[0].cdf),
						 &(patch[0].cdf),
						 &(canopy_strata[0].ns),
						 &(canopy_strata[0].ndf),
						 &(patch[0].ndf),
						 &(patch[0].litter_cs),
						 &(patch[0].litter_ns),
						 &(canopy_strata[0].snag_sequence),
						 &(canopy_strata[0].redneedle_sequence),
						 inx,
						 thin_type,
						// root_alive,
						// harvest_dead_root,
						 mort,
						 beetle);
				}
			//}
//			printf("in update mortality5 for beetle attack\n");
        //}
}
