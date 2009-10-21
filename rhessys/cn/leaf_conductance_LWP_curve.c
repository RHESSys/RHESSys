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
/*	double	leaf_conductance_LWP_curve(	double,		*/
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

double	leaf_conductance_LWP_curve(	double	LWP_predawn,
								   double	LWP_min_spring,
								   double	LWP_stom_closure) 	
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	m_LWP;
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
		if (LWP_min_spring == LWP_stom_closure)
			m_LWP = 1.0;
		else
			m_LWP = (LWP_predawn - LWP_stom_closure)
			/ (LWP_min_spring - LWP_stom_closure);
	}

	return(m_LWP);
} /*end leaf_conductance_LWP_curve*/
