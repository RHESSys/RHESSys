/*--------------------------------------------------------------*/
/* 								*/
/*		compute_vapour_conductance			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_vapour_conductance - estiamtes diffusivity of 	*/
/*		water vapour at top of soil column		*/
/*								*/
/*	SYNOPSIS						*/
/*	double compute_vapour_conductance(	int		*/
/*						double,		*/
/*						double);	*/
/*								*/
/*	returns:						*/
/*	soil vapour conductance (m water vapour/day)		*/
/*								*/
/*	OPTIONS							*/
/*	int	verbose_flag - comman line control flag		*/
/*	double	unsat_storage - (m water) patch unsat zone water*/
/*	double	sat_deficit - (m water) water eq depth to sat zn*/
/*								*/
/*	DESCRIPTION						*/
/*	Estimates soil diffuse conductance to water vapour 	*/
/*	exfiltration for use in soil potential exfiltration 	*/
/*	equations.  The relationship used in based on an 	*/
/*	empirical relationship between forest floor diffusive	*/
/*	resistance and root zone soil water content in a plot	*/
/*	having a dense douglas fir overstory with sandy loam 	*/
/*	soil.  Application to other sites is at your own risk.	*/
/*	Note that the porosity at the site was 0.205 so we 	*/
/*	also assume that above this porosity the curve is good.	*/
/*	Also, given the possibility of variable depth rooting	*/
/*	zones for different strata ina patch we use the ratio	*/
/*	of unsat storage to sat deficit.  			*/
/*								*/
/*	Reference						*/
/*	Kelliher, F.M., T.A. Black, and D.T. Price, Estimating 	*/
/*	the Effects of Understory Removal From a Douglas Fir	*/
/*	Forest Using a Two-Layer Canopy Evapotranspiration Model*/
/*	Water Resour. Res., 22:13, 1891-1899, 1986.		*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	May 14, 1998 RAF					*/
/*	Original code.						*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_vapour_conductance(      
								   int	verbose_flag,
								   double	unsat_storage,
								   double	sat_deficit,
								   double	active_zone_depth )
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	conductance;
	double	theta;
	/*--------------------------------------------------------------*/
	/*	Determine if sat deficit is at or above surface.	*/
	/*--------------------------------------------------------------*/
	if ( sat_deficit <= 0 ){
		conductance = 99999999;
	}
	else{
		theta = unsat_storage/min(sat_deficit,active_zone_depth);
		if ( theta > 0.185 ){
			conductance = 0.001429;
		}
		else{
			conductance = 1 / ( -83000 * theta + 16100 );
		}
	}
	return(conductance);
} /*compute_vapour_conductance*/
