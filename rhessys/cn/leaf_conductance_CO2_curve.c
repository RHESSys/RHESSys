/*--------------------------------------------------------------*/
/*																*/
/*		leaf_conductance_CO2_curve								*/
/*																*/
/*	leaf_conductance_CO2_curve									*/
/*																*/
/*	NAME														*/
/*	leaf_conductance_CO2_curve									*/
/*																*/
/*	SYNOPSIS													*/
/*	double	leaf_conductance_CO2_curve(	double,					*/
/*						double, double)							*/
/*																*/
/*	returns: 						*/
/*	m_CO2	CO2 multiplier on leaf conductance								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Reference: 						*/
/*								*/
/* 	insert paper						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	leaf_conductance_CO2_curve(	double CO2, double coef_CO2) 	
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.	
	/*--------------------------------------------------------------*/
	double m_CO2;
	/*--------------------------------------------------------------*/
	/*	Estimate multiplier on stomatal_conductance_max to get	*/
	/*	mean stratum conductance reduction			*/
	/*--------------------------------------------------------------*/
	m_CO2 = (1 - (1 - coef_CO2)*(CO2/350-1));
	m_CO2 = min(1.0, m_CO2);
	m_CO2 = max(0.0, m_CO2);


	return (m_CO2);
} /*end leaf_conductance_CO2_curve*/
