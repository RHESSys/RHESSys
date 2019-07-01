/*--------------------------------------------------------------*/
/* 								*/
/*			compute_xylem_conductance	*/
/*								*/
/*	compute_xylem_conductance - computes conductance to H20	*/
/*				of a single mean vascular stratum		*/
/*								*/
/*	NAME							*/
/*	compute_xylem_conductance 				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_xylem_conductance(			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*	follows the TREES approach to reducing xylem conductance */
/* 	due to low soil water potential and recovers based 	*/
/*	on days after recovery of soil water above some 	*/
/*	treshold value						*/
/*      mackay et al (2015) Interdependence of hydraulic...	*/
/*	Water Resources Research				*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	compute_xylem_conductance(
											 double	LWP_predawn,
											 struct epconst_struct epc,
											 struct	canopy_strata_object *stratum)
											 
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/

	double gxylem;

	/*--------------------------------------------------------------*/
	/* detemine if LWP is low enough to cause reduction in xylem conductance */
	/* only do this if stomata are open (e.g greater than curticular conductance 				*/
	/*--------------------------------------------------------------*/

	if ((LWP_predawn < epc.LWP_gxylem_min) && (stratum[0].gs_sunlit >= epc.gxylem_min_gs) && 
				(stratum[0].gs_shade >= epc.gxylem_min_gs)) {
	stratum[0].gxylem = epc.gxylem_max * exp(pow( (-1.0*LWP_predawn/epc.gxylem_bsat),epc.gxylem_csat));
	}


	if ((stratum[0].gxylem < epc.gxylem_max) && (LWP_predawn > epc.LWP_gxylem_min)) {
		stratum[0].gxylem  += epc.gxylem_recovery_rate;
	}
	
	stratum[0].gxylem = min(epc.gxylem_max, stratum[0].gxylem);
	stratum[0].gxylem = max(stratum[0].gxylem,0.0);


	
	return;
} /*end compute_xylem_conductance*/
