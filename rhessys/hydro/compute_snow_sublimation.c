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
							double	 T_snow,
							double	vp_air,
							double	snowpack_water_equivalent,
							double  gasnow,
   							double z_obs,
							double	net_radiation,
							double	pa,
						    double *radsubl)
{
	/*--------------------------------------------------------------*/
	/*	Local variable declaration.				*/
	/*--------------------------------------------------------------*/

   double Qe;			/* Latent energy exchange (KJ/m2/s) */
   double Uz;			/* windspeed (m/sec) at height z_obs */ 
   double z_rough=0.005;	/* roughness height (m) from Price & Dunne 1976 */
   double De;			/* Exchange coefficient for latent heat (m/s) */
   double hf;			/* latent heat of fusion for water (KJ/kg) */
   double hv, hs;			/* latent heat of vaporization for water (KJ/kg) */
   double T_air_K;		/* Air temperature at height (K) */
   double T_snow_K;		/* snow surface temperature (K) */
   double vp_sat_ice;		/* saturation vapor pressure over ice (Pa) */
   double ra, delta, psych, sublim, sublim_rad, sublim_turb;
   
   double ro; /* Was set to 2.23... not sure why? Should be density of air in kg/m3 */
   double stabil_func;
   double von_Karman=0.4;

   double sublim_floor_wind;

	T_air_K = T_air + KELVIN;
	T_snow_K = T_snow + KELVIN; /* not used */
	hf = 334;
	hv = 2470;
	stabil_func = 1.0;	/* Assuming neutral stability */
	
	/* Lundberg 1994 reduce conductance for snow vs. rain by factor of 10 */
	gasnow = gasnow / 1.0;
	
	/*	Density of air (ro) as a fn. of air temp.		*/
	ro = 1.292 - ( 0.00428 * T_air );
	
	/* Latent heat of sublimation in KJ/kg */
	hs = (677.0 - 0.07*T_air) * 4.1868;
	
	/* Sat vapor pressure over ice from Magnus Teten (Murray, 1967) */
	vp_sat_ice = 610.78 * exp ((21.875 * (T_air_K - KELVIN)/(T_air_K - 7.66)));
	
	/*vp_sat_ice = 610.78 * exp ((17.269*T_air)/(237.3+T_air));
	 if (T_air < 0.0) vp_sat_ice *= (1.0 + 0.00972*T_air + 0.000042*T_air*T_air);*/
	
	/*Uz = windsp;
	 De = (von_Karman*von_Karman * Uz) / pow(log(z_obs/z_rough),2.0);*/
	De = gasnow; /* not used */
	ra = 1/gasnow;
	Qe = -ro * hs * De * stabil_func * (0.622 * (vp_air - vp_sat_ice)/pa); /* not used */
	
	/***** Lundberg *****/
	delta = 2508.3/pow((T_air+237.3),2) * exp(17.3*T_air/(T_air+237.3)) * 1000.0; /*converted from kPa/C to Pa/C*/
	psych = 1.006 * pa / (0.622 * hs); /*psychrometric constant in Pa/C */
	sublim_rad = (delta * net_radiation) / (1000 * hs * (delta + psych));
	/*sublim_turb = max((86400 * psych * 0.622 * hs * ro * (vp_sat_ice - vp_air) / (pa * ra)) / (1000 * hs * (delta + psych)),0.0);*/
	sublim_turb = (86400 * psych * 0.622 * hs * ro * (vp_sat_ice - vp_air) / (pa * ra)) / (1000 * hs * (delta + psych));
	/***** end Lundberg *****/
	
	/***** Dingman *****/
	/*sublim_rad = 0.0;
	 /*if (T_snow < 0)
	 sublim_turb = (-ro * hs * gasnow * stabil_func * 0.622 * (vp_air - vp_sat_ice) / pa) / (1000 * hs) * 86400; /* phase change, so hs */
	/*else
	 sublim_turb = (-ro * hv * gasnow * stabil_func * 0.622 * (vp_air - vp_sat_ice) / pa) / (1000 * hv) * 86400; /* melting snowpack so no phase change, just hv */
	/***** end Dingman *****/
	
	sublim = max(sublim_rad + sublim_turb, 0.0);
	sublim = min(sublim, snowpack_water_equivalent);
	
	if (sublim_rad > 0)
		*radsubl = sublim_rad / (sublim_rad + sublim_turb);
	else
		*radsubl = 0.0;
	
	if (verbose_flag == -5) {
		printf("\n                                             SUBLIM: De=%lf ro=%lf vp_air=%lf vp_ice=%lf pa=%lf Qe_pot=%lf subl_pot=%lf", 
			   De,ro,vp_air,vp_sat_ice,pa,Qe,Qe*SECONDS_PER_DAY/(hf+hv)/1000.0);
		printf("\n                                             SUBLIM2: netrad=%lf delta=%lf psych=%lf sublim_rad=%lf sublim_turb=%lf sublim=%lf swe=%lf", 
			   net_radiation/86.4, delta, psych, sublim_rad, sublim_turb, sublim, snowpack_water_equivalent);
	}
   
   /* OLD CODE, SUBLIM_FLOOR_WIND NOT USED */
   /*--------------------------------------------------------------*/
   /*	limit sublimation based on available energy		*/
   /*--------------------------------------------------------------*/
   /*net_radiation = 1000000;*/
   /*if ((Qe > 0.0) && (net_radiation >= 0.0))		
      Qe = min(Qe, net_radiation);				
   else
      Qe = 0.0;
   sublim_floor_wind = max(Qe*seconds_per_day/hs/1000.0, 0.0); /* (density of H2O = 1000 kg/m3) */
   /*--------------------------------------------------------------*/
   /*	don't sublimate more snow than available		*/
   /*--------------------------------------------------------------*/
   /*sublim_floor_wind = min (sublim_floor_wind, snowpack_water_equivalent);*/
   /* END OLD CODE */

    return( sublim );
} /*end compute_snow_sublimation*/
