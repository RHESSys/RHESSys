/*--------------------------------------------------------------*/
/*								*/
/*		leaf_conductance_LWP_curve			*/
/*								*/
/*	leaf_conductance_LWP_curve				*/
/*								*/
/*	NAME							*/
/*	leaf_conductance_LWP_curve				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	leaf_conductance_LWP_curve( int	double,		*/
/*						double,		*/
/*						double,		*/
/*						double,		*/
/*						double,		*/
/*						double);	*/
/*								*/
/*	returns: 						*/
/*	m_LWP	(DIM)	predawn LWP multiplier on leaf cond	*/
/*								*/
/*	OPTIONS							*/
/*	LWP_predawn (-Mpa) - predawn leaf water potential	*/
/*	LWP_min_spring (-Mpa) - minimum spring leaf water pot.	*/
/*	LWP_stom_closure (-Mpa) - lwp for stomatal closure	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This function estimates the marginal effect of 		*/
/*	predawn_LWP on stomatal conductance.  Note that the LWP	*/
/*	should be estimated for the morning of this day.	*/
/*								*/
/*	Reference: Running and Coughlan, ...			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	leaf_conductance_LWP_curve(int curve,
				double	LWP_predawn,
			        double	LWP_min_spring,
			        double	LWP_stom_closure,
				double  LWP_threshold,
				double  slp,
				double  intercpt) 	
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	m_LWP, estimate;
	/*--------------------------------------------------------------*/
	/*	Specify curve						*/
	/*--------------------------------------------------------------*/

	if ( LWP_predawn > LWP_min_spring ){
		m_LWP = 1.0;
	}
	else if ( LWP_predawn < LWP_stom_closure ){
		m_LWP = 0.0;
	}
	
	else{

	 if (curve == 0) {  /* original Biome BGC approach */ 
		if (LWP_min_spring == LWP_stom_closure)
			m_LWP = 1.0;
		else
			m_LWP = (LWP_predawn - LWP_stom_closure)
			/ (LWP_min_spring - LWP_stom_closure);
		}
	else {
		if (LWP_predawn < LWP_threshold)	{
			estimate = slp*(LWP_predawn-LWP_threshold)+intercpt;
			m_LWP = pow(estimate,curve);
			m_LWP = max(m_LWP,0);
			m_LWP = min(m_LWP, 1);
		}
		else m_LWP = 1;
	}
	}

	return(m_LWP);
} /*end leaf_conductance_LWP_curve*/
