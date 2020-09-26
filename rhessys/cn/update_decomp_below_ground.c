/*--------------------------------------------------------------*/
/* 								*/
/*		update_decomp					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_decomp -  					*/
/*		performs decomposition and updates soil/litter	*/
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
/*		modified from Peter Thornton (1998)		*/
/*			dynamic - 1d-bgc ver4.0			*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_decomp_bg(
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

	total_preday_N = ns_litr->litr1n + ns_litr->litr2n +  ns_litr->litr3n
		+ ns_litr->litr4n + ns_soil->soil1n + ns_soil->soil2n + ns_soil->soil3n
		+ ns_soil->soil4n + ns_soil->sminn + ns_soil->nitrate
        + ns_litr->litr1n_bg + ns_litr->litr2n_bg + ns_litr->litr3n_bg + ns_litr->litr4n_bg; //add the below ground litter N
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
		if (nlimit && ndf->pmnf_l1s1_bg > 0.0){
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
		if (nlimit && ndf->pmnf_l2s2_bg > 0.0){
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
		if (nlimit && ndf->pmnf_l3l2_bg > 0.0){
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
		if (nlimit && ndf->pmnf_l4s3_bg > 0.0){
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

	/* fast microbial recycling pool */
	/* only update this soil pool in the update_decomp.c */
	/*if (cs_soil->soil1c > ZERO){
		if (nlimit && ndf->pmnf_s1s2 > 0.0){
			cdf->psoil1c_loss *= fpi;
			ndf->pmnf_s1s2 *= fpi;
		}
		cdf->soil1c_hr = rfs1s2 * cdf->psoil1c_loss;
		cdf->soil1c_to_soil2c = (1.0 - rfs1s2) * cdf->psoil1c_loss; // soil1c to soil2c
		ndf->soil1n_to_soil2n = cdf->psoil1c_loss / cn_s1;
		ndf->sminn_to_soil2n_s1 = ndf->pmnf_s1s2;
		daily_net_nmin -= ndf->pmnf_s1s2;
	}
	/* medium microbial recycling pool */
	/*if (cs_soil->soil2c > ZERO){
		if (nlimit && ndf->pmnf_s2s3 > 0.0){
			cdf->psoil2c_loss *= fpi;
			ndf->pmnf_s2s3 *= fpi;
		}
		cdf->soil2c_hr = rfs2s3 * cdf->psoil2c_loss;
		cdf->soil2c_to_soil3c = (1.0 - rfs2s3) * cdf->psoil2c_loss; //soil2c to soil3c
		ndf->soil2n_to_soil3n = cdf->psoil2c_loss / cn_s2;
		ndf->sminn_to_soil3n_s2 = ndf->pmnf_s2s3;
		daily_net_nmin -= ndf->pmnf_s2s3;
	}
	/* slow microbial recycling pool */
	/*if (cs_soil->soil3c > ZERO){
		if (nlimit && ndf->pmnf_s3s4 > 0.0){
			cdf->psoil3c_loss *= fpi;
			ndf->pmnf_s3s4 *= fpi;
		}
		cdf->soil3c_hr = rfs3s4 * cdf->psoil3c_loss;
		cdf->soil3c_to_soil4c = (1.0 - rfs3s4) * cdf->psoil3c_loss; //soil3c to soil4c
		ndf->soil3n_to_soil4n = cdf->psoil3c_loss / cn_s3;
		ndf->sminn_to_soil4n_s3 = ndf->pmnf_s3s4;
		daily_net_nmin -= ndf->pmnf_s3s4;
	}
	/* recalcitrant SOM pool (rf = 1.0, always mineralizing) */
	/*if (cs_soil->soil4c > ZERO){
		cdf->soil4c_hr = cdf->psoil4c_loss;
		ndf->soil4n_to_sminn = cdf->psoil4c_loss / cn_s4;
		daily_net_nmin += ndf->soil4n_to_sminn;
	}
	/* update soild and litter stores */
	/* Fluxes out of labile litter pool */
	cs_litr->litr1c_hr_snk_bg += cdf->litr1c_hr_bg; // this related to basin yearly growth add this to output too
	cs_litr->litr1c_bg       -= cdf->litr1c_hr_bg;
	if (cs_litr->litr1c_bg - cdf->litr1c_to_soil1c_bg < 0.0) {
		cdf->litr1c_to_soil1c_bg = max(cs_litr->litr1c_bg,0.0);
		ndf->litr1n_to_soil1n_bg = cdf->litr1c_to_soil1c_bg / cn_l1 ;
	}
	if (ns_litr->litr1n_bg - ndf->litr1n_to_soil1n_bg < 0.0) {
		ndf->litr1n_to_soil1n_bg = max(ns_litr->litr1n_bg,0.0);
		cdf->litr1c_to_soil1c_bg = cdf->litr1c_to_soil1c_bg * cn_l1 ;
	}
	cs_soil->soil1c       += cdf->litr1c_to_soil1c_bg;
	cs_litr->litr1c_bg       -= cdf->litr1c_to_soil1c_bg;
	/* Fluxes out of cellulose litter pool */
	cs_litr->litr2c_hr_snk_bg += cdf->litr2c_hr_bg;
	cs_litr->litr2c_bg       -= cdf->litr2c_hr_bg;
	if (cs_litr->litr2c_bg - cdf->litr2c_to_soil2c_bg < 0.0) {
		cdf->litr2c_to_soil2c_bg = max(cs_litr->litr2c_bg,0.0);
		ndf->litr2n_to_soil2n_bg = max(ns_litr->litr2n_bg,0.0);
	}
	cs_soil->soil2c       += cdf->litr2c_to_soil2c_bg;
	cs_litr->litr2c_bg       -= cdf->litr2c_to_soil2c_bg;
	/* Fluxes from shielded to unshielded cellulose pools */
	if (cs_litr->litr3c_bg - cdf->litr3c_to_litr2c_bg < 0.0) {
		cdf->litr3c_to_litr2c_bg = max(cs_litr->litr3c_bg,0.0);
		ndf->litr3n_to_litr2n_bg = max(ns_litr->litr3n_bg,0.0);
	}
	cs_litr->litr2c_bg       += cdf->litr3c_to_litr2c_bg;
	cs_litr->litr3c_bg       -= cdf->litr3c_to_litr2c_bg;
	/* Fluxes out of lignin litter pool */
	cs_litr->litr4c_hr_snk_bg += cdf->litr4c_hr_bg;
	cs_litr->litr4c_bg       -= cdf->litr4c_hr_bg;
	if (cs_litr->litr4c_bg - cdf->litr4c_to_soil3c_bg < 0.0) {
		cdf->litr4c_to_soil3c_bg = max(cs_litr->litr4c_bg,0.0);
		ndf->litr4n_to_soil3n_bg = max(ns_litr->litr4n_bg,0.0);
	}
	cs_soil->soil3c       += cdf->litr4c_to_soil3c_bg;
	cs_litr->litr4c_bg       -= cdf->litr4c_to_soil3c_bg;
	/* Fluxes out of fast soil pool */
	/* just need to update once in the update_decomp */
	/*cs_soil->soil1c_hr_snk += cdf->soil1c_hr;
	cs_soil->soil1c       -= cdf->soil1c_hr;
	if (cs_soil->soil1c - cdf->soil1c_to_soil2c < 0.0) {
		cdf->soil1c_to_soil2c = max(cs_soil->soil1c, 0.0);
		ndf->soil1n_to_soil2n = max(ns_soil->soil1n, 0.0);
	}
	cs_soil->soil2c       += cdf->soil1c_to_soil2c;
	cs_soil->soil1c       -= cdf->soil1c_to_soil2c;
	/* Fluxes out of medium soil pool */
	/*cs_soil->soil2c_hr_snk += cdf->soil2c_hr;
	cs_soil->soil2c       -= cdf->soil2c_hr;
	if (cs_soil->soil2c - cdf->soil2c_to_soil3c < 0.0) {
		cdf->soil2c_to_soil3c = max(cs_soil->soil2c, 0.0);
		ndf->soil2n_to_soil3n = max(ns_soil->soil2n, 0.0);
	}
	cs_soil->soil3c       += cdf->soil2c_to_soil3c;
	cs_soil->soil2c       -= cdf->soil2c_to_soil3c;
	/* Fluxes out of slow soil pool */
	/*cs_soil->soil3c_hr_snk += cdf->soil3c_hr;
	cs_soil->soil3c       -= cdf->soil3c_hr;
	if (cs_soil->soil3c - cdf->soil3c_to_soil4c < 0.0) {
		cdf->soil3c_to_soil4c = max(cs_soil->soil3c, 0.0);
		ndf->soil3n_to_soil4n = max(ns_soil->soil3n, 0.0);
	}
	cs_soil->soil4c       += cdf->soil3c_to_soil4c;
	cs_soil->soil3c       -= cdf->soil3c_to_soil4c;
	/* Fluxes out of recalcitrant SOM pool */
	/*cs_soil->soil4c_hr_snk += cdf->soil4c_hr;
	cs_soil->soil4c       -= cdf->soil4c_hr; */

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

	/* Fluxes out of fast soil pool */
	/*ns_soil->soil2n       += ndf->soil1n_to_soil2n;
	ns_soil->soil1n       -= ndf->soil1n_to_soil2n;
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_s1;
	/* Fluxes out of medium soil pool */
	/*ns_soil->soil3n       += ndf->soil2n_to_soil3n;
	ns_soil->soil2n       -= ndf->soil2n_to_soil3n;
	ns_soil->soil3n	      += ndf->sminn_to_soil3n_s2;
	/* Fluxes out of slow soil pool */
	/*ns_soil->soil4n       += ndf->soil3n_to_soil4n;
	ns_soil->soil3n       -= ndf->soil3n_to_soil4n;
	ns_soil->soil4n	      += ndf->sminn_to_soil4n_s3;
	ns_soil->soil4n	      -= ndf->soil4n_to_sminn;
	/* Fluxes into mineralized N pool */
	/* Fluxes output of mineralized N pool for net microbial immobilization */
	if (daily_net_nmin > ZERO)
		ns_soil->sminn += daily_net_nmin; // here connect sminn with the above ground litter so it is the same siminn pool
	else {
		if (-1.0*daily_net_nmin > ns_soil->sminn + ns_soil->nitrate + ZERO) {

			/* this should not happen  but if it does warn user and but let sminn go negative*/
			printf("In update below ground decomp not enough for mineral N will reduce accordingly \n");
			balance = ns_soil->sminn + ns_soil->nitrate + daily_net_nmin;
			printf("\n below ground litter required %lf balance unmet %lf \n", -1.0*daily_net_nmin, balance);
			daily_net_nmin = -1.0 * (ns_soil->sminn + ns_soil->nitrate);

		}
		nitrate_immob = min(ns_soil->nitrate, -1.0*daily_net_nmin);
		ns_soil->nitrate -= max(nitrate_immob,0.0);
		ns_soil->sminn -= (-1.0*daily_net_nmin - nitrate_immob);
	}
	/* Fluxes output of mineralized N pool from plant uptake */
	/*	we use N from the following first soil nitrate, */
	/*	second soil NH4; and thirds surface NO3, 4th surface NH4 */
	//remaining_uptake = ndf->sminn_to_npool; //comment out
	/* remove from soil nitrate pool */
	/*N_uptake = max((min(ns_soil->nitrate, remaining_uptake)),0.0); //comment out
	ns_soil->nitrate -= N_uptake;
	remaining_uptake -= N_uptake;

	/* remove from soil NH4 pool */
	/*N_uptake = max((min(ns_soil->sminn, remaining_uptake)),0.0); //comment out
	ns_soil->sminn -=  N_uptake;
	remaining_uptake -= N_uptake;

	/* remove from surface NO3 pool */
	/*N_uptake = max((min(patch[0].surface_NO3, remaining_uptake)),0.0); //comment out
	patch[0].surface_NO3 -=  N_uptake;
	remaining_uptake -= N_uptake;

	/* remove from surface NH4 pool */ //comment out
	/*N_uptake = max((min(patch[0].surface_NH4, remaining_uptake)),0.0);
	patch[0].surface_NH4 -=  N_uptake;
	remaining_uptake -= N_uptake; */

	if (remaining_uptake > ZERO) printf("N balance issue for below ground litter \n");
	ndf->net_mineralized_bg = daily_net_nmin; //use different pool as the above ground litter
	total_N = ns_litr->litr1n + ns_litr->litr2n +  ns_litr->litr3n
		+ ns_litr->litr4n + ns_soil->soil1n + ns_soil->soil2n
		+ ns_soil->soil3n + ns_soil->soil4n + ns_soil->sminn + ns_soil->nitrate;
		+ ns_litr->litr1n_bg + ns_litr->litr2n_bg + ns_litr->litr3n_bg + ns_litr->litr4n_bg; //add the below ground litter N
	balance = (total_preday_N)  - (total_N + ndf->sminn_to_npool);
	if (abs(balance) > ZERO)
		printf("\n Decomp N doesn't balance for betlow ground litter by %lf ", balance);

	return (!ok);
} /* end update_decomp.c */
