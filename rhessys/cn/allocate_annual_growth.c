/*--------------------------------------------------------------*/
/* 								*/
/*								*/
/*	allocate_annual_growth				*/
/*								*/
/*	NAME							*/
/*		allocate_annual_growth			*/
/*								*/
/*	SYNOPSIS						*/
/*	int allocate_annual_growth( int , int, double, double,	*/
/*			    struct cdayflux_struct *,  		*/
/*			    struct cstate_struct *,		*/
/*			    struct ndayflux_struct *,	 	*/
/*			    struct nstate_struct *, 		*/
/*			    struct ndayflux_patch_struct *, 	*/
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

int allocate_annual_growth(				int id,
							int default_ID,
							int vmort_flag,
							double cover_fraction,
							double cpool_mort_fract,
						   struct epvar_struct *epv,
						   struct cdayflux_struct *cdf,
						   struct cstate_struct *cs,
						   struct ndayflux_struct *ndf,
						   struct nstate_struct *ns,
						   struct cdayflux_patch_struct *cdf_patch,
						   struct ndayflux_patch_struct *ndf_patch,
						   struct litter_c_object *cs_litr,
						   struct litter_n_object *ns_litr,
						   struct epconst_struct epc,
						   struct command_line_object *command_line)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/

	void	update_mortality(
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

	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double storage_transfer_prop;
	double delta_livestemc, delta_deadstemc;
	double delta_livecrootc, delta_deadcrootc;
	double delta_frootc, delta_frootn;
	double delta_nitrogen, delta_carbon;
	double cnl, cnlw, cndw, cnfr, mean_cn;
	double fcroot, flive, fdead, fleaf, froot, fwood;
	double total_wood_c, total_wood_n, wood_cn;
	double rem_excess_carbon, excess_carbon, transfer_carbon, excess_nitrogen;
	double unmetn, carbohydrate_transfer_n;
	double plantc, excess_lai, excess_leaf_carbon, stemc, leafc;
	double delta_leaf, leaf_growth_deficit;
	double total_store, ratio, total_above_biomass,total_biomass, carbohydrate_transfer;
	struct mortality_struct mort;


	fcroot = epc.alloc_crootc_stemc;
	flive = epc.alloc_livewoodc_woodc;
	fdead = (1-flive);
	cnl = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;

	/*--------------------------------------------------------------*/
	/* carbon store transfers */
	/* Move store material into transfer compartments on the annual
	allocation day. This is a special case, where a flux is assessed in
	the state_variable update routine.  This is required to have the
	allocation of excess C and N show up as new growth in the next growing
	season, instead of two growing seasons from now. */


	/*--------------------------------------------------------------*/
	/*	check to make sure LAI is not greater than user specified */
	/*	max lai (note LAI here does NOT take into account sunlit */
	/*	shaded compenent - so LAI output by RHESSys may sometime */
	/*	get to be slightly higher than specified max LAI */
	/*	carbon in excess of that required for max LAI */
	/*	is re-allocated to stemwood; for grasses it is allocated to roots */
	/*--------------------------------------------------------------*/
   	excess_lai = (cs->leafc + cs->leafc_store + cs->leafc_transfer) * epc.proj_sla - epc.max_lai;
 	if ( excess_lai > ZERO) {
 
                excess_carbon = excess_lai / epc.proj_sla;
 		rem_excess_carbon = excess_carbon;
 		if (epc.veg_type == TREE) {
 			/* remove excess carbon from storage, transfer and then leaf carbon until gone */
 			if (cs->leafc_store > excess_carbon) {
                 		cs->leafc_store -= rem_excess_carbon;
                 		ns->leafn_store -= rem_excess_carbon / epc.leaf_cn;
 				}
 			else {
 				rem_excess_carbon -= cs->leafc_store;
 				cs->leafc_store = 0.0;
 				ns->leafn_store = 0.0;
 				if (cs->leafc_transfer > rem_excess_carbon) {
 					cs->leafc_transfer -= rem_excess_carbon;
                 			ns->leafn_transfer -= rem_excess_carbon / epc.leaf_cn;
 					}
 				else {
 					rem_excess_carbon -= cs->leafc_transfer;
 					cs->leafc_transfer = 0.0;
 					ns->leafn_transfer = 0.0;
 					cs->leafc -= rem_excess_carbon;
                 			ns->leafn -= rem_excess_carbon / epc.leaf_cn;
 					}
 			}
 
                 	cs->deadstemc_store += (1-epc.alloc_livewoodc_woodc)*excess_carbon;
                 	cs->livestemc_store+= epc.alloc_livewoodc_woodc*excess_carbon;
                 	ns->deadstemn_store += (1-epc.alloc_livewoodc_woodc)*excess_carbon / epc.deadwood_cn;
                 	ns->livestemn_store += epc.alloc_livewoodc_woodc*excess_carbon / epc.livewood_cn;
 			excess_nitrogen = excess_carbon / epc.leaf_cn -
 			   (1-epc.alloc_livewoodc_woodc)*excess_carbon / epc.deadwood_cn -
 			    epc.alloc_livewoodc_woodc*excess_carbon / epc.livewood_cn;
 			ns->npool += excess_nitrogen;
 		}
 		else {
 			/* remove excess carbon from storage, transfer and then leaf carbon until gone */
 			if (cs->leafc_store > excess_carbon) {
                 		cs->leafc_store -= rem_excess_carbon;
                 		ns->leafn_store -= rem_excess_carbon / epc.leaf_cn;
 				}
 			else {
 				rem_excess_carbon -= cs->leafc_store;
 				cs->leafc_store = 0.0;
 				ns->leafn_store = 0.0;
 				if (cs->leafc_transfer > rem_excess_carbon) {
 					cs->leafc_transfer -= rem_excess_carbon;
                 			ns->leafn_transfer -= rem_excess_carbon / epc.leaf_cn;
 					}
 				else {
 					rem_excess_carbon -= cs->leafc_transfer;
 					cs->leafc_transfer = 0.0;
 					ns->leafn_transfer = 0.0;
 					cs->leafc -= rem_excess_carbon;
                 			ns->leafn -= rem_excess_carbon / epc.leaf_cn;
 					}
 			}
			cs->frootc_store += excess_carbon;
			ns->frootn_store += excess_carbon / epc.froot_cn;
			excess_nitrogen = excess_carbon/epc.leaf_cn - excess_carbon/epc.froot_cn;
			ns->npool += excess_nitrogen;

 		}
         }

	total_store =  cs->cpool;
	
	/* Changed to just be live C */
	/*total_biomass =  (cs->leafc + cs->frootc + cs->live_stemc + cs->dead_stemc +
			cs->live_crootc + cs->dead_crootc);*/
	if (cs->cpool > ZERO) 
	total_biomass =  (cs->leafc + cs->frootc + cs->live_stemc + cs->live_crootc + cs->cpool);
	else
	total_biomass =  (cs->leafc + cs->frootc + cs->live_stemc + cs->live_crootc);

	/* Changed to just be live C */
	/*total_above_biomass =  cs->leafc+cs->dead_stemc+cs->live_stemc;*/
	total_above_biomass =  cs->leafc+cs->live_stemc;
	
	if (total_biomass > ZERO)
		ratio = (total_store/total_biomass);
	else
		ratio = 1.0;

        /* if (epc.max_storage_percent > ZERO)
	              storage_transfer_prop = 1.0 - (1.0  - epc.storage_transfer_prop) * (epc.max_storage_percent - ratio)/ epc.max_storage_percent;
	*/
         if ((epc.max_storage_percent > ZERO) && (ratio < epc.max_storage_percent))
		storage_transfer_prop = epc.storage_transfer_prop;
        else
               storage_transfer_prop = 1.0;

	storage_transfer_prop = min(storage_transfer_prop,1.0);


	/*--------------------------------------------------------------*/
	/* 	carbohydrate starvation mortality 			*/
	/*--------------------------------------------------------------*/

	cs->mortality_fract = 0.0;
	if ((total_store < cpool_mort_fract*total_biomass) && (total_biomass > ZERO) && (cs->age > 1) && (vmort_flag == 1)) {
		printf("\n drought stress mortality for %d", id);
		excess_carbon = 1.0 - total_store/(cpool_mort_fract*total_biomass);
		excess_carbon = max(excess_carbon,0);
		excess_carbon = min(1.0,excess_carbon);
		cs->mortality_fract = excess_carbon;
		mort.mort_cpool = excess_carbon;
		mort.mort_leafc = excess_carbon;
		mort.mort_deadleafc = excess_carbon;
		mort.mort_livestemc = excess_carbon;
		mort.mort_deadstemc = excess_carbon;
		mort.mort_livecrootc = excess_carbon;
		mort.mort_deadcrootc = excess_carbon;
		mort.mort_frootc = excess_carbon;					
		update_mortality(epc,
						 cs, cdf, cdf_patch,
						 ns, ndf, ndf_patch, 
						 cs_litr, ns_litr, 1,
						 mort);	
	}
		
	
	/*--------------------------------------------------------------*/
	/*	respiration thinning			*/
	/*	now substituting carbon hydrat storage mortality 		*/
	/*--------------------------------------------------------------*/
/*
	if ((vmort_flag == 1) && ((cs->leafc_store+cs->leafc) < epc.min_percent_leafg*epv->prev_leafcalloc) ) {

		plantc =  (cs->live_crootc
			+ cs->live_stemc + cs->dead_crootc
			+ cs->dead_stemc + cs->livecrootc_store
			+ cs->livestemc_store + cs->deadcrootc_store
			+ cs->deadstemc_store
			+ cs->livecrootc_transfer
			+ cs->livestemc_transfer
			+ cs->deadcrootc_transfer
			+ cs->deadstemc_transfer
			+ cs->leafc + cs->leafc_transfer + cs->leafc_store
			+ cs->frootc + cs->frootc_transfer + cs->frootc_store);

		leafc =	 cs->leafc;
		stemc = 
			 cs->live_stemc 
			+ cs->dead_stemc ;
		
		if (leafc/stemc > 0.6)
			excess_leaf_carbon = stemc*(leafc/stemc - 0.6); 
		else
			excess_leaf_carbon = 0.0;

		excess_leaf_carbon = min(cs->leafc, excess_leaf_carbon);
		excess_leaf_carbon = max(0.0, excess_leaf_carbon);
		excess_leaf_carbon = min(-1.0*cs->cpool, excess_leaf_carbon);

		excess_carbon = min(1.0, -1.0*cs->cpool/plantc - excess_leaf_carbon);

		cs->leafc -= excess_leaf_carbon;
		ns->leafn -= excess_leaf_carbon/epc.leaf_cn;

		cs->cpool += excess_leaf_carbon;
		ns->npool += excess_leaf_carbon/epc.leaf_cn;

		update_mortality(epc,
			cs, cdf, cdf_patch,
			ns, ndf, ndf_patch, 
			cs_litr, ns_litr, cover_fraction, excess_carbon);
		}
*/




	/*--------------------------------------------------------------*
	/*  we include a delay on storage output so that the
		veg does not die in a bad year -esp. for Grasses	*/
	/*--------------------------------------------------------------*/
	cdf->leafc_store_to_leafc_transfer = cs->leafc_store * storage_transfer_prop;
	cs->cpool += cs->leafc_store * (1.0-storage_transfer_prop);
	cs->leafc_store = 0.0;

	cdf->frootc_store_to_frootc_transfer=cs->frootc_store* storage_transfer_prop;
	cs->cpool += cs->frootc_store * (1.0-storage_transfer_prop);
	cs->frootc_store = 0.0;
	cdf->gresp_store_to_gresp_transfer = cs->gresp_store * storage_transfer_prop;
	cs->cpool += cs->gresp_store * (1.0-storage_transfer_prop);
	cs->gresp_store = 0.0;
	if (epc.veg_type == TREE){
		cdf->livestemc_store_to_livestemc_transfer = cs->livestemc_store
			* storage_transfer_prop;
		cs->cpool += cs->livestemc_store * (1.0-storage_transfer_prop);
		cs->livestemc_store = 0.0;
		cdf->deadstemc_store_to_deadstemc_transfer = cs->deadstemc_store
			* storage_transfer_prop;
		cs->cpool += cs->deadstemc_store * (1.0-storage_transfer_prop);
		cs->deadstemc_store = 0.0;
		cdf->livecrootc_store_to_livecrootc_transfer = cs->livecrootc_store
			* storage_transfer_prop;
		cs->cpool += cs->livecrootc_store * (1.0-storage_transfer_prop);
		cs->livecrootc_store = 0.0;
		cdf->deadcrootc_store_to_deadcrootc_transfer = cs->deadcrootc_store
			* storage_transfer_prop;
		cs->cpool += cs->deadcrootc_store * (1.0-storage_transfer_prop);
		cs->deadcrootc_store = 0.0;
	}

	/* nitrogen transfers */
	/* for grasses, we include a delay on storage output so that the
	grasses do not die in a bad year				*/

	ndf->leafn_store_to_leafn_transfer = ns->leafn_store * storage_transfer_prop;
	ns->npool += ns->leafn_store * (1.0-storage_transfer_prop);
	ns->leafn_store = 0.0;
	ndf->frootn_store_to_frootn_transfer=ns->frootn_store* storage_transfer_prop;
	ns->npool += ns->frootn_store * (1.0-storage_transfer_prop);
	ns->frootn_store = 0.0;
	if (epc.veg_type == TREE){
		ndf->livestemn_store_to_livestemn_transfer = ns->livestemn_store
			* storage_transfer_prop;
		ns->npool += ns->livestemn_store * (1.0-storage_transfer_prop);
		ns->livestemn_store = 0.0;
		ndf->deadstemn_store_to_deadstemn_transfer = ns->deadstemn_store
			* storage_transfer_prop;
		ns->npool += ns->deadstemn_store * (1.0-storage_transfer_prop);
		ns->deadstemn_store = 0.0;
		ndf->livecrootn_store_to_livecrootn_transfer = ns->livecrootn_store
			* storage_transfer_prop;
		ns->npool += ns->livecrootn_store * (1.0-storage_transfer_prop);
		ns->livecrootn_store = 0.0;
		ndf->deadcrootn_store_to_deadcrootn_transfer = ns->deadcrootn_store
			* storage_transfer_prop;
		ns->npool += ns->deadcrootn_store * (1.0-storage_transfer_prop);
		ns->deadcrootn_store = 0.0;
	}
	
	
	/*--------------------------------------------------------------*/
	/* use carbohydrates if stressed				*/
	/*--------------------------------------------------------------*/
	
	if (total_above_biomass > ZERO)
		 excess_carbon = (cdf->leafc_store_to_leafc_transfer+cs->leafc) - epc.min_percent_leafg*total_biomass;  
	else
		excess_carbon = 0.0;

	if (excess_carbon < -ZERO) {

		carbohydrate_transfer = -1.0*excess_carbon; 


		fleaf = exp(-1.0*epc.dickenson_pa * epv->proj_lai);
		fleaf = min(fleaf, 1.0);

		if (epc.veg_type==TREE) {
			froot = 0.5*(1-fleaf);
			fwood = 0.5*(1-fleaf);
		}
		else {
			fwood = 0;
			froot = (1-fleaf);
		}

		fleaf = max(0.0, fleaf);
		
		if (epc.veg_type == TREE){
		mean_cn = 1.0 / (fleaf / cnl + froot / cnfr + flive * fwood / cnlw + fwood * fdead / cndw);
		}
		else{
	   	mean_cn = 1.0 / (fleaf / cnl + froot / cnfr);
		}

		carbohydrate_transfer = carbohydrate_transfer/fleaf;

		carbohydrate_transfer = min(cs->cpool, carbohydrate_transfer);		
		carbohydrate_transfer = max(carbohydrate_transfer, 0.0);

		if (mean_cn > ZERO)
			carbohydrate_transfer_n = carbohydrate_transfer/mean_cn;
		else	
			carbohydrate_transfer_n = 0.0;

		if (carbohydrate_transfer > ZERO) {
		unmetn = carbohydrate_transfer_n - ns->npool;
		unmetn = max(unmetn, 0.0);
		}
		else unmetn=0.0;

		ns->retransn = max(ns->retransn, 0.0);
		if (unmetn > ns->retransn)   {
			ndf->retransn_to_npool += ns->retransn;
			carbohydrate_transfer = (ns->npool+ns->retransn)*mean_cn;
			}
			else {
			ndf->retransn_to_npool += unmetn;
			}	
				

		cdf->leafc_store_to_leafc_transfer += carbohydrate_transfer * fleaf;
		cdf->frootc_store_to_frootc_transfer += carbohydrate_transfer * froot;
		ndf->leafn_store_to_leafn_transfer += (carbohydrate_transfer * fleaf) / cnl;
		ndf->frootn_store_to_frootn_transfer += (carbohydrate_transfer * froot) / cnfr ;
			
		if (epc.veg_type == TREE){
			cdf->livestemc_store_to_livestemc_transfer += carbohydrate_transfer * fwood * (1.0-fcroot) * flive;
			cdf->livecrootc_store_to_livecrootc_transfer += carbohydrate_transfer * fwood * fcroot * flive;
			cdf->deadstemc_store_to_deadstemc_transfer += carbohydrate_transfer * fwood * (1.0-fcroot) * fdead;
			cdf->deadcrootc_store_to_deadcrootc_transfer += carbohydrate_transfer * fwood * fcroot * fdead;
			ndf->livestemn_store_to_livestemn_transfer += (carbohydrate_transfer * fwood * (1.0-fcroot) * flive) / cnlw;
			ndf->livecrootn_store_to_livecrootn_transfer += (carbohydrate_transfer * fwood * fcroot * flive) / cnlw;
			ndf->deadstemn_store_to_deadstemn_transfer += (carbohydrate_transfer * fwood * (1.0-fcroot) * fdead) / cndw;
			ndf->deadcrootn_store_to_deadcrootn_transfer += (carbohydrate_transfer * fwood * fcroot * fdead) / cndw;
		}

		cs->cpool -= carbohydrate_transfer;
		if (mean_cn > ZERO)
			ns->npool -= carbohydrate_transfer/mean_cn;
	}
	/*--------------------------------------------------------------*/
	/* finally if there is really nothing restart with small amount of growth   */
	/*	we allow only a certain amount of resprouting based on 	*/
	/*	a stratum default file parameterization 		*/
	/*--------------------------------------------------------------*/
	if ((cdf->leafc_store_to_leafc_transfer + cs->leafc) < epc.min_leaf_carbon) {
		if (cs->num_resprout < epc.max_years_resprout) {

		//printf("\n Resprouting stratum %d", id);
		cs->num_resprout += 1;
		cs->age = 0;
		cs->cpool = 0.0;
		ns->npool = 0.0;
		cs->leafc_store = epc.resprout_leaf_carbon;
		cs->frootc_store = cs->leafc_store * epc.alloc_frootc_leafc;
		cdf->leafc_store_to_leafc_transfer = cs->leafc_store;
		cdf->frootc_store_to_frootc_transfer = cs->frootc_store;
		ns->leafn_store = cs->leafc_store / epc.leaf_cn;
		ns->frootn_store = cs->frootc_store / epc.froot_cn;
		ndf->leafn_store_to_leafn_transfer = ns->leafn_store;
		ndf->frootn_store_to_frootn_transfer = ns->frootn_store;
		cs->leafc_transfer = 0.0;
		cs->leafc = 0.0;
		cs->frootc_transfer = 0.0;
		ns->leafn_transfer = 0.0;
		ns->frootn_transfer = 0.0;
		ns->leafn = 0.0;
		cdf->gresp_store_to_gresp_transfer = 0.0;
		epv->prev_leafcalloc = epc.resprout_leaf_carbon;

		if (epc.veg_type == TREE) {
			cs->livecrootc_store = epc.resprout_leaf_carbon * epc.alloc_stemc_leafc;
			ns->livecrootn_store = cs->livecrootc_store / epc.livewood_cn;
			cdf->livecrootc_store_to_livecrootc_transfer = cs->livecrootc_store;
			ndf->livecrootn_store_to_livecrootn_transfer = ns->livecrootn_store;

			cs->livestemc_store = epc.resprout_leaf_carbon * epc.alloc_stemc_leafc;
			ns->livestemn_store = cs->livestemc_store / epc.livewood_cn;
			cdf->livestemc_store_to_livestemc_transfer = cs->livestemc_store;
			ndf->livestemn_store_to_livestemn_transfer = ns->livestemn_store;

			cs->live_stemc = 0.0;
			cs->dead_stemc = 0.0;
			cs->live_crootc = 0.0;
			cs->dead_crootc = 0.0;
			ns->live_stemn = 0.0;
			ns->dead_stemn = 0.0;
			ns->live_crootn = 0.0;
			ns->dead_crootn = 0.0;

			cs->deadstemc_store =  0.0;
			cs->deadcrootc_store =  0.0;
			ns->deadstemn_store =  0.0;
			ns->deadcrootn_store =  0.0;

			cs->livestemc_transfer =  0.0;
			cs->deadstemc_transfer =  0.0;
			cs->livecrootc_transfer =  0.0;
			cs->deadcrootc_transfer =  0.0;
			ns->livestemn_transfer =  0.0;
			ns->deadstemn_transfer =  0.0;
			ns->livecrootn_transfer =  0.0;
			ns->deadcrootn_transfer =  0.0;

			cdf->livestemc_store_to_livestemc_transfer = 0.0;
			cdf->deadstemc_store_to_deadstemc_transfer = 0.0;
			cdf->livecrootc_store_to_livecrootc_transfer = 0.0;
			cdf->deadcrootc_store_to_deadcrootc_transfer = 0.0;

			ndf->livestemn_store_to_livestemn_transfer = 0.0;
			ndf->deadstemn_store_to_deadstemn_transfer = 0.0;
			ndf->livecrootn_store_to_livecrootn_transfer = 0.0;
			ndf->deadcrootn_store_to_deadcrootn_transfer = 0.0;
			
		} /* end if TREE */
		} /* end if resprout */
	} /* end if less than min_leaf_carbon */
	else  {
		 // cs->num_resprout = max(cs->num_resprout-1,0);	
		 cs->age += 1;
		 }

	/* update states variables */
	cs->leafc_transfer    += cdf->leafc_store_to_leafc_transfer;
	cs->frootc_transfer   += cdf->frootc_store_to_frootc_transfer;
	cs->gresp_transfer    += cdf->gresp_store_to_gresp_transfer;

	/* remaining store goes back to cpool - carbohydrate pool */
	epv->prev_leafcalloc = cs->leafc+cs->leafc_transfer;
	if (epc.veg_type == TREE){
		cs->livestemc_transfer  += cdf->livestemc_store_to_livestemc_transfer;
		cs->deadstemc_transfer  += cdf->deadstemc_store_to_deadstemc_transfer;
		cs->livecrootc_transfer += cdf->livecrootc_store_to_livecrootc_transfer;
		cs->deadcrootc_transfer += cdf->deadcrootc_store_to_deadcrootc_transfer;
	}
	
		
	/* update states variables */
	ns->leafn_transfer    += ndf->leafn_store_to_leafn_transfer;
	ns->frootn_transfer   += ndf->frootn_store_to_frootn_transfer;
	if (epc.veg_type == TREE){
		ns->livestemn_transfer  += ndf->livestemn_store_to_livestemn_transfer;
		ns->deadstemn_transfer  += ndf->deadstemn_store_to_deadstemn_transfer;
		ns->livecrootn_transfer += ndf->livecrootn_store_to_livecrootn_transfer;
		ns->deadcrootn_transfer += ndf->deadcrootn_store_to_deadcrootn_transfer;
	}
	
	/* for deciduous system, force leafc and frootc to exactly 0.0 on the
	last day */
	if (epc.phenology_type != EVERGREEN){
		if (ns->leafn < 1e-10)  {
			ns->leafn = 0.0;
			cs->leafc = 0.0;
		}
		if (ns->frootn < 1e-10) {
			ns->frootn = 0.0;
			cs->frootc = 0.0;
		}
	}

	return (!ok);
} /* end allocate_annual_growth */
