/*--------------------------------------------------------------*/
/* 								*/
/*			compute_prop_alloc_daily.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_prop_alloc_daily - 					*/
/*		computes maintenance respiration		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_prop_alloc_daily( 				*/
/*					);			*/	
/*								*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	used to allocate daily rather than annual in cases 	*/
/*      where leaves are below some threshold 			*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"


double 		compute_prop_alloc_daily(
		double curr_daily_alloc,
		struct cstate_struct *cs,
		struct epconst_struct epc)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double prop;
	double total_biomass, excess_carbon;


	total_biomass =  (cs->leafc+cs->frootc+cs->dead_stemc+cs->live_stemc+
			cs->dead_crootc+cs->live_crootc);

	if (total_biomass > ZERO)
		 excess_carbon = (cs->leafc) - epc.min_percent_leafg*total_biomass;  
	else
		excess_carbon = 0.0;

	if (excess_carbon < ZERO)
		prop = 1;
	else
		prop = curr_daily_alloc;

	return (prop);
} /*compute_prop_alloc_daily*/
