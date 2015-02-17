/*--------------------------------------------------------------*/
/* 								*/
/*		compute_N_demand					*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_N_demand -				*/
/*		computes potential N uptake from soil		*/
/*		for this strata without mineralization		*/
/*		limitation					*/
/*								*/
/*	SYNOPSIS						*/
/*	int compute_N_demand(					*/
/*                          struct epconst_struct,              */
/*                          struct cstate_struct *,             */
/*			    double  growth_cn,			*/
/*                          struct command_line_object *)           */
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
double compute_N_demand(
								  struct	epconst_struct epc,
								  struct cstate_struct *cs,
								  double growth_cn,
								  struct command_line_object *command_line)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	double ctoreprod, ctogrowth, plant_ndemand, total_biomass;

	if (growth_cn > ZERO)	{
	if (command_line[0].reproduction_flag == 1) {
		total_biomass = cs->leafc+cs->frootc;
		if (epc.veg_type==TREE) {
		total_biomass =  total_biomass + (cs->dead_stemc+cs->live_stemc+
			cs->dead_crootc+cs->live_crootc);
		}
		if (total_biomass > ZERO) {
			if (cs->cpool/total_biomass > epc.cpool_fract_reprod_thresh) {
				ctoreprod =  min(1.0,epc.alloc_cpool_reprodc * (1-epc.storage_transfer_prop));
				ctogrowth =  max(0.0,1.0-ctoreprod);

				plant_ndemand = cs->availc / (1.0+epc.gr_perc) / 
					((ctogrowth*growth_cn + ctoreprod*epc.seed_cn)/(ctogrowth+ctoreprod));
				}
			else
				plant_ndemand = cs->availc / (1.0+epc.gr_perc) / growth_cn;
		}
	}
	else
		plant_ndemand = cs->availc * 1.0/growth_cn;

	}
	else
		plant_ndemand = 0.0;


	return(plant_ndemand);

} /* 	end compute_N_demand */
