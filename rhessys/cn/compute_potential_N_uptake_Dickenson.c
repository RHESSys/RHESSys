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
/*	uses Dickenson et al. 1998, J of Climate		*/
/*	where fraction of allocation to leaves 			*/
/*	is a function of LAI					*/
/*								*/
/*	SYNOPSIS						*/
/*	int compute_potential_N_uptake_Dickenson( int		*/
/*                          struct epconst_struct,              */
/*                          struct epv_var_struct *,              */
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
/*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"
double compute_potential_N_uptake_Dickenson( 
								  struct	epconst_struct epc,
								  struct epvar_struct	*epv,
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
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double fwood, fleaf, froot, flive, fdead; 	/* fraction allocate to each component */
	double total_wood, B,C;
	double mean_cn;
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
	cnl = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;
	/*---------------------------------------------------------------*/
	/* constant B and C are currently set for forests from Dickenson et al. */	
	/*----------------------------------------------------------------*/


	fleaf = exp(-1.0*epc.dickenson_pa * epv->proj_lai);
	fleaf = min(fleaf, 1.0);
	total_wood = (cs->live_crootc + cs->dead_crootc + cs->live_stemc + cs->dead_stemc);

	
	if (epc.veg_type==TREE) {
		if (2*fleaf < 0.8) {
			froot = fleaf;
			fwood= 1.0-fleaf-froot;
			}
		else {
			fleaf = min(fleaf, 0.6);
			froot = 0.5*(1-fleaf);
			fwood = 0.5*(1-fleaf);
			}
		}
	else {
		fwood = 0;
		froot = (1-fleaf);
		}
	
	flive = epc.alloc_livewoodc_woodc;
	fdead = 1-flive;

	if ((fleaf+froot) > ZERO) {
	if (epc.veg_type == TREE){
		mean_cn = 1.0 / (fleaf / cnl + froot / cnfr + flive * fwood / cnlw + fwood * fdead / cndw);
	}
	else{
	   mean_cn = 1.0 / (fleaf / cnl + froot / cnfr);
	}
	}
	else mean_cn = 1.0;



	cdf->fleaf = fleaf;
	cdf->froot = froot;
	cdf->fwood = fwood;	

	/* add in nitrogen for plants and for nitrogen deficit in pool */
	plant_ndemand = cs->availc / (1.0+epc.gr_perc) / mean_cn; 
	
	return(plant_ndemand);
} /* 	end compute_potential_N_uptake */
