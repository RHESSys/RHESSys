/*--------------------------------------------------------------*/
/*								*/
/*		leaf_conductance_tmin_curve			*/
/*								*/
/*	leaf_conductance_tmin_curve				*/
/*								*/
/*	NAME							*/
/*	leaf_conductance_tmin_curve				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	leaf_conductance_tmin_curve(	double )	*/
/*								*/
/*	returns: 						*/
/*	m_tmin	(DIM)	 tmin multiplier on leaf cond		*/
/*								*/
/*	OPTIONS							*/
/*	Tair_min (deg C) - min 24hr air temperature		*/ 
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Estimates marginal effect of daily minimum air temp. on	*/
/*	leaf conductance.  Arbitrarily sets conductance to 0	*/
/*	at -8 degress C.					*/
/*								*/
/*	Reference: Running and Coughlan, ...			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	leaf_conductance_tmin_curve(	double	Tair_min)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	m_tmin;
	/*--------------------------------------------------------------*/
	/*	Specify curve						*/
	/*--------------------------------------------------------------*/
	if ( Tair_min > 0.0 ){
		m_tmin = 1.0;
	}
	else if ( Tair_min < -8.0 ){
		m_tmin = 0.0;
	}
	else{
		m_tmin = 1.0 + ( 0.125 * Tair_min );
	}
	return(m_tmin);
} /*end leaf_conductance_tmin_curve*/
