/*--------------------------------------------------------------*/
/* 								*/
/*		compute_vapor_pressure_deficit				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_vapor_pressure_deficit 	*/
/*			*/
/*								*/
/*	SYNOPSIS						*/
/*	double compute_vapor_pressure_deficit(double saturation_vapor_pressure, */
/*		double dewpoint_vapor_pressure)					*/
/*								*/
/*	returns:						*/
/*	vapor pressure deficit (Pa)	*/
/*								*/
/*	OPTIONS							*/
/*	double saturation_vapor_pressure (Pa),
	double dewpoint_vapor_pressure (Pa)
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Vapour Pressure	Deficit (Pa)							*/
/*																*/
/*--------------------------------------------------------------*/
/*	vapor pressure deficit (VPD; units: Pa)						*/
/*								*/
/*	Eq. 5.14, p. 110, Jones, "Plants and Microclimate"	*/
/*	Limited to at least 0.0 as per rhessys C code.		*/
/*--------------------------------------------------------------*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*--------------------------------------------------------------*/
#include "rhessys.h"


double compute_vapor_pressure_deficit(double saturation_vapor_pressure,
		double dewpoint_vapor_pressure) {
	return max(saturation_vapor_pressure - dewpoint_vapor_pressure, 0.0);
}
