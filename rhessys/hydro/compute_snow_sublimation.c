/*--------------------------------------------------------------*/
/* 								*/
/*		compute_snow_sublimation				*/
/*								*/
/*	NAME							*/
/*	compute_snow_sublimation - 				*/
/*		computes snow lost to sublmation		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_snow_sublimation( int verbose,			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	RETURNS:						*/
/*	snow_sublmated (m/day) - snow sublimated		*/
/*								*/
/*	DESCRIPTION						*/
/*	computes snow sublimated based on vpd; 			*/
/*	assuming that snow temperature at surface is		*/
/*	air temperature						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	original code obtained from Melannie Hartmann, NREL	*/
/*	adapted to include an radiation constraint, on 		*/
/*	amount lost						*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_snow_sublimation(
							int	verbose_flag,
							double	T_air,
							double	vp_air,
							double	snowpack_water_equivalent,
							double  windsp,
   							double z_obs,
							double	net_radiation,
							double	pa)
{
	/*--------------------------------------------------------------*/
	/*	Local variable declaration.				*/
	/*--------------------------------------------------------------*/

   double Qe;			/* Latent energy exchange (J/sec/m2) or (W/m2) */
   double Uz;			/* windspeed (m/sec) at height z_obs */ 
   double z_rough=0.005;	/* roughness height (m) */
   double De;			/* Exchange coefficient for latent heat (units?) */
   double hf;			/* latent heat of fusion for water (J/mm/m2) or (KJ/m/m2) */
   double hv;			/* latent heat of vaporization for water (J/mm/m2) or (KJ/m/m2) */
   double T_air_K;		/* Air temperature at height (K) */
   double T_snow_K;		/* snow surface temperature (K) */
   double vp_sat_ice;		/* saturation vapor pressure over ice (Pa) */
   
   double ro=2.23;
   double stabil_func;
   double H=8000;		/* Scale height of atmosphere (m) */
   double mb2Pa=100;		/* millibar to Pascal conversion, 1mb = 100 Pa */
   double von_Karman=0.4;
   

   double melt;
   double sublim_floor_wind;

   T_air_K = T_air + KELVIN;
   T_snow_K = T_air_K;
   hf = 3.34 * 1e5;
   hv = 2.50 * 1e6;
   stabil_func = 1.0;	/* Assuming neutral stability */

   vp_sat_ice = 610.78 * exp ((21.875 * (T_air_K - KELVIN)/(T_air_K - 7.66)));

   Uz = windsp;
   De = (von_Karman*von_Karman * Uz) / pow(log(z_obs/z_rough),2.0);
   Qe = ro * (hf+hv) * De * stabil_func * (0.622 * (vp_air - vp_sat_ice)/pa);
	/*--------------------------------------------------------------*/
	/*	limit sublimation based on available energy		*/
	/*--------------------------------------------------------------*/
   if ((Qe < 0.0) && (net_radiation >= 0.0))		
      Qe = min(-Qe, net_radiation);				
   else
      Qe = 0.0;

   sublim_floor_wind = max(Qe*seconds_per_day/(hf+hv), 0.0);
   sublim_floor_wind /= 1000;		/* convert from mm to meters */
   
	/*--------------------------------------------------------------*/
	/*	don't sublimate more snow than available		*/
	/*--------------------------------------------------------------*/
   sublim_floor_wind = min (sublim_floor_wind, snowpack_water_equivalent);

    return( sublim_floor_wind);
} /*end compute_snow_sublimation*/
