#include "functions.h"


double compute_vapor_pressure_deficit(double saturation_vapor_pressure,
		double dewpoint_vapor_pressure) {
	/*--------------------------------------------------------------*/
	/*	vapor pressure deficit (VPD; units: Pa)						*/
	/*								*/
	/*	Eq. 5.14, p. 110, Jones, "Plants and Microclimate"	*/
	/*	Limited to at least 0.0 as per rhessys C code.		*/
	/*--------------------------------------------------------------*/
	return max(saturation_vapor_pressure - dewpoint_vapor_pressure, 0.0);
}
