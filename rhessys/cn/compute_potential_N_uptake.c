/*--------------------------------------------------------------*/
/* 								*/
/*		compute_potential_N_uptake					*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_potential_N_uptake -				*/
/*		computes potential N uptake from soil		*/
/*		for this strata without mineralization		*/
/*		limitation					*/
/*								*/
/*	SYNOPSIS						*/
/*	int compute_potential_N_uptake(					*/
/*                          struct epconst_struct,              */
/*			    struct epvar_struct *epv,		*/
/*                          struct cstate_struct *,             */
/*                          struct nstate_struct *,             */
/*                          struct cdayflux_struct *)           */
/*								*/
/*								*/
/*	returns int:						*/
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
/*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"
double compute_potential_N_uptake(
								  struct	epconst_struct epc,
								  struct	epvar_struct *epv,
								  struct cstate_struct *cs,
								  struct nstate_struct *ns,
								  struct cdayflux_struct *cdf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double day_gpp;     /* daily gross production */
	double day_mresp;   /* daily total maintenance respiration */
	double f1;          /* RATIO   new fine root C : new leaf C     */
	double f2;          /* RATIO   new coarse root C : new stem C   */
	double f3;          /* RATIO   new stem C : new leaf C          */
	double f4;          /* RATIO   new live wood C : new wood C     */
	double g1;          /* RATIO   C respired for growth : C grown  */
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double cnmax;       /* RATIO   max of root and leaf C:N      */
	double c_allometry, n_allometry;
	double plant_ndemand;
	/*---------------------------------------------------------------
	Assess the carbon availability on the basis of this day's
	gross production and maintenance respiration costs
	----------------------------------------------------------------*/
	cs->availc = cdf->psn_to_cpool-cdf->total_mr;
	/* no allocation when the daily C balance is negative */
	if (cs->availc < 0.0) cs->availc = 0.0;
	/* test for cpool deficit */
	if (cs->cpool < 0.0){
	/*--------------------------------------------------------------
	running a deficit in cpool, so the first priority
	is to let today's available C accumulate in cpool.  The actual
	accumulation in the cpool is resolved in day_carbon_state().
		--------------------------------------------------------------*/
		if (-cs->cpool < cs->availc){
		/*------------------------------------------------
		cpool deficit is less than the available
		carbon for the day, so aleviate cpool deficit
		and use the rest of the available carbon for
		new growth and storage. Remember that fluxes in
		and out of the cpool are reconciled at the end
		of the daily loop, so for now, just keep track
		of the amount of daily GPP-MR that is not needed
		to restore a negative cpool.
			-----------------------------------------------*/
			cs->availc += cs->cpool;
		}
		else{
		/*---------------------------------------------------------
		cpool deficit is >= available C, so all of the
		daily GPP, if any, is used to alleviate negative cpool
			------------------------------------------------------------*/
			cs->availc = 0.0;
		}
	} /* end if negative cpool */

	/* assign local values for the allocation control parameters */
	f1 = epc.alloc_frootc_leafc;
	f2 = epc.alloc_crootc_stemc;
	f3 = epc.alloc_stemc_leafc;
	f4 = epc.alloc_livewoodc_woodc;
	g1 = epc.gr_perc;
	cnl = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;
	/*---------------------------------------------------------------
	given the available C, use constant allometric relationships to
	determine how much N is required to meet this potential growth
	demand */
	/* ----------------------------------------------------------------*/
	if (epc.veg_type == TREE){
		c_allometry = ((1.0+g1)*(1.0 + f1 + f3*(1.0+f2)));
		n_allometry = (1.0/cnl + f1/cnfr + (f3*f4*(1.0+f2))/cnlw
			+ (f3*(1.0-f4)*(1.0+f2))/cndw);
	}
	else{
		c_allometry = (1.0 + g1 + f1 + f1*g1);
		n_allometry = (1.0/cnl + f1/cnfr);
	}
	plant_ndemand = cs->availc * (n_allometry / c_allometry);

	cdf->fleaf =  1.0/(1.0+f1+f3+f2*f3);
        cdf->froot = cdf->fleaf*f1;
        cdf->fwood = cdf->fleaf*f3*(1.0+f2);

	return(plant_ndemand);
} /* 	end compute_potential_N_uptake */
