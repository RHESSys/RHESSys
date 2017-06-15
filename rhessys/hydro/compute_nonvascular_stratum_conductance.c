/*--------------------------------------------------------------*/
/* 								*/
/*				compute_nonvascular_stratum_conductance	*/
/*								*/
/*	compute_nonvascular_stratum_conductance - computes conductance to H20	*/
/*				of a single mean nonvascular stratum		*/
/*								*/
/*	NAME							*/
/*	compute_nonvascular_stratum_conductance 				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_nonvascular_stratum_conductance(			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	Based on relationship observed for conductance to water	*/
/*	(%maximum) vs water content (fresh weight/dry weight)	*/
/*	in :							*/
/*								*/
/*	Williams, T.G. and Lawrence B Flanagan, 1996,		*/
/*	"Effect of Changes in Water Content in Photosynthesis,	*/
/*	Transpiration and Discrimination against 13CO2 and	*/
/*	C18O16O in Pleurozium and Spaghnum, Oceologica, Mar 20.	*/
/*								*/
/*	Note that this is for a capacity of 8.78 g wet/g dry	*/
/*	which gave a actual conductance maximum of		*/
/*	1000 mmol/m2/s +/- 250 mmol/m2/s for different trials.	*/
/*	Fields mosses typically have max water contents of	*/
/*	5g/g which corresponds to a measured conductance of 	*/
/*	500 mmol/m2/s by Williams et. al .			*/
/*								*/
/*	The data for Williams et. al. does not extend below	*/
/*	2.5g/g moisture content.  Fernandes has fitted a	*/
/*	quadratic equation to the linear relationship of	*/
/*	Williams et. al. which is forced to 0 conductance	*/
/*	at 0 moisture content.					*/
/*								*/
/*	Finally, to take account for the fact that moss 	*/ 
/*	evaporates for a whole day and that the rain stored	*/
/*	term includes precip during the day and ignores		*/
/*	during day drying we take the weighted mean of the	*/
/*	rain stored in the moss and  				*/
/*	max(0,stratum[0].rain_stored - 1.0 )			*/
/*								*/
/*	It is assumed that litter have conductances that	*/
/*	can be similarily parameterized.					*/
/*								*/
/*	PROGRAMMERS NOTES									*/
/*														*/
/*	Sep 2 1997 RAF										*/
/*														*/
/*	New code taken from old live_moss_daily_F.c			*/
/*								*/
/*	Sep 3 1997						*/
/*	Made sure stratum conductance is non zero.		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_nonvascular_stratum_conductance(
												int verbose_flag,
												double	rain_stored,
												double	rain_capacity,
												double	gl_c,
												double	gsurf_slope,
												double	gsurf_intercept)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double  approx_day_end_storage;
	double	adjusted_storage;
	double	stratum_conductance;
	/*--------------------------------------------------------------*/
	/*		Use a forward prediction of the end of day storage		*/
	/*		by assuming a 1mm daytime evaporation from the stratum.	*/
	/*--------------------------------------------------------------*/
	approx_day_end_storage = max(0,rain_stored - 0.001 );
	if (rain_capacity <  0.00000000000001)
		adjusted_storage = 0.0;
	else
		adjusted_storage = max(0,((rain_stored + approx_day_end_storage )/2.0)
		/ ( rain_capacity));
	/*--------------------------------------------------------------*/
	/*		Use linear fit if storage is > 0						*/
	/*--------------------------------------------------------------*/
	stratum_conductance = max(gsurf_slope*adjusted_storage + gsurf_intercept ,0);
	return(stratum_conductance);
} /*end compute_nonvascular_stratum_conductance*/
