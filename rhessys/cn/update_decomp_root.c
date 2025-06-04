/*--------------------------------------------------------------*/
/* 								*/
/*		update_decomp_root					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_decomp -  					*/
/*		performs decomposition of roots and updates soil/litter	*/
/*		carbon and nitrogen stores			*/
/*								*/
/*	SYNOPSIS						*/
/*	int update_decomp(					*/
/*			double,					*/
/*			double,					*/
/*			double,					*/
/*			double,					*/
/*			struct	soil_c_object	*		*/
/*			struct	soil_n_object	*		*/
/*			struct	litter_c_object	*		*/
/*			struct	litter_n_object	*		*/
/*			struct	cdayflux_patch_object *		*/
/*			struct	ndayflux_patch_object *		*/
/*				)				*/
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
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_decomp_root(
				  struct	date	current_date,
				  struct  soil_c_object   *cs_soil,
				  struct  soil_n_object   *ns_soil,
				  struct  litter_c_object *cs_litr,
				  struct  litter_n_object *ns_litr,
				  struct cdayflux_patch_struct *cdf,
				  struct ndayflux_patch_struct *ndf,
				  struct patch_object	*patch)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok = 1;
	double rfl1s1, rfl2s2,rfl4s3,rfs1s2,rfs2s3,rfs3s4;
	double cn_l1,cn_l3, cn_l2,cn_l4,cn_s1,cn_s2,cn_s3,cn_s4;
	double daily_net_nmin;
	double nlimit, fpi;
	double total_N, total_preday_N, balance;
	double nitrate_immob, N_uptake, remaining_uptake;
	
	//add the below ground litter N

	total_preday_N = ns_litr->litr1n + ns_litr->litr2n +  ns_litr->litr3n
		+ ns_litr->litr4n + ns_soil->soil1n + ns_soil->soil2n + ns_soil->soil3n
		+ ns_soil->soil4n + ns_soil->sminn + ns_soil->nitrate
        	+ ns_litr->litr1n_bg + ns_litr->litr2n_bg + ns_litr->litr3n_bg + ns_litr->litr4n_bg; 

	nlimit = ns_soil->nlimit;
	fpi = ns_soil->fract_potential_immob;
	/* now use the N limitation information fpi to assess the final decomposition
	fluxes. Mineralizing fluxes (pmnf* < 0.0) occur at the potential rate
	regardless of the competing N demands between microbial processes and
	plant uptake, but immobilizing fluxes are reduced when soil mineral
	N is limiting */
	/* calculate litter and soil compartment C:N ratios */
	if (ns_litr->litr1n_bg > ZERO) cn_l1 = cs_litr->litr1c_bg/ns_litr->litr1n_bg;
		else cn_l1 = LIVELAB_CN;
	if (ns_litr->litr2n_bg > ZERO) cn_l2 = cs_litr->litr2c_bg/ns_litr->litr2n_bg;
		else cn_l2 = CEL_CN;
	if (ns_litr->litr3n_bg > ZERO) cn_l3 = cs_litr->litr3c_bg/ns_litr->litr3n_bg;
		else cn_l3 = LIG_CN;
	if (ns_litr->litr4n_bg > ZERO) cn_l4 = cs_litr->litr4c_bg/ns_litr->litr4n_bg;
		else cn_l4 = LIG_CN;
	cn_s1 = SOIL1_CN;
	cn_s2 = SOIL2_CN;
	cn_s3 = SOIL3_CN;
	cn_s4 = SOIL4_CN;
	/* respiration fractions for fluxes between compartments */
	rfl1s1 = 0.39;
	rfl2s2 = 0.55;
	rfl4s3 = 0.29;
	rfs1s2 = 0.28;
	rfs2s3 = 0.46;
	rfs3s4 = 0.55;
	daily_net_nmin = 0.0;
	/* labile litter fluxes */
	if (cs_litr->litr1c_bg > ZERO) {
		if (nlimit && ndf->pmnf_l1s1_bg > ZERO){
			cdf->plitr1c_loss_bg *= fpi; // creat new below ground fluxes
			ndf->pmnf_l1s1_bg *= fpi;
		}
		cdf->litr1c_hr_bg = rfl1s1 * cdf->plitr1c_loss_bg;
		cdf->litr1c_to_soil1c_bg = (1.0 - rfl1s1) * cdf->plitr1c_loss_bg; //litter 1c to soil 1c
		if (ns_litr->litr1n_bg > ZERO)
			ndf->litr1n_to_soil1n_bg = cdf->plitr1c_loss_bg / cn_l1;
		else ndf->litr1n_to_soil1n_bg = 0.0;
		ndf->sminn_to_soil1n_l1_bg = ndf->pmnf_l1s1_bg;
		daily_net_nmin -= ndf->pmnf_l1s1_bg;
	}
	/* cellulose litter fluxes */
	if (cs_litr->litr2c_bg > ZERO){
		if (nlimit && ndf->pmnf_l2s2_bg > ZERO){
			cdf->plitr2c_loss_bg *= fpi;
			ndf->pmnf_l2s2_bg *= fpi;
		}
		cdf->litr2c_hr_bg = rfl2s2 * cdf->plitr2c_loss_bg;
		cdf->litr2c_to_soil2c_bg = (1.0 - rfl2s2) * cdf->plitr2c_loss_bg; //litter 2c to soil 2c
		if (ns_litr->litr2n_bg > ZERO)
			ndf->litr2n_to_soil2n_bg = cdf->plitr2c_loss_bg / cn_l2;
		else ndf->litr2n_to_soil2n_bg = 0.0;
		ndf->sminn_to_soil2n_l2_bg = ndf->pmnf_l2s2_bg;
		daily_net_nmin -= ndf->pmnf_l2s2_bg;
	}
	/* release of shielded cellulose litter, tied to the decay rate of
	lignin litter */
	/* actually going to litr 2 rather than soil but will use soil2 as repository for mineralized N */
	if (cs_litr->litr3c_bg > ZERO){
		if (nlimit && ndf->pmnf_l3l2_bg > ZERO){
			cdf->plitr3c_loss_bg *= fpi;
			ndf->pmnf_l3l2_bg *= fpi;
		}
		cdf->litr3c_hr_bg = rfl4s3 * cdf->plitr3c_loss_bg;
		cdf->litr3c_to_litr2c_bg = (1.0 - rfl4s3) * cdf->plitr3c_loss_bg; // litter3c to litter 2c
		if (ns_litr->litr3n_bg > 0.000000001)
			ndf->litr3n_to_litr2n_bg = cdf->plitr3c_loss_bg / cn_l3;
		else ndf->litr3n_to_litr2n_bg = 0.0;
		ndf->sminn_to_soil2n_l3_bg = ndf->pmnf_l3l2_bg;
		daily_net_nmin -= ndf->pmnf_l3l2_bg;
	}

	/* lignin litter fluxes */
	if (cs_litr->litr4c_bg > ZERO){
		if (nlimit && ndf->pmnf_l4s3_bg > ZERO){
			cdf->plitr4c_loss_bg *= fpi;
			ndf->pmnf_l4s3_bg *= fpi;
		}
		cdf->litr4c_hr_bg = rfl4s3 * cdf->plitr4c_loss_bg;
		cdf->litr4c_to_soil3c_bg = (1.0 - rfl4s3) * cdf->plitr4c_loss_bg; //litter4c to soil 3c
		if (ns_litr->litr4n_bg > 0.000000001)
			ndf->litr4n_to_soil3n_bg = cdf->plitr4c_loss_bg / cn_l4;
		else ndf->litr4n_to_soil3n_bg = 0.0;
		ndf->sminn_to_soil3n_l4_bg = ndf->pmnf_l4s3_bg;
		daily_net_nmin -= ndf->pmnf_l4s3_bg;
	}

	/* update soild and litter stores */
	/* Fluxes out of labile litter pool */
	cs_litr->litr1c_hr_snk_bg += cdf->litr1c_hr_bg; // this related to basin yearly growth add this to output too
	cs_litr->litr1c_bg       -= cdf->litr1c_hr_bg;
	if (cs_litr->litr1c_bg - cdf->litr1c_to_soil1c_bg < ZERO) {
		cdf->litr1c_to_soil1c_bg = max(cs_litr->litr1c_bg,0.0);
		ndf->litr1n_to_soil1n_bg = cdf->litr1c_to_soil1c_bg / cn_l1 ;
	}
	if (ns_litr->litr1n_bg - ndf->litr1n_to_soil1n_bg < ZERO) {
		ndf->litr1n_to_soil1n_bg = max(ns_litr->litr1n_bg,0.0);
		cdf->litr1c_to_soil1c_bg = cdf->litr1c_to_soil1c_bg * cn_l1 ;
	}
	cs_soil->soil1c       += cdf->litr1c_to_soil1c_bg;
	cs_litr->litr1c_bg       -= cdf->litr1c_to_soil1c_bg;
	/* Fluxes out of cellulose litter pool */
	cs_litr->litr2c_hr_snk_bg += cdf->litr2c_hr_bg;
	cs_litr->litr2c_bg       -= cdf->litr2c_hr_bg;
	if (cs_litr->litr2c_bg - cdf->litr2c_to_soil2c_bg < ZERO) {
		cdf->litr2c_to_soil2c_bg = max(cs_litr->litr2c_bg,0.0);
		ndf->litr2n_to_soil2n_bg = max(ns_litr->litr2n_bg,0.0);
	}
	cs_soil->soil2c       += cdf->litr2c_to_soil2c_bg;
	cs_litr->litr2c_bg       -= cdf->litr2c_to_soil2c_bg;
	/* Fluxes from shielded to unshielded cellulose pools */
	if (cs_litr->litr3c_bg - cdf->litr3c_to_litr2c_bg < ZERO) {
		cdf->litr3c_to_litr2c_bg = max(cs_litr->litr3c_bg,0.0);
		ndf->litr3n_to_litr2n_bg = max(ns_litr->litr3n_bg,0.0);
	}
	cs_litr->litr2c_bg       += cdf->litr3c_to_litr2c_bg;
	cs_litr->litr3c_bg       -= cdf->litr3c_to_litr2c_bg;
	/* Fluxes out of lignin litter pool */
	cs_litr->litr4c_hr_snk_bg += cdf->litr4c_hr_bg;
	cs_litr->litr4c_bg       -= cdf->litr4c_hr_bg;
	if (cs_litr->litr4c_bg - cdf->litr4c_to_soil3c_bg < ZERO) {
		cdf->litr4c_to_soil3c_bg = max(cs_litr->litr4c_bg,0.0);
		ndf->litr4n_to_soil3n_bg = max(ns_litr->litr4n_bg,0.0);
	}
	cs_soil->soil3c       += cdf->litr4c_to_soil3c_bg;
	cs_litr->litr4c_bg       -= cdf->litr4c_to_soil3c_bg;

	/* Fluxes out of labile litter pool */
	ns_soil->soil1n       += ndf->litr1n_to_soil1n_bg;
	ns_litr->litr1n_bg       -= ndf->litr1n_to_soil1n_bg;
	ns_soil->soil1n	      += ndf->sminn_to_soil1n_l1_bg;
	/* Fluxes out of cellulose litter pool */
	ns_soil->soil2n       += ndf->litr2n_to_soil2n_bg;
	ns_litr->litr2n_bg       -= ndf->litr2n_to_soil2n_bg;
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_l2_bg;
	/* Fluxes from shielded to unshielded cellulose pools */
	ns_litr->litr2n_bg       += ndf->litr3n_to_litr2n_bg;
	ns_litr->litr3n_bg       -= ndf->litr3n_to_litr2n_bg;
	/* this one is odd because we don't know where to get the N for shifting between litter 2 and 3 */
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_l3_bg;
	/* Fluxes out of lignin litter pool */
	ns_soil->soil3n       += ndf->litr4n_to_soil3n_bg;
	ns_litr->litr4n_bg       -= ndf->litr4n_to_soil3n_bg;
	ns_soil->soil3n	      += ndf->sminn_to_soil3n_l4_bg;

	/* Fluxes output of mineralized N pool for net microbial immobilization */
	if (daily_net_nmin > ZERO)
		ns_soil->sminn += daily_net_nmin; // here connect sminn with the above ground litter so it is the same siminn pool
	else {
		if (-1.0*daily_net_nmin > ns_soil->sminn + ns_soil->nitrate + ZERO) {

			/* this should not happen  but if it does warn user and but let sminn go negative*/
			//printf("In update below ground decomp not enough for mineral N will reduce accordingly \n");
			balance = ns_soil->sminn + ns_soil->nitrate + daily_net_nmin;
			//printf("\n below ground litter required %lf balance unmet %lf \n", -1.0*daily_net_nmin, balance);
			daily_net_nmin = -1.0 * (ns_soil->sminn + ns_soil->nitrate);

		}
		nitrate_immob = min(ns_soil->nitrate, -1.0*daily_net_nmin);
		ns_soil->nitrate -= max(nitrate_immob,0.0);
		ns_soil->sminn -= (-1.0*daily_net_nmin - nitrate_immob);
	}
	ndf->net_mineralized_bg = daily_net_nmin; //use different pool as the above ground litter

	return (!ok);
} /* end update_decomp.c */
