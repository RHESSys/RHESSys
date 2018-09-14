/*--------------------------------------------------------------*/
/*								*/
/*		leaf_conductance_vpd_curve			*/
/*								*/
/*	leaf_conductance_vpd_curve				*/
/*								*/
/*	NAME							*/
/*	leaf_conductance_vpd_curve				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	leaf_conductance_vpd_curve(	double )	*/
/*								*/
/*	returns: 						*/
/*	m_vpd	(DIM)	 vpd multiplier on leaf cond		*/
/*								*/
/*	OPTIONS							*/
/*	vpd	(Pa)	- daytime mean	vpd			*/
/*	vpd_open (Pa)	- vpd at which multiplier is 1		*/
/*	vpd_closed (Pa)	- vpd at which multiplier is 0		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Estimates marginal effect of vpd on leaf conductance.	*/
/*	Note that this effect assumes that the mean vpd is	*/
/*	sufficient to represent the daytime vpd and that there	*/
/*	is a linear response of stomatal conductance vs vpd.	*/
/*								*/
/*	Refernce: bbgc						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
double	leaf_conductance_vpd_curve(	double	vpd,
								   double	vpd_open,
								   double	vpd_closed)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	m_vpd;
	/*--------------------------------------------------------------*/
	/*	Specify curve						*/
	/*--------------------------------------------------------------*/
	if ( vpd < vpd_open ){
		m_vpd = 1.0;
	}
	else if ( vpd > vpd_closed ){
		m_vpd = 0.0;
	}
	else{
		if (vpd_closed == vpd_open)
			m_vpd = 0;
		else
			m_vpd = ( vpd_closed - vpd ) / ( vpd_closed - vpd_open);
	}
	return(m_vpd);
} /*end leaf_conductance_vpd_curve*/
