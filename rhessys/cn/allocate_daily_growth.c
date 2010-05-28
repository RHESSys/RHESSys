/*--------------------------------------------------------------*/
/* 								*/
/*								*/
/*	allocate_daily_growth				*/
/*								*/
/*	NAME							*/
/*		allocate_daily_growth			*/
/*								*/
/*	SYNOPSIS						*/
/*	int allocate_daily_growth( int , double, double,double,	*/
/*			    struct cdayflux_struct *,  		*/
/*			    struct cstate_struct *,		*/
/*			    struct ndayflux_struct *,	 	*/
/*			    struct nstate_struct *, 		*/
/*			    struct ndayflux_patch_struct *, 	*/
/*				struct epvar_struct *,		*/
/*			    struct epconst_struct)		*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*		calculates daily C and N allocations		*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*              modified from Peter Thornton (1998)             */
/*                      dynamic - 1d-bgc ver4.0                 */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int allocate_daily_growth(int nlimit,
						  double pnow,
						  double total_soil_frootc,
						  double cover_fraction,
						  struct cdayflux_struct *cdf,
						  struct cstate_struct *cs,
						  struct ndayflux_struct *ndf,
						  struct nstate_struct *ns,
						  struct ndayflux_patch_struct *ndf_patch,
						  struct epvar_struct *epv,
						  struct epconst_struct epc,
						  struct date current_date)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double f1;          /* RATIO   new fine root C : new leaf C     */
	double f2;          /* RATIO   new coarse root C : new stem C   */
	double f3;          /* RATIO   new stem C : new leaf C          */
	double f4;          /* RATIO   new live wood C : new wood C     */
	double g1;          /* RATIO   C respired for growth : C grown  */
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double nlc;         /* actual new leaf C, minimum of C and N limits   */
	double resp, gresp_store;
	double plant_ndemand;
	double sum_plant_nsupply, soil_nsupply;
	double plant_nalloc, plant_calloc;
	double closs, nloss, add_to_cpool;
	double plant_remaining_ndemand;
	double excess_allocation_to_leaf, excess_c, excess_n, excess_lai;
	double sminn_to_npool;
	double n_newleafc, c_newleafc;
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
	excess_c = 0.0;
	resp = 0.0;
	add_to_cpool = 0.0;
	plant_ndemand = ndf->potential_N_uptake;

	/*--------------------------------------------------------------*/
	/*	the amount available from the soil is potential_N_uptake adjusted */
	/*	by fract_potential_uptake which is calculated in resolve_N_competition */
	/*	based on available soil mineralized nitrogen				*/
	/*	if there are no roots then we uptake anyway to get things started */
	/*--------------------------------------------------------------*/
	if (nlimit == 1)
		if (total_soil_frootc > ZERO)
			soil_nsupply = min(ndf->potential_N_uptake,
			(ndf_patch->plant_avail_uptake *
			cs->frootc / total_soil_frootc));
		else
			soil_nsupply = ndf->potential_N_uptake;
	else
		soil_nsupply = ndf->potential_N_uptake;
	/*----------------------------------------------------------------
		now compare the combined decomposition immobilization and plant
		growth N demands against the available soil mineral N pool.
	--------------------------------------------------------------------*/
	if (nlimit == 0){
	/* N availability is not limiting so plant
		uptake, and both can proceed at  potential rates */
		/* Determine the split between retranslocation N and soil mineral
		N to meet the plant demand */
		sum_plant_nsupply = ns->retransn + soil_nsupply;
		if (sum_plant_nsupply > 0.0){
			ndf->retransn_to_npool = ndf->potential_N_uptake
				* (ns->retransn/sum_plant_nsupply);
		}
		else{
			ndf->retransn_to_npool = 0.0;
		}
		sminn_to_npool = ndf->potential_N_uptake - ndf->retransn_to_npool;
		plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
		ns->nlimit = 0;
	}
	else{
	/* N availability can not satisfy the sum of immobiliation and
	plant growth demands, so these two demands compete for available
		soil mineral N */
		sminn_to_npool = soil_nsupply;
		plant_remaining_ndemand = plant_ndemand - sminn_to_npool;
		/* the demand not satisfied by uptake from soil mineral N is
		now sought from the retranslocated N pool */
		if (plant_remaining_ndemand <= ns->retransn){
		/* there is enough N available in retranslocation pool to
			satisfy the remaining plant N demand */
			ndf->retransn_to_npool = plant_remaining_ndemand;
			plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
			ns->nlimit = 0;
		}
		else{
		/* there is not enough retranslocation N left to satisfy the
		entire demand. In this case, all remaing retranslocation N is
		used, and the remaining unsatisfied N demand is translated
		back to a C excess, which is deducted from
			photosynthesis source */
			ndf->retransn_to_npool = ns->retransn;
			plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
			ns->nlimit = 1;
		}
	}


	soil_nsupply = plant_nalloc;
	/*--------------------------------------------------------------*/
	/* calculate the amount of new leaf C dictated by these allocation
	decisions, and figure the daily fluxes of C and N to current
	growth and storage pools 
	given the available C, use constant allometric relationships to
	determine how much N is required to meet this potential growth
	demand */
	/*------------------------------------------------------------------ */
	if (epc.veg_type == TREE)
	{
		c_newleafc = cs->availc / (1.0+g1+f1+f1*g1+f3+f3*g1+f2*f3+f2*f3*g1);
		n_newleafc = plant_nalloc   / (1.0/cnl + f1/cnfr + (f3 * f4 * (1.0+f2))/cnlw +
						(f3 * (1.0-f4) * (1.0+f2))/cndw);
	 }
	 else
	 {
		c_newleafc = cs->availc / (1.0+g1+f1+f1*g1);
	 	n_newleafc = plant_nalloc / (1.0/cnl + f1/cnfr);
	 }

	if (n_newleafc < c_newleafc) nlc = n_newleafc;
		else nlc = c_newleafc;

	if (epc.veg_type == TREE) {
		plant_calloc = nlc * (1.0+g1+f1+f1*g1+f3+f3*g1+f2*f3+f2*f3*g1);
		plant_nalloc = nlc * (1.0/cnl + f1/cnfr + (f3 * f4 * (1.0+f2))/cnlw + (f3 * (1.0-f4) * (1.0+f2))/cndw);
		}
	 else {
		plant_calloc = nlc * (1.0+g1+f1+f1*g1);
		plant_nalloc = nlc * (1.0/cnl + f1/cnfr);
		}

	if (plant_calloc > 0.0) {
		cdf->fleaf = 1.0/(1.0+f1+f3+f2*f3);
		cdf->froot = cdf->fleaf*f1;
		cdf->fwood = cdf->fleaf*f3*(1.0+f2);
		}
	else
	{
		cdf->fleaf = 0.0;
		cdf->fwood = 0.0;
		cdf->froot = 0.0;
	}

	

	ndf->actual_N_uptake = plant_nalloc;

	/*--------------------------------------------------------------*/
	/* Nitrogen availabiliy limited photosynethesis so reduce	*/
	/*--------------------------------------------------------------*/
	excess_c = cs->availc - plant_calloc;
	if (excess_c > ZERO)
		cdf->psn_to_cpool -= excess_c;


	cdf->cpool_to_leafc              = nlc * pnow;
	cdf->cpool_to_leafc_store      = nlc * (1.0-pnow);
	cdf->cpool_to_frootc             = f1 * nlc * pnow;
	cdf->cpool_to_frootc_store     = f1 * nlc * (1.0-pnow);
	if (epc.veg_type == TREE){
		cdf->cpool_to_livestemc        = nlc * f3 * f4 * pnow;
		cdf->cpool_to_livestemc_store  = nlc * f3 * f4 * (1.0-pnow);
		cdf->cpool_to_deadstemc          = nlc * f3 * (1.0-f4) * pnow;
		cdf->cpool_to_deadstemc_store  = nlc * f3 * (1.0-f4) * (1.0-pnow);
		cdf->cpool_to_livecrootc         = nlc * f2 * f3 * f4 * pnow;
		cdf->cpool_to_livecrootc_store = nlc * f2 * f3 * f4 * (1.0-pnow);
		cdf->cpool_to_deadcrootc         = nlc * f2 * f3 * (1.0-f4) * pnow;
		cdf->cpool_to_deadcrootc_store = nlc * f2 * f3 * (1.0-f4) * (1.0-pnow);
	}
	/* daily N fluxes out of npool and into new growth or storage */
	ndf->sminn_to_npool = sminn_to_npool;
	ndf_patch->sminn_to_npool += sminn_to_npool * cover_fraction;

	ndf->npool_to_leafn              = cdf->cpool_to_leafc / cnl;
	ndf->npool_to_leafn_store      = cdf->cpool_to_leafc_store / cnl;
	ndf->npool_to_frootn              = cdf->cpool_to_frootc / cnfr;
	ndf->npool_to_frootn_store      = cdf->cpool_to_frootc_store / cnfr;
	if (epc.veg_type == TREE){
		ndf->npool_to_livestemn        = cdf->cpool_to_livestemc / cnlw; 
		ndf->npool_to_livestemn_store  = cdf->cpool_to_livestemc_store / cnlw; 
		ndf->npool_to_deadstemn        = cdf->cpool_to_deadstemc / cndw; 
		ndf->npool_to_deadstemn_store  = cdf->cpool_to_deadstemc_store / cndw; 
		ndf->npool_to_livecrootn        = cdf->cpool_to_livecrootc / cnlw; 
		ndf->npool_to_livecrootn_store  = cdf->cpool_to_livecrootc_store / cnlw; 
		ndf->npool_to_deadcrootn        = cdf->cpool_to_deadcrootc / cndw; 
		ndf->npool_to_deadcrootn_store  = cdf->cpool_to_deadcrootc_store / cndw; 

	}

	/* calculate the amount of carbon that needs to go into growth
	respiration storage to satisfy all of the storage growth demands */
	if (epc.veg_type == TREE){
		gresp_store = (cdf->cpool_to_leafc_store + cdf->cpool_to_frootc_store
			+ cdf->cpool_to_livestemc_store + cdf->cpool_to_deadstemc_store
			+ cdf->cpool_to_livecrootc_store + cdf->cpool_to_deadcrootc_store)
			* g1;
	}
	else{
		gresp_store = (cdf->cpool_to_leafc_store+cdf->cpool_to_frootc_store) * g1;
	}
	cdf->cpool_to_gresp_store = gresp_store;
	/*---------------------------------------------------------------------------	*/
	/*	create a maximum lai							*/
	/*---------------------------------------------------------------------------	*/

	excess_lai = (cs->leafc + cs->leafc_transfer + cs->leafc_store + cdf->cpool_to_leafc) * epc.proj_sla - epc.max_lai; 
	if ( excess_lai > ZERO) 
	{
		excess_c = excess_lai / epc.proj_sla;
		if (epc.veg_type == TREE) {
	/*---------------------------------------------------------------------------	*/
	/*	shift allocation to stemwood if leafc max has been reached		*/
	/*---------------------------------------------------------------------------	*/
			excess_allocation_to_leaf = min(cdf->cpool_to_leafc,  excess_c);
			cdf->cpool_to_leafc -= excess_allocation_to_leaf;
			cdf->cpool_to_deadstemc += (1-f4)*excess_allocation_to_leaf;
			cdf->cpool_to_livestemc += f4*excess_allocation_to_leaf;
			ndf->npool_to_leafn = max(ndf->npool_to_leafn - excess_allocation_to_leaf / cnl, 0.0);
			ndf->npool_to_deadstemn += (1-f4)*excess_allocation_to_leaf / cndw ;
			ndf->npool_to_livestemn += f4*excess_allocation_to_leaf / cnlw;
		}
		else {
		     excess_allocation_to_leaf = min(cdf->cpool_to_leafc,  excess_c);
		     cdf->cpool_to_leafc -= excess_allocation_to_leaf;
		     ndf->npool_to_leafn = max(ndf->npool_to_leafn - excess_allocation_to_leaf / cnl, 0.0);
		     cs->cpool += excess_allocation_to_leaf;
		     ns->npool += excess_allocation_to_leaf / cnl;
		}
	}

	
	/*---------------------------------------------------------------------------
	if ((current_date.month == 5) && (current_date.day < 10))
	printf(" \n %lf %lf %lf %lf %lf %lf ",
	gresp_store, cdf->cpool_to_leafc, cdf->cpool_to_leafc_store, cs->leafc,
	cs->leafc_store, cdf->psn_to_cpool);
	---------------------------------------------------------------------------*/
	return(!ok);
} /* end daily_allocation.c */

