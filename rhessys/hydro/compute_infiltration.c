/*--------------------------------------------------------------*/
/* 								*/
/*		compute_infiltration				*/
/*								*/
/*	NAME							*/
/*	compute_infiltration - estimates vertical 		*/
/*		drainage into soil		.		*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_infiltration(				*/
/*				int	,			*/	
/*				double	,			*/
/*				double	,			*/
/*				double	,			*/
/*				double	)			*/
/*								*/
/*	returns:						*/
/*	infiltration - (m water) infiltration 			*/
/*								*/
/*	OPTIONS							*/
/*	int verbose_flag 					*/
/*	double	z - (m) depth to the water table		*/
/*	double S - soil moisture storage			*/
/*	double Ksat_0 - (m/day) sat. hydraulic conductivity	*/
/*				at the surface.			*/
/*	double m_z - decay of conductivity with depth		*/
/*	double p_0 - porosity at surface			*/
/*	double p - porosity decay parameter			*/ 
/*	double precip - incoming precip				*/
/*	double duration - duration				*/
/*      double psi_air_entry - air entry pressure		*/
/*								*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes infiltration based on Phillip approach  	*/
/*			(Philip, 1957)				*/
/*	sorptivity parameter is estimated			*/
/*	from Ksat_0 and air entry pressure following		*/
/*	Manley (1977)						*/
/*	infiltration is dependent on rain fall rate which	*/
/*	is calculated from rainfall duration; note that		*/
/*	if rainfall duration is not input (see zone_daily_F)	*/
/*	it is estimated as daylength; user should be 		*/
/*	aware of the potential for error in using these		*/
/*	average daily rainfall rates				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_infiltration(int verbose_flag,
							 double z,
							 double S,
							 double Ksat_vertical,
							 double Ksat_0,
							 double m_z,
							 double p_0,
							 double p,
							 double precip,
							 double duration,
							 double psi_air_entry)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	double porosity;
	double Ksat;
	double Sp;
	double psi_f;
	double theta;
	double intensity, tp,t;
	double infiltration;
	/*--------------------------------------------------------------*/
	/* only infiltrate for on unsaturated soil			*/
	/*--------------------------------------------------------------*/
	if ((S < 1.0) && (Ksat_0 > ZERO)) {

	/*--------------------------------------------------------------*/
	/*	use mean K and p (porosity) given current saturation    */
	/*	depth							*/
	/*--------------------------------------------------------------*/
	if (m_z > ZERO)
		Ksat = m_z * Ksat_0 *  (1-exp(-z/m_z))/z;
	else
		Ksat = Ksat_0;
	if (p < 999.9)
		porosity = p*p_0*(1-exp(-z/p))/z;
	else
		porosity = p_0;
	/*--------------------------------------------------------------*/
	/*	soil moisture deficit - S must be converted to theta	*/
	/*--------------------------------------------------------------*/
	theta = S*porosity;
	/*--------------------------------------------------------------*/
	/*	estimate sorptivity					*/
	/*--------------------------------------------------------------*/
	psi_f = 0.76 * psi_air_entry;
	Sp = pow(2 * Ksat *  (psi_f),0.5);
	/*--------------------------------------------------------------*/
	/*	calculate rainfall intensity				*/
	/*--------------------------------------------------------------*/
	intensity = precip/duration;
	/*--------------------------------------------------------------*/
	/*	estimate time to ponding				*/
	/*--------------------------------------------------------------*/
	if (intensity > Ksat)
		tp = Ksat *  psi_f * (porosity-theta) / (intensity * (intensity-Ksat));
	else
		tp = duration;
	/*--------------------------------------------------------------*/
	/*	calculate infiltration					*/
	/*--------------------------------------------------------------*/
	t = duration - tp;
	if (duration <= tp)
		infiltration = precip;
	else
		infiltration = Sp * pow(t, 0.5) +Ksat*1.5/3.0 * t + tp * intensity;

	}
	/*--------------------------------------------------------------*/
	/* otherwise soil is saturated 					*/
	/*--------------------------------------------------------------*/
	else 
		infiltration = 0.0;

	if (infiltration > precip)
		infiltration = precip;
	if (infiltration < ZERO)
		infiltration = 0.0;

	/*--------------------------------------------------------------*/
	/* use Ksat_vertical to limit infiltration only to pervious area */
	/*--------------------------------------------------------------*/

	infiltration = infiltration * Ksat_vertical;
	return(infiltration);
} /*compute_infiltration*/
