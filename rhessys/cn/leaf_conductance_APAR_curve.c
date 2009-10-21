/*--------------------------------------------------------------*/
/*								*/
/*		leaf_conductance_APAR_curve			*/
/*								*/
/*	leaf_conductance_APAR_curve				*/
/*								*/
/*	NAME							*/
/*	leaf_conductance_APAR_curve				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	leaf_conductance_APAR_curve(	double,		*/
/*						double,		*/
/*								*/
/*	returns: 						*/
/*	m_APAR	(DIM)	APAR multiplier on leaf conductance	*/
/*								*/
/*	OPTIONS							*/
/*	APAR (umol photon/m2*sec) - total PAR absorbed in day	*/
/*	APAR_coef ( s /m2 / umol photon ) - defines shape	*/
/*			of curve of multiplier			*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Reference: 						*/
/*								*/
/* 	biome bgc						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	leaf_conductance_APAR_curve(	double	APAR,
									double	APAR_coef) 	
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	m_APAR;
	/*--------------------------------------------------------------*/
	/*	Estimate multiplier on stomatal_conductance_max to get	*/
	/*	mean stratum conductance reduction 			*/
	/*	APAR was the only factor.				*/
	/*--------------------------------------------------------------*/
	m_APAR = ( APAR * APAR_coef ) / ( 1.0 + APAR * APAR_coef );
	return(m_APAR);
} /*end leaf_conductance_APAR_curve*/
