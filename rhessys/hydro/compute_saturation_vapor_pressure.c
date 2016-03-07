/*--------------------------------------------------------------*/
/* 								*/
/*		compute_saturation_vapor_pressure				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_saturation_vapor_pressure 	*/
/*			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_saturation_vapor_pressure(				*/
/*			double temperature)					*/
/*								*/
/*	returns:						*/
/*	vapor pressure at saturation (Pa)	*/
/*								*/
/*	OPTIONS							*/
/*	double temperature (Celsius)
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Saturation Vapour Pressure	(Pa)							*/
/*																*/
/*	Note the original rehssys code supplied es in mbar.			*/
/*	c.f. eq. 1 Running and Coughlan , 1987, p. 133.				*/
/*																*/
/*	Since 1 bar = 100 kpa (approx) ; a millibar = 100 Pa approx.*/
/*	This explains why the es from the original code was:		*/
/*																*/
/*	6.1078 * exp((17.269*z[0].metv.tday)/(237.3 +			*/
/*									z[0].metv.tday))		*/
/*																*/
/*	Which is approx 100 times that of the es here.				*/
/*																*/
/*	Eq. 5.12 p. 110, Jones, "Plants and Microclimate"			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*--------------------------------------------------------------*/
double compute_saturation_vapor_pressure(double temperature) {
	double es = 613.75 * exp( (17.502 * temperature)
			/ ( 240.97 + temperature) );
	return es;
}

