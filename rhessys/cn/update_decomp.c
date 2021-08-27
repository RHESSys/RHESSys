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

int update_decomp(
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
	double temp;
	double totalNO3, totalNH4, ratio_NO3, ratio_NH4;

	total_preday_N = ns_litr->litr1n + ns_litr->litr2n +  ns_litr->litr3n
		+ ns_litr->litr4n + ns_soil->soil1n + ns_soil->soil2n + ns_soil->soil3n
		+ ns_soil->soil4n + ns_soil->sminn + ns_soil->nitrate + patch[0].rtzSatNO3 + patch[0].rtzSatNH4; //rtzSatNO3 is calculated in resolve_sminn_cometition.c line 85 based on root depth

	nlimit = ns_soil->nlimit;
	fpi = ns_soil->fract_potential_immob;

  // check the range
	if(fpi>1.0){fpi=1.0;}
	if(fpi<0.0){fpi=0.0;}
	/* now use the N limitation information fpi to assess the final decomposition
	fluxes. Mineralizing fluxes (pmnf* < 0.0) occur at the potential rate
	regardless of the competing N demands between microbial processes and
	plant uptake, but immobilizing fluxes are reduced when soil mineral
	N is limiting */
	/* calculate litter and soil compartment C:N ratios */
	if ((cs_litr->litr1c > 0.0) && (ns_litr->litr1n > 0.0)) cn_l1 = cs_litr->litr1c/ns_litr->litr1n;
		else cn_l1 = LIVELAB_CN;
	if ((cs_litr->litr2c > 0.0) && (ns_litr->litr2n > 0.0)) cn_l2 = cs_litr->litr2c/ns_litr->litr2n;
		else cn_l2 = CEL_CN;
		if ((cs_litr->litr3c > 0.0) && (ns_litr->litr3n > 0.0))  cn_l3 = cs_litr->litr3c/ns_litr->litr3n;
		else cn_l3 = LIG_CN;
		if ((cs_litr->litr4c > 0.0) && (ns_litr->litr4n > 0.0)) cn_l4 = cs_litr->litr4c/ns_litr->litr4n;
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
	if ((cs_litr->litr1c > ZERO) && (ns_litr->litr1n > ZERO)) {
		if (nlimit && ndf->pmnf_l1s1 > 0.0){
			cdf->plitr1c_loss *= fpi;
			ndf->pmnf_l1s1 *= fpi;
		}
		cdf->litr1c_hr = rfl1s1 * cdf->plitr1c_loss;
		cdf->litr1c_to_soil1c = (1.0 - rfl1s1) * cdf->plitr1c_loss;
		if (ns_litr->litr1n > ZERO)
			ndf->litr1n_to_soil1n = cdf->plitr1c_loss / cn_l1;
		else ndf->litr1n_to_soil1n = 0.0;
		ndf->sminn_to_soil1n_l1 = ndf->pmnf_l1s1;
		daily_net_nmin -= ndf->pmnf_l1s1;
	} else{ // check which patch has negative litr1c

	  if ((cs_litr->litr1c < 0.0) || (ns_litr->litr1n < 0.0)); //printf("lc1=%e, ln1=%e\n",cs_litr->litr1c, ns_litr->litr1n);
        cs_litr->litr1c = 0.0; //Nbalance check 20201104
        ns_litr->litr1n = 0.0;
	  cdf->litr1c_hr = 0.0;
	  cdf->litr1c_to_soil1c = 0.0;
	  ndf->litr1n_to_soil1n = 0.0;
	  ndf->sminn_to_soil1n_l1 = 0.0;
	}

	/* cellulose litter fluxes */
	if ((cs_litr->litr2c > ZERO) && (ns_litr->litr2n > ZERO)){
		if (nlimit && ndf->pmnf_l2s2 > 0.0){
			cdf->plitr2c_loss *= fpi;
			ndf->pmnf_l2s2 *= fpi;
		}
		cdf->litr2c_hr = rfl2s2 * cdf->plitr2c_loss;
		cdf->litr2c_to_soil2c = (1.0 - rfl2s2) * cdf->plitr2c_loss;
		if (ns_litr->litr2n > ZERO)
			ndf->litr2n_to_soil2n = cdf->plitr2c_loss / cn_l2;
		else ndf->litr2n_to_soil2n = 0.0;
		ndf->sminn_to_soil2n_l2 = ndf->pmnf_l2s2;
		daily_net_nmin -= ndf->pmnf_l2s2;
	} else{

	  if ((cs_litr->litr2c < 0.0) || (ns_litr->litr2n < 0.0)) printf("lc2=%e, ln2=%e\n",cs_litr->litr2c, ns_litr->litr2n);
        cs_litr->litr2c = 0.0;
        ns_litr->litr2n = 0.0;
	  cdf->litr2c_hr = 0.0;
	  cdf->litr2c_to_soil2c = 0.0;
	  ndf->litr2n_to_soil2n = 0.0;
	  ndf->sminn_to_soil2n_l2 = 0.0;
	}

	/* release of shielded cellulose litter, tied to the decay rate of
	lignin litter */
	/* actually going to litr 2 rather than soil but will use soil2 as repository for mineralized N */
	if ((cs_litr->litr3c > ZERO) && (ns_litr->litr3n > ZERO)){
		if (nlimit && ndf->pmnf_l3l2 > 0.0){
			cdf->plitr3c_loss *= fpi;
			ndf->pmnf_l3l2 *= fpi;
		}
		cdf->litr3c_hr = rfl4s3 * cdf->plitr3c_loss;
		cdf->litr3c_to_litr2c = (1.0 - rfl4s3) * cdf->plitr3c_loss;
		if (ns_litr->litr3n > 0.000000001)
			ndf->litr3n_to_litr2n = cdf->plitr3c_loss / cn_l3;
		else ndf->litr3n_to_litr2n = 0.0;
		ndf->sminn_to_soil2n_l3 = ndf->pmnf_l3l2;
		daily_net_nmin -= ndf->pmnf_l3l2;
	} else{

	  if ((cs_litr->litr3c < 0.0) || (ns_litr->litr3n < 0.0)) printf("lc3=%e, ln3=%e\n",cs_litr->litr3c, ns_litr->litr3n);
        cs_litr->litr3c = 0.0;
        ns_litr->litr3n = 0.0;
	  cdf->litr3c_hr = 0.0;
	  cdf->litr3c_to_litr2c = 0.0;
	  ndf->litr3n_to_litr2n = 0.0;
	  ndf->sminn_to_soil2n_l3 = 0.0;
	}

	/* lignin litter fluxes */
	if ((cs_litr->litr4c > ZERO) && (ns_litr->litr4n > ZERO)){
		if (nlimit && ndf->pmnf_l4s3 > 0.0){
			cdf->plitr4c_loss *= fpi;
			ndf->pmnf_l4s3 *= fpi;
		}
		cdf->litr4c_hr = rfl4s3 * cdf->plitr4c_loss;
		cdf->litr4c_to_soil3c = (1.0 - rfl4s3) * cdf->plitr4c_loss;
		if (ns_litr->litr4n > 0.000000001)
			ndf->litr4n_to_soil3n = cdf->plitr4c_loss / cn_l4;
		else ndf->litr4n_to_soil3n = 0.0;
		ndf->sminn_to_soil3n_l4 = ndf->pmnf_l4s3;
		daily_net_nmin -= ndf->pmnf_l4s3;
	} else{

	  if ((cs_litr->litr4c < 0.0) || (ns_litr->litr4n < 0.0)) printf("lc4=%e, ln4=%e\n",cs_litr->litr4c, ns_litr->litr4n);
        cs_litr->litr4c = 0.0;
        ns_litr->litr4n = 0.0;
	  cdf->litr4c_hr = 0.0;
	  cdf->litr4c_to_soil3c = 0.0;
	  ndf->litr4n_to_soil3n = 0.0;
	  ndf->sminn_to_soil3n_l4 = 0.0;
	}

	/* fast microbial recycling pool */
	if (cs_soil->soil1c > ZERO && ns_soil->soil1n > ZERO){
		if (nlimit && ndf->pmnf_s1s2 > 0.0){
			cdf->psoil1c_loss *= fpi;
			ndf->pmnf_s1s2 *= fpi;
		}
		cdf->soil1c_hr = rfs1s2 * cdf->psoil1c_loss;
		cdf->soil1c_to_soil2c = (1.0 - rfs1s2) * cdf->psoil1c_loss;
		ndf->soil1n_to_soil2n = cdf->psoil1c_loss / cn_s1;
		ndf->sminn_to_soil2n_s1 = ndf->pmnf_s1s2;
		daily_net_nmin -= ndf->pmnf_s1s2;
	} else{
	  cs_soil->soil1c = 0.0;
	  ns_soil->soil1n = 0.0;
	  cdf->soil1c_hr = 0.0;
	  cdf->soil1c_to_soil2c = 0.0;
	  ndf->soil1n_to_soil2n = 0.0;
	  ndf->sminn_to_soil2n_s1 = 0.0;
	}

	/* medium microbial recycling pool */
	if (cs_soil->soil2c > ZERO && ns_soil->soil2n > ZERO){
		if (nlimit && ndf->pmnf_s2s3 > 0.0){
			cdf->psoil2c_loss *= fpi;
			ndf->pmnf_s2s3 *= fpi;
		}
		cdf->soil2c_hr = rfs2s3 * cdf->psoil2c_loss;
		cdf->soil2c_to_soil3c = (1.0 - rfs2s3) * cdf->psoil2c_loss;
		ndf->soil2n_to_soil3n = cdf->psoil2c_loss / cn_s2;
		ndf->sminn_to_soil3n_s2 = ndf->pmnf_s2s3;
		daily_net_nmin -= ndf->pmnf_s2s3;
	} else{
	   cs_soil->soil2c = 0.0;
	  ns_soil->soil2n = 0.0;
	  cdf->soil2c_hr = 0.0;
	  cdf->soil2c_to_soil3c = 0.0;
	  ndf->soil2n_to_soil3n = 0.0;
	  ndf->sminn_to_soil3n_s2 = 0.0;
	}

	/* slow microbial recycling pool */
	if (cs_soil->soil3c > ZERO && ns_soil->soil3n > ZERO){
		if (nlimit && ndf->pmnf_s3s4 > 0.0){
			cdf->psoil3c_loss *= fpi;
			ndf->pmnf_s3s4 *= fpi;
		}
		cdf->soil3c_hr = rfs3s4 * cdf->psoil3c_loss;
		cdf->soil3c_to_soil4c = (1.0 - rfs3s4) * cdf->psoil3c_loss;
		ndf->soil3n_to_soil4n = cdf->psoil3c_loss / cn_s3;
		ndf->sminn_to_soil4n_s3 = ndf->pmnf_s3s4;
		daily_net_nmin -= ndf->pmnf_s3s4;
	} else{
	  cs_soil->soil3c = 0.0;
	  ns_soil->soil3n = 0.0;
	  cdf->soil3c_hr = 0.0;
	  cdf->soil3c_to_soil4c = 0.0;
	  ndf->soil3n_to_soil4n = 0.0;//<<-------
	  ndf->sminn_to_soil4n_s3 = 0.0;
	}

	/* recalcitrant SOM pool (rf = 1.0, always mineralizing) */
	if (cs_soil->soil4c > ZERO && ns_soil->soil4n > ZERO){
		cdf->soil4c_hr = cdf->psoil4c_loss;
		ndf->soil4n_to_sminn = cdf->psoil4c_loss / cn_s4;
		daily_net_nmin += ndf->soil4n_to_sminn;
	} else{
        cs_soil->soil4c = 0.0;
        ns_soil->soil4n = 0.0;
	  cdf->soil4c_hr = 0.0;
	  ndf->soil4n_to_sminn = 0.0;
	}

	/* update soild and litter stores */
	/* Fluxes out of labile litter pool */
	cs_litr->litr1c_hr_snk += cdf->litr1c_hr;
	cs_litr->litr1c       -= cdf->litr1c_hr;
	if (cs_litr->litr1c - cdf->litr1c_to_soil1c < 0.0) {
		cdf->litr1c_to_soil1c = max(cs_litr->litr1c,0.0);
		ndf->litr1n_to_soil1n = cdf->litr1c_to_soil1c / cn_l1 ;
	}
	if (ns_litr->litr1n - ndf->litr1n_to_soil1n < 0.0) {
		ndf->litr1n_to_soil1n = max(ns_litr->litr1n,0.0);
		cdf->litr1c_to_soil1c = cdf->litr1c_to_soil1c * cn_l1 ;
	}
	cs_soil->soil1c       += cdf->litr1c_to_soil1c;
	cs_litr->litr1c       -= cdf->litr1c_to_soil1c;
	/* Fluxes out of cellulose litter pool */
	cs_litr->litr2c_hr_snk += cdf->litr2c_hr;
	cs_litr->litr2c       -= cdf->litr2c_hr;
	if (cs_litr->litr2c - cdf->litr2c_to_soil2c < 0.0) {
		cdf->litr2c_to_soil2c = max(cs_litr->litr2c,0.0);
		ndf->litr2n_to_soil2n = max(ns_litr->litr2n,0.0);
	}
	cs_soil->soil2c       += cdf->litr2c_to_soil2c;
	cs_litr->litr2c       -= cdf->litr2c_to_soil2c;
	/* Fluxes from shielded to unshielded cellulose pools */
	if (cs_litr->litr3c - cdf->litr3c_to_litr2c < 0.0) {
		cdf->litr3c_to_litr2c = max(cs_litr->litr3c,0.0);
		ndf->litr3n_to_litr2n = max(ns_litr->litr3n,0.0);
	}
	cs_litr->litr2c       += cdf->litr3c_to_litr2c;
	cs_litr->litr3c       -= cdf->litr3c_to_litr2c;
	/* Fluxes out of lignin litter pool */
	cs_litr->litr4c_hr_snk += cdf->litr4c_hr;
	cs_litr->litr4c       -= cdf->litr4c_hr;
	if (cs_litr->litr4c - cdf->litr4c_to_soil3c < 0.0) {
		cdf->litr4c_to_soil3c = max(cs_litr->litr4c,0.0);
		ndf->litr4n_to_soil3n = max(ns_litr->litr4n,0.0);
	}
	cs_soil->soil3c       += cdf->litr4c_to_soil3c;
	cs_litr->litr4c       -= cdf->litr4c_to_soil3c;
	/* Fluxes out of fast soil pool */
	cs_soil->soil1c_hr_snk += cdf->soil1c_hr;
	cs_soil->soil1c       -= cdf->soil1c_hr;
	if (cs_soil->soil1c - cdf->soil1c_to_soil2c < 0.0) {
		cdf->soil1c_to_soil2c = max(cs_soil->soil1c, 0.0);
		ndf->soil1n_to_soil2n = max(ns_soil->soil1n, 0.0);
	}
	cs_soil->soil2c       += cdf->soil1c_to_soil2c;
	cs_soil->soil1c       -= cdf->soil1c_to_soil2c;
	/* Fluxes out of medium soil pool */
	cs_soil->soil2c_hr_snk += cdf->soil2c_hr;
	cs_soil->soil2c       -= cdf->soil2c_hr;
	if (cs_soil->soil2c - cdf->soil2c_to_soil3c < 0.0) {
		cdf->soil2c_to_soil3c = max(cs_soil->soil2c, 0.0);
		ndf->soil2n_to_soil3n = max(ns_soil->soil2n, 0.0);
	}
	cs_soil->soil3c       += cdf->soil2c_to_soil3c;
	cs_soil->soil2c       -= cdf->soil2c_to_soil3c;
	/* Fluxes out of slow soil pool */
	cs_soil->soil3c_hr_snk += cdf->soil3c_hr;
	cs_soil->soil3c       -= cdf->soil3c_hr;
	if (cs_soil->soil3c - cdf->soil3c_to_soil4c < 0.0) {
		cdf->soil3c_to_soil4c = max(cs_soil->soil3c, 0.0);
		ndf->soil3n_to_soil4n = max(ns_soil->soil3n, 0.0);
	}
	cs_soil->soil4c       += cdf->soil3c_to_soil4c;
	cs_soil->soil3c       -= cdf->soil3c_to_soil4c;
	/* Fluxes out of recalcitrant SOM pool */
	cs_soil->soil4c_hr_snk += cdf->soil4c_hr;
	cs_soil->soil4c       -= cdf->soil4c_hr;
	/* Fluxes out of labile litter pool */
	ns_soil->soil1n       += ndf->litr1n_to_soil1n;
	ns_litr->litr1n       -= ndf->litr1n_to_soil1n;
	ns_soil->soil1n	      += ndf->sminn_to_soil1n_l1;
	/* Fluxes out of cellulose litter pool */
	ns_soil->soil2n       += ndf->litr2n_to_soil2n;
	ns_litr->litr2n       -= ndf->litr2n_to_soil2n;
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_l2;
	/* Fluxes from shielded to unshielded cellulose pools */
	ns_litr->litr2n       += ndf->litr3n_to_litr2n;
	ns_litr->litr3n       -= ndf->litr3n_to_litr2n;
	/* this one is odd because we don't know where to get the N for shifting between litter 2 and 3 */
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_l3;
	/* Fluxes out of lignin litter pool */
	ns_soil->soil3n       += ndf->litr4n_to_soil3n;
	ns_litr->litr4n       -= ndf->litr4n_to_soil3n;
	ns_soil->soil3n	      += ndf->sminn_to_soil3n_l4;
	/* Fluxes out of fast soil pool */
	ns_soil->soil2n       += ndf->soil1n_to_soil2n;
	ns_soil->soil1n       -= ndf->soil1n_to_soil2n;
	ns_soil->soil2n	      += ndf->sminn_to_soil2n_s1;
	/* Fluxes out of medium soil pool */
	ns_soil->soil3n       += ndf->soil2n_to_soil3n;
	ns_soil->soil2n       -= ndf->soil2n_to_soil3n;
	ns_soil->soil3n	      += ndf->sminn_to_soil3n_s2;
	/* Fluxes out of slow soil pool */
	ns_soil->soil4n       += ndf->soil3n_to_soil4n;
	ns_soil->soil3n       -= ndf->soil3n_to_soil4n;
	ns_soil->soil4n	      += ndf->sminn_to_soil4n_s3;
	ns_soil->soil4n	      -= ndf->soil4n_to_sminn;
	/* Fluxes into mineralized N pool */
	/* Fluxes output of mineralized N pool for net microbial immobilization */
	if (patch[0].rtzSatNO3 > ZERO && patch[0].rtzSatNH4 > ZERO) {
	totalNO3 = ns_soil->nitrate + patch[0].rtzSatNO3;
	totalNH4 = ns_soil->sminn + patch[0].rtzSatNH4;
	ratio_NO3 = ns_soil->nitrate / totalNO3;
	ratio_NH4 = ns_soil->sminn / totalNH4;}
	else {
    totalNO3 = ns_soil->nitrate;
    totalNH4 = ns_soil->sminn;
    ratio_NO3 = 1;
    ratio_NH4 = 1;
	}

	if (daily_net_nmin > ZERO){
		ns_soil->sminn += daily_net_nmin * ratio_NH4;
		patch[0].sat_NH4 += daily_net_nmin * (1 - ratio_NH4);}
	else {
		//if (-1.0*daily_net_nmin > ns_soil->sminn + ns_soil->nitrate + ZERO) {
        if (-1.0*daily_net_nmin > (totalNO3 + totalNH4 + ZERO)) {
			/* this should not happen  but if it does warn user and but let sminn go negative*/
			printf("In update decomp not enough for mineral N will reduce accordingly ");
			balance = totalNO3 + totalNH4 + daily_net_nmin;
			printf("\n required %lf balance unmet %lf", -1.0*daily_net_nmin, balance);
			daily_net_nmin = -1.0 * (totalNH4 + totalNO3);

		}
		nitrate_immob = min(totalNO3, -1.0*daily_net_nmin); // daily_net_nmin < total N may larger than totalNO3
		ns_soil->nitrate = max(ns_soil->nitrate - nitrate_immob * ratio_NO3,0.0); // remove immobolized nitrate from soil
		patch[0].sat_NO3 = max(patch[0].sat_NO3 - nitrate_immob * (1 - ratio_NO3), 0.0); //remove immobolized nitrate from sat_NO3
		patch[0].rtzSatNO3 = max(patch[0].rtzSatNO3 - nitrate_immob *(1 - ratio_NO3), 0.0); // this pool just for tracking the sat_NO3 is the main pool

		double leftover = max(-1.0*daily_net_nmin - nitrate_immob, 0.0); // so this is daily_net_min - total NO3
		ns_soil->sminn = max(ns_soil->sminn - leftover* ratio_NH4, 0.0);
		patch[0].sat_NH4 = max(patch[0].sat_NH4 - leftover * (1 - ratio_NH4), 0.0);
		patch[0].rtzSatNH4 = max(patch[0].rtzSatNH4 - leftover * (1 - ratio_NH4), 0.0); // this is just for tracking too
	}
	/* Fluxes output of mineralized N pool from plant uptake */
	/*	we use N from the following first soil nitrate, */
	/*	second soil NH4; and thirds surface NO3, 4th surface NH4 */
	remaining_uptake = ndf->sminn_to_npool;
	/* remove from soil nitrate pool */
	N_uptake = max((min(ns_soil->nitrate, remaining_uptake)),0.0);
	ns_soil->nitrate -= N_uptake;
	remaining_uptake -= N_uptake;
	/* remove from sat_NO3 pool for ratio use rtzSatNO3 for remove use sat_NO3*/
	if (patch[0].sat_NO3 > ZERO && patch[0].rtzSatNO3 > ZERO) {

    temp = min(patch[0].sat_NO3, patch[0].rtzSatNO3);
	N_uptake = max((min(temp, remaining_uptake)), 0.0);
	patch[0].sat_NO3 -= N_uptake;
	patch[0].rtzSatNO3 -= N_uptake; // for tracking
	remaining_uptake -= N_uptake;

	}

	/* remove from soil NH4 pool */
	N_uptake = max((min(ns_soil->sminn, remaining_uptake)),0.0);
	ns_soil->sminn -=  N_uptake;
	remaining_uptake -= N_uptake;

	/* remove from sat_Nh4 pool for ratio use rtzSatNh4for remove use sat_NH4*/
	if (patch[0].sat_NH4 > ZERO && patch[0].rtzSatNH4 > ZERO) {

    temp = min(patch[0].sat_NH4, patch[0].rtzSatNH4);
	N_uptake = max((min(temp, remaining_uptake)), 0.0);
	patch[0].sat_NH4 -= N_uptake;
	patch[0].rtzSatNH4 -= N_uptake; // for tracking
	remaining_uptake -= N_uptake;

	}

	/* remove from surface NO3 pool */
	N_uptake = max((min(patch[0].surface_NO3, remaining_uptake)),0.0);
	patch[0].surface_NO3 -=  N_uptake;
	remaining_uptake -= N_uptake;

	/* remove from surface NH4 pool */
	N_uptake = max((min(patch[0].surface_NH4, remaining_uptake)),0.0);
	patch[0].surface_NH4 -=  N_uptake;
	remaining_uptake -= N_uptake;

	if (remaining_uptake > ZERO) printf("N balance issue [remaining upate %lf], [patch ID %d], [totalNO3 %lf], [totalNH4 %lf], [ratio_NO3 %lf], [ratio_NH4 %lf] \n",
                                        remaining_uptake, patch[0].ID, totalNO3, totalNH4, ratio_NO3, ratio_NH4);
	ndf->net_mineralized = daily_net_nmin;
	total_N = ns_litr->litr1n + ns_litr->litr2n +  ns_litr->litr3n
		+ ns_litr->litr4n + ns_soil->soil1n + ns_soil->soil2n
		+ ns_soil->soil3n + ns_soil->soil4n + ns_soil->sminn + ns_soil->nitrate + patch[0].rtzSatNH4 + patch[0].rtzSatNO3;

	balance = (total_preday_N)  - (total_N + ndf->sminn_to_npool);
	if (abs(balance) > ZERO)
		printf("\n Decomp N doesn't balance by %lf ", balance);

	/* calculate the fluxes out NREN 20190927 */
    cdf->litterc_to_atmos += cdf->litr1c_hr;
    cdf->litterc_to_atmos += cdf->litr2c_hr;
    cdf->litterc_to_atmos += cdf->litr3c_hr;
    cdf->litterc_to_atmos += cdf->litr4c_hr;

    cdf->litterc_to_soilc += cdf->litr1c_to_soil1c;
    cdf->litterc_to_soilc += cdf->litr2c_to_soil2c;
    cdf->litterc_to_soilc += cdf->litr4c_to_soil3c;



	return (!ok);
} /* end update_decomp.c */
