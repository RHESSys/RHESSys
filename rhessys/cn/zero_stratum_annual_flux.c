/*--------------------------------------------------------------*/
/* 								*/
/*		zero_stratum_annual_flux				*/
/*								*/
/*								*/
/*	NAME							*/
/*	zero_stratum_annual_flux -  rain intercepted by stratum and 	*/
/*		updates rain flaaing through.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	zero_stratum_annual_flux(				*/
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

int zero_stratum_annual_flux(struct cstate_struct *cs,
							 struct cannflux_struct* caf,
							 struct nannflux_struct* naf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;
	/* sets all the annual flux variables to 0.0 at the beginning of the day */
	/* annual carbon fluxes */
/*
	cs->gpsn_src = 0.0;
	cs->leaf_mr_snk = 0.0;
	cs->livestem_mr_snk = 0.0;
	cs->livecroot_mr_snk = 0.0;
	cs->froot_mr_snk = 0.0;
	cs->leaf_gr_snk = 0.0;
	cs->livestem_gr_snk = 0.0;
	cs->livecroot_gr_snk = 0.0;
	cs->deadstem_gr_snk = 0.0;
	cs->froot_gr_snk = 0.0;
	cs->deadcroot_gr_snk = 0.0;
*/
	/* annual nitrogen fluxes */
	return (!ok);
}/*end zero_stratum_annual_flux.c*/


	
    
    
