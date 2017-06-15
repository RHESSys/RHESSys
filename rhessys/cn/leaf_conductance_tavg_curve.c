/*--------------------------------------------------------------*/
/*								*/
/*		leaf_conductance_tavg_curve			*/
/*								*/
/*	leaf_conductance_tavg_curve				*/
/*								*/
/*	NAME							*/
/*	leaf_conductance_tavg_curve				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	leaf_conductance_tavg_curve(	double,		*/
/*						double,		*/
/*						double,		*/
/*						double);	*/
/*								*/
/*	returns: 						*/
/*	m_tavg	(DIM)	avg daytime temp multiplier on leaf cond*/
/*								*/
/*	OPTIONS							*/
/*	Tair_mean_day	- daytime mean air temperature (deg C)	*/
/*	topt	- temperature of optimal leaf conductance (degC)*/
/*	tmax	- temp. at which multiplier hits 0 	(deg C)	*/
/*	tcoef 	- coefficient specifying shape of curve		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This function estimates the marginal effect of daytime	*/
/*	average leaf temperature on leaf conductance as per	*/
/*	the bbgc code.  I am not sure of the original source.	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	leaf_conductance_tavg_curve(	double	Tair_mean_day,
									double	topt,
									double	tmax,
									double	tcoef) 	
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	dt;
	double	m_tavg;
	double	temp;
	double	pp;
	/*--------------------------------------------------------------*/
	/*	Specify curve						*/
	/*--------------------------------------------------------------*/
	if ( Tair_mean_day < 0.0 || Tair_mean_day > tmax ){
		m_tavg = 0.0;
	}
	else{
		pp = pow(((tmax-Tair_mean_day)/(tmax - topt)),((tmax - topt)/(topt +10)));
		m_tavg  = (Tair_mean_day +10 ) / (topt + 10 ) * pp;
		dt = tmax - topt;
		temp = exp(log((tmax-Tair_mean_day)/dt) * dt * tcoef);
		m_tavg = temp * exp(tcoef * (Tair_mean_day - topt));
		if ( Tair_mean_day < 5.0 ){
			m_tavg = m_tavg * Tair_mean_day / 5.0;
		}
	} /*end if - else */
	return(m_tavg);
} /*end leaf_conductance_tavg_curve*/
