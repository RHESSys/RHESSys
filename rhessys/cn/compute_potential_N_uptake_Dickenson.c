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
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double fstem, fcroot,fwood, fleaf, froot, fdead, fbroot; 	/* fraction allocate to each component */
	double f4;
	double mean_cn, ratio;
	double transfer,plant_ndemand;
	/*---------------------------------------------------------------
	Assess the carbon availability on the basis of this day's
	gross production and maintenance respiration costs
	----------------------------------------------------------------*/
	cs->availc = cdf->psn_to_cpool-cdf->total_mr;
	/* no allocation when the daily C balance is negative */
 	if (cs->availc < 0.0) {
                cs->mr_deficit += -cs->availc;
                        cs->availc = 0.0; }
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


	/*---------------------------------------------------------------
	Also try to reduce any seasonal maintenance respiration deficit
		-----------------------------------------------*/
	 if (cs->mr_deficit > ZERO) {
                 transfer = min(cs->availc, cs->mr_deficit);
                  cs->availc -= transfer;
                   cs->cpool += transfer;
                cs->mr_deficit -= transfer;
                }

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

	if (epc.veg_type == TREE) {


		fleaf = exp(-1.0*epc.dickenson_pa * epv->proj_lai);
		fleaf = min(fleaf, 1.0);
		fbroot = fleaf * epc.alloc_frootc_leafc ;
		ratio = fbroot/fleaf;

		if (fbroot+fleaf > 0.95) {
			fleaf = 0.95/(1+ratio);
			fbroot = fleaf*ratio;
			}
		
		fcroot = fbroot/(1+epc.alloc_frootc_crootc);
		froot = fbroot-fcroot;

		fstem = 1.0-(froot+fcroot+fleaf);
		fwood = fstem+fcroot;
		}

	else {
		fleaf = exp(-1.0*epc.dickenson_pa * epv->proj_lai);
		fleaf = min(fleaf, 1.0);
		froot = fleaf * epc.alloc_frootc_leafc ;
		fcroot=0.0;
		fwood=0.0;
	}


	f4 = epc.alloc_livewoodc_woodc;
	if (epc.veg_type == TREE){
		if ((fleaf + froot + fwood) > ZERO) 
			mean_cn = 1.0 / (fleaf / cnl + froot / cnfr + f4 * fwood / cnlw + fwood * (1.0-f4) / cndw);
		else mean_cn = 1.0;
	}
        else{
		if ((fleaf + froot) > ZERO) 	
           		mean_cn = 1.0 / (fleaf / cnl + froot / cnfr);
	else
		mean_cn=1.0;
        }

	if (mean_cn > ZERO)
		plant_ndemand = cs->availc / (1.0+epc.gr_perc) / mean_cn;
	else
		plant_ndemand = 0.0;


	cdf->fleaf = fleaf;
	cdf->fwood = fwood;
	cdf->froot = froot;
	cdf->fcroot = fcroot;

	
	
	return(plant_ndemand);
} /* 	end compute_potential_N_uptake */
