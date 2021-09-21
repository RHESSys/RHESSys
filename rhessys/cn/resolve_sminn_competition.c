/*--------------------------------------------------------------*/
/* 								*/
/*								*/
/*	resolve_sminn_competition				*/
/*								*/
/*	NAME							*/
/*		resolve_sminn_competition			*/
/*								*/
/*	SYNOPSIS						*/
/*	int resolve_sminn_competition(				*/
/*   			    struct  soil_n_object   *ns_soil,	*/
/*     			    struct ndayflux_patch_struct *ndf)  */
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*              modified from Peter Thornton (1998)             */
/*                      dynamic - 1d-bgc ver4.0                 */
/*		plant uptake under limiting nitrogen is 	*/
/*		now a function of root density			*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int resolve_sminn_competition(
							  struct  soil_n_object   *ns_soil,
							  double surface_NO3,
							  double surface_NH4,
							  double rooting_depth,
							  double active_zone_z,
							  double N_decay_rate,
							  struct patch_object *patch,
							  struct ndayflux_patch_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double sum_ndemand, sum_avail;
	double actual_immob, actual_uptake, perc_inroot;
	/*--------------------------------------------------------------*/
	/* compare the combined decomposition immobilization and plant*/
	/*growth N demands against the available soil mineral N pool. */
	/*--------------------------------------------------------------*/
	sum_ndemand = ndf->plant_potential_ndemand + ndf->potential_immob;
	sum_avail = max(ns_soil->sminn + ns_soil->nitrate + ndf->mineralized, 0.0);
	/*--------------------------------------------------------------*/
	/* limit available N for plants by rooting depth		*/
	/* for really small rooting depths this can be problematic	*/
	/* for now provide a minimum access				*/
	/*--------------------------------------------------------------*/
	perc_inroot = (1.0-exp(-N_decay_rate * rooting_depth)) /
			(1.0 - exp(-N_decay_rate * active_zone_z));



	perc_inroot = min(perc_inroot,1.0);
	if (rooting_depth > ZERO) {
        perc_inroot = max(0.1, perc_inroot);
       // sum_avail = perc_inroot * sum_avail;//test if there is no root how the decom works Ren

	}

	//ndf->perc_inroot = perc_inroot;
	ns_soil->perc_inroot = perc_inroot;
    sum_avail = perc_inroot * sum_avail;
    // sum_avail also needs to consider the N_sat at saturated zone
    double avail_rtzN = 0.0;
    patch[0].rtzSatNH4 = 0.0;
    if(patch[0].available_soil_water > ZERO){
        //change the sat solute profile
        patch[0].rtzSatNH4 = patch[0].sat_NH4 * max(patch[0].rootzone.potential_sat-patch[0].sat_deficit,0.0)/patch[0].available_soil_water;
    }//if
    patch[0].rtzSatNO3 = 0.0;
    if(patch[0].available_soil_water > ZERO){
        patch[0].rtzSatNO3 = patch[0].sat_NO3 * max(patch[0].rootzone.potential_sat-patch[0].sat_deficit,0.0)/patch[0].available_soil_water;
    }//if
    //sum_avail = sum_avail + patch[0].rtzSatNO3 + patch[0].rtzSatNH4;
    sum_avail = sum_avail + patch[0].sat_NO3 + patch[0].sat_NH4;//test N balance
     //printf("\n|| [ns_soil.perc_inroot] %f, [perc_inroot] %f || ", ns_soil->perc_inroot, perc_inroot);


	if (sum_ndemand <= sum_avail){
	/* N availability is not limiting immobilization or plant
		uptake, and both can proceed at their potential rates */
		actual_immob = ndf->potential_immob;
		ns_soil->nlimit = 0;
		ns_soil->fract_potential_immob = 1.0;
		ns_soil->fract_potential_uptake = 1.0;
		ndf->plant_avail_uptake = ndf->plant_potential_ndemand;
		//printf("\n===== N demand[%f] < N avail[%f], perc_inroot[%f], rootDepth[%f], Ndecay[%f],active_z[%f]===========\n",
	//	sum_ndemand, sum_avail, perc_inroot, rooting_depth, N_decay_rate, active_zone_z);
	}
	else{
	/* N availability can not satisfy the sum of immobiliation and
	plant growth demands, so these two demands compete for available
		soil mineral N */
		ns_soil->nlimit = 1;
		actual_immob = (sum_avail) * (ndf->potential_immob/sum_ndemand);
		actual_uptake = sum_avail - actual_immob;

		if (ndf->potential_immob == 0.0)
			ns_soil->fract_potential_immob = 0.0;
		else
			ns_soil->fract_potential_immob = actual_immob/ndf->potential_immob;

        //printf("\n===== N demand[%f] > N avail[%f], actual_immob[%f], potential_immob[%f], fpi[%f], perc_inroot[%f], rootDepth[%f], Ndecay[%f],active_z[%f]===========\n",
        //        sum_ndemand, sum_avail, actual_immob, ndf->potential_immob, ns_soil->fract_potential_immob, perc_inroot, rooting_depth, N_decay_rate, active_zone_z);

		if (ndf->plant_potential_ndemand == 0.0) {
			ns_soil->fract_potential_uptake = 0.0;
			//ndf->plant_avail_uptake = actual_uptake;
			ndf->plant_avail_uptake = 0.0; // zero
		}
		else {
			ns_soil->fract_potential_uptake = actual_uptake
				/ ndf->plant_potential_ndemand;
			ndf->plant_avail_uptake = actual_uptake;
		}
	}
    if (patch[0].sat_NH4 != patch[0].sat_NH4 || patch[0].sat_NH4 <  -0.00001){
        printf("\nresolve sminn competition NH4 %e< ZERO", patch[0].sat_NH4);}
	return(0);
} /* end resolve_sminn_competition.c */
