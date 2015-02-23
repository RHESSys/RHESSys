/*--------------------------------------------------------------*/
/* 								*/
/*					compute_soil_water_potential	*/
/*								*/
/* 	compute_soil_water_potential - computes pre-dawn leaf water potential*/
/*							*/
/*	NAME						*/
/*							*/
/*	SYNOPSIS					*/
/*	double	compute_soil_water_potential(			*/
/*			int	,			*/
/*			int	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	);			*/
/*							*/
/*	OPTIONS						*/
/*	int	verbose_flag,				*/
/*	int	curve,					*/
/*	double	Tsoil - (deg C)				*/
/*	double	LWP_min_spring - (Mpa)			*/
/*	double	LWP_stom_closure - (Mpa)		*/
/*  	double  psi_air_entry - curve parameter 1                  */
/*      double  pore_size_index - curve parameter 2                  */
/*      double  p_0 - porosity		                */
/*      double  p porosity decay                  	*/
/*	double	sat_deficit- (m)			*/
/*	double	unsat_storage - (m water)		*/
/*	double	rooting_depth				*/
/*	double	field_capacity - (m water)		*/
/*							*/
/*							*/
/*	DESCRIPTION					*/
/*							*/
/*	We make the assumption that the roots are sitting in a		*/
/*	soil or moss column.  We assume that the roots do not		*/
/*	extend through more than one column.  We then assume that	*/
/*	the rooting field capacity is the amount of water that		*/
/*	can be held (at steady state drainage) within the region of	*/
/*	the column occupied by roots ABOVE the saturate zone  		 */
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Sept 24, 1997 RAF					*/
/*	New code.						*/
/*																*/
/*	Oct 15 1997 RAF						*/
/*	water unavailable to roots should have been in terms 	*/
/*	of sat_deficit_z and not rooting_depth			*/
/*																*/
/*	Oct 15 1997 RAF						*/
/*	Changed condition where satdef_z==0 giving no water	*/
/*	stresss to sat_def_z<=0 (i.e. also if ponding)		*/
/*																*/
/*	Oct 15 1997 RAF						*/
/*	LWP_predawn from Brooks Corey was in m water tension.	*/
/*	converted to Mpa.					*/
/*								*/
/*	Nov 6, 1997 RAF						*/
/*	Simpli8fied so that unsat zone soil moisture is assumed	*/
/*	to be distributed such that unsat zone tension is the	*/
/*	same everwhere.						*/
/*								*/
/*	Nov 27 , 1997 RAF					*/
/*	Cleared bug where we were first checking if 		*/
/*	unsat storage is zero before checking if sat deficit was*/
/*	negative.  This meant that if there was ponding there	*/
/*	was stomatal closure - while this may be true we prefer	*/
/*	to assume that the ponding is statistical (in low areas)*/
/*	and is currently indicative of NO stress to leaves.	*/
/*	So we have switched the order of these checks.		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_soil_water_potential(
							int	verbose_flag,
							int	curve,
							double	Tsoil,
							double	LWP_min_spring,
							double	LWP_stom_closure,
							double	psi_air_entry,
							double 	pore_size_index,
							double  p3,
							double  p4,
							double  p_0,
							double  p,
							double	S)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	LWP_predawn;
	/*--------------------------------------------------------------*/
	/*      Make sure p and p_0 are non zero.                       */
	/*--------------------------------------------------------------*/
	p = max(p,0.00000001);
	p_0 = max(p_0,0.00000001);
	/*--------------------------------------------------------------*/
	/*	compute soil water storage				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	We assume that roots BELOW the water table do not 	*/
	/*	uptake water actively.					*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	We assume that water is distribued in the unsat zone 	*/
	/*	so that relative effective moisture content is uniform	*/
	/*	with z.  Then no matter how deep the roots are they 	*/
	/*	still sense the same unsat zone suction.		*/
	/*	Note that the other extreme, that water fills the 	*/
	/*	unsat zone from bottom up, will make predawn LWP 	*/
	/*	sensitive to rooting depth in the unsat zone.  However	*/
	/*	for wet areas or areas where there is hydraulic lift 	*/
	/*	the current assumption may be useful on a daily time stp*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Now we use Clapp and Hornberger relationship 	 	*/
	/*	or the Can Genuchten relation ship to			*/
	/*	define psi-theta curve					*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	If stot == 0 we set LWP_predawn to its maximum value	*/
	/*	this is a problem but if unsat_storage is zero there	*/
	/*	is actually lots of water				*/
	/*--------------------------------------------------------------*/
	if (S > 1.0) {
		LWP_predawn = LWP_min_spring;
	}
	else{
		/*--------------------------------------------------------------*/
		/*	Compute predawn LWP without soil temp adjustment.	*/
		/*	This is converted from m water tension to MPa using	*/
		/*	1m water tension = 10000 Pa				*/
		/*			= 0.01 Mpa				*/
		/*--------------------------------------------------------------*/
		switch(curve) {

		case 1: 
			LWP_predawn = min(-1.0 * 0.01 * ( psi_air_entry
				* pow ( S , -1/pore_size_index) ), LWP_min_spring);
				/*printf("\n%lf %lf %lf %lf %lf",psi_air_entry,S,pore_size_index,LWP_min_spring,LWP_predawn);*/
			break;
		case 2:
			if (S > ZERO) 
				LWP_predawn = min( -1.0 * 0.01 * psi_air_entry
				* pow( pow(1/S,1/(1-1/pore_size_index)) -1 , 1/pore_size_index )
					, LWP_min_spring);
			else LWP_predawn = LWP_stom_closure;
			break;
		case 3:
			if (S > ZERO) 
				LWP_predawn = -1.0*exp(p3+p4*log(S));
			else LWP_predawn = LWP_stom_closure;
			break;
		}
	}
	/*--------------------------------------------------------------*/
	/*	Increase LWP is soil is cold.				*/
	/*	(I am not sure where the factor of 2.0 came from	*/
	/*--------------------------------------------------------------*/
	if ( Tsoil < 0.0000000001 )
		LWP_predawn = LWP_predawn * 2.0;
	/*--------------------------------------------------------------*/
	/*	Limit the lowest LWP to that of stomatal closure	*/
	/*--------------------------------------------------------------*/
	LWP_predawn = max(LWP_predawn, LWP_stom_closure);
	/*printf(" %lf %lf",Tsoil,LWP_predawn);*/
	return(LWP_predawn);
} /*end compute_soil_water_potential)*/
