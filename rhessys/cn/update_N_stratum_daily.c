/*--------------------------------------------------------------*/
/* 								*/
/*		update_N_stratum_daily				*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_N_stratum_daily -					*/
/*								*/
/*		updates daily C stores to			*/
/*		account for psn and respiration 		*/
/*	SYNOPSIS						*/
/*	double	update_N_stratum_daily(					*/
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

int update_N_stratum_daily(			 struct epconst_struct epc,
						   struct nstate_struct *ns, struct ndayflux_struct *ndf,
						   struct soil_n_object *ns_soil)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;

	
	if (epc.veg_type == TREE){
		ns->preday_totaln = (ns->npool+ns->leafn+ns->leafn_store+ns->leafn_transfer+ns->frootn_store+ns->frootn_transfer+ns->frootn +
		ns->live_stemn + ns->livestemn_transfer + ns->livestemn_store +
		ns->dead_stemn + ns->deadstemn_transfer + ns->deadstemn_store +
		ns->live_crootn + ns->livecrootn_transfer + ns->livecrootn_store +
		ns->dead_crootn + ns->deadcrootn_transfer + ns->deadcrootn_store + ns->cwdn + ns->retransn);
	}
	else {
		ns->preday_totaln = (ns->npool+ns->leafn+ns->leafn_store+ns->leafn_transfer+ns->frootn_store+ns->frootn_transfer+ns->frootn +
		ns->cwdn + ns->retransn);
	}

	/* Plant allocation flux, from N retrans pool and soil mineral N pool */
	ns->npool      += ndf->retransn_to_npool;
	ns->retransn   -= ndf->retransn_to_npool;
	ns->npool      += ndf->sminn_to_npool;


	/* Daily allocation fluxes */
	/* Daily leaf allocation fluxes */
	ns->leafn          += ndf->npool_to_leafn;
	ns->npool          -= ndf->npool_to_leafn;
	ns->leafn_store  += ndf->npool_to_leafn_store;
	ns->npool          -= ndf->npool_to_leafn_store;
	/* Daily fine root allocation fluxes */
	ns->frootn         += ndf->npool_to_frootn;
	ns->npool          -= ndf->npool_to_frootn;
	ns->frootn_store += ndf->npool_to_frootn_store;
	ns->npool          -= ndf->npool_to_frootn_store;
	if (epc.veg_type == TREE){
		/* Daily live stem allocation fluxes */
		ns->live_stemn          += ndf->npool_to_livestemn;
		ns->npool              -= ndf->npool_to_livestemn;
		ns->livestemn_store  += ndf->npool_to_livestemn_store;
		ns->npool              -= ndf->npool_to_livestemn_store;
		/* Daily dead stem allocation fluxes */
		ns->dead_stemn          += ndf->npool_to_deadstemn;
		ns->npool              -= ndf->npool_to_deadstemn;
		ns->deadstemn_store  += ndf->npool_to_deadstemn_store;
		ns->npool              -= ndf->npool_to_deadstemn_store;
		/* Daily live coarse root allocation fluxes */
		ns->live_crootn         += ndf->npool_to_livecrootn;
		ns->npool              -= ndf->npool_to_livecrootn;
		ns->livecrootn_store += ndf->npool_to_livecrootn_store;
		ns->npool              -= ndf->npool_to_livecrootn_store;
		/* Daily dead coarse root allocation fluxes */
		ns->dead_crootn         += ndf->npool_to_deadcrootn;
		ns->npool              -= ndf->npool_to_deadcrootn;
		ns->deadcrootn_store += ndf->npool_to_deadcrootn_store;
		ns->npool              -= ndf->npool_to_deadcrootn_store;
	}
	/*------------------------------------------------------*/
	/*	return any excess nitrogen to the soil		*/
	/*------------------------------------------------------*/
	if (epc.veg_type == TREE){
		ns->totaln = (ns->npool+ns->leafn+ns->leafn_store+ns->leafn_transfer+ns->frootn_store+ns->frootn_transfer+ns->frootn +
		ns->live_stemn + ns->livestemn_transfer + ns->livestemn_store +
		ns->dead_stemn + ns->deadstemn_transfer + ns->deadstemn_store +
		ns->live_crootn + ns->livecrootn_transfer + ns->livecrootn_store +
		ns->dead_crootn + ns->deadcrootn_transfer + ns->deadcrootn_store + ns->cwdn + ns->retransn);
	}
	else {
		ns->totaln = (ns->npool+ns->leafn+ns->leafn_store+ns->leafn_transfer+ns->frootn_store+ns->frootn_transfer+ns->frootn +
		ns->cwdn + ns->retransn);
	}


	return (!ok);
}/*end update_N_stratum_daily.c*/		

