/*--------------------------------------------------------------*/
/* 																*/
/*				snowpack_daily_F.c								*/
/*																*/
/*	NAME														*/
/*	snowpack_daily_F - performs snowpack cycling for a day		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	This routine calculates daily snowmelt based on 			*/
/*	a degree day method and simple estimates of 				*/
/*	temperature and radiation driven melt						*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	May 1, 1997	RAF	*/
/*	Made sure snow height was actually updates at end of routine.	*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

double	snowpack_daily_F(
						 struct  date   current_date,
						 int	verbose_flag,
						 struct	snowpack_object	*snowpack,
						 double	T_air,
						 double ea,
						 double windspeed,
						 double pa,
						 double  cloud_fraction,
						 double	rain,
						 double	snow,
						 double	*Kdown_direct_snow,
						 double	*Kdown_diffuse_snow,
						 double	*PAR_direct_snow,
						 double	*PAR_diffuse_snow,
						 double  maximum_energy_deficit,
						 double	snow_water_capacity,
						 double	light_ext_coef,
						 double  melt_Tcoef,
						 double	Lstar_snow)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	double	compute_radiative_fluxes(
		int,
		double	*,
		double,
		double,
		double);

	double  compute_snow_sublimation(int, 
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	long julday(struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double 	cp,cs, cw,pho_water, pho_air;
	double	cs_slope;
	double 	latent_heat_vapour, latent_heat_melt;
	double	latent_heat_sublimation;
	double	snow_density;
	double Q_advected_precip;
	double Q_radiation_net;
	double	rad_melt, melt, T_melt, precip_melt;
	double	snowpack_total_water_depth;
	double	total_extinction;
	double v_factor, ess_at;
	double	optical_depth;

	/*--------------------------------------------------------------*/
	/*  Fix heat capacity and density for air and water,ice for now */
	/*																*/
	/* 	many of these should actually vary with temp				*/
	/*	cp air is 1.01	kJ/kg.K										*/
	/*	cw  water 0.004180 kJ/kg.K									*/
	/* 	density of water 1.0e+3 kg/m -3 							*/
	/*	density of air	1.29 kg/m -3								*/
	/* 																*/
	/*	relative density of new snow	0.1 		 				*/
	/*	- note this is often highly variable						*/
	/*		taken from Dingman,1994									*/
	/*																*/
	/*  latent heat vapourization is 2.495e+3 kJ/kg					*/
	/*  latent heat of melt is 3.34e+2 kJ/kg						*/
	/*--------------------------------------------------------------*/
	cp = 1.01;
	cw = 4.216;
	pho_water = 1.0e+3;
	pho_air = 1.29;
	snow_density = 0.1;
	latent_heat_vapour = 2.257e+3;
	latent_heat_melt = 3.35e+2;
	latent_heat_sublimation = latent_heat_vapour + latent_heat_melt;
	/*--------------------------------------------------------------*/
	/* 	From Anderson, 1976	 specific heat of ice is 				*/
	/*	units of kJ/kg C											*/
	/*--------------------------------------------------------------*/
	cs_slope = 0.00737;
	cs = 2.106 + 0.00737 * snowpack[0].T;
	/*--------------------------------------------------------------*/
	/*	Default snowpack optical characteristics for now.			*/
	/*	Ideally these would change daily.							*/
	/*	I have no idea about what the optical depth should be here	*/
	/*--------------------------------------------------------------*/
	optical_depth =  1.0;
	snowpack_total_water_depth = snowpack[0].water_depth
		+ snowpack[0].water_equivalent_depth;
	/*--------------------------------------------------------------*/
	/*	Snowpack height calculated based on relative snow density	*/
	/*																*/
	/*--------------------------------------------------------------*/
	snowpack[0].height = snowpack[0].water_equivalent_depth / snow_density;
	/*--------------------------------------------------------------*/
	/*	Treat K_reflectance as snowpack albedo						*/
	/* 	albedo changes with snow pack age as given by				*/
	/*		Laramie and Schaake, 1972								*/
	/* 	assume for now K_absorptance is  (1-albedo)    				*/
	/*--------------------------------------------------------------*/
	snowpack[0].K_reflectance = 0.85;
	if (snowpack[0].surface_age > 0.0) {
		if ( (snowpack[0].energy_deficit < 0.0)){
			snowpack[0].K_reflectance = 0.85
				* pow(0.94,pow(1.0*snowpack[0].surface_age, 0.58));
		}
		else{
			snowpack[0].K_reflectance = 0.85
				* pow(0.82,pow(1.0*snowpack[0].surface_age, 0.46));
		}
	}
	snowpack[0].PAR_reflectance = snowpack[0].K_reflectance;
	snowpack[0].K_absorptance = 1.0 - snowpack[0].K_reflectance;
	snowpack[0].PAR_absorptance = 1.0 - snowpack[0].K_reflectance;
	/*--------------------------------------------------------------*/
	/*	Syntheisze snowpack level total extincition coeff.	*/
	/*--------------------------------------------------------------*/
	total_extinction =  (light_ext_coef * optical_depth );
	if( verbose_flag > 1) {
		printf("\n%8d -777.0 ",current_date.day);
		printf("%8.2f %8.2f %8.2f %8.2f %8.2f ",
			snowpack[0].K_reflectance, *Kdown_direct_snow,*Kdown_diffuse_snow,
			*PAR_direct_snow,*PAR_diffuse_snow);
	}
	/*--------------------------------------------------------------*/
	/*  Intercept direct radiation.                                 */
	/*--------------------------------------------------------------*/
	if( verbose_flag > 2)
		printf("\n%8d -777.1 ",julday(current_date)-2449000);
	snowpack[0].Kstar_direct = compute_radiative_fluxes(
		verbose_flag,
		Kdown_direct_snow,
		total_extinction,
		snowpack[0].K_reflectance,
		snowpack[0].K_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.2 ",julday(current_date)-2449000);
	snowpack[0].APAR_direct = compute_radiative_fluxes(
		verbose_flag,
		PAR_direct_snow,
		total_extinction,
		snowpack[0].PAR_reflectance,
		snowpack[0].PAR_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.3 ",julday(current_date)-2449000);
	snowpack[0].Kstar_diffuse = compute_radiative_fluxes(
		verbose_flag,
		Kdown_diffuse_snow,
		total_extinction,
		snowpack[0].K_reflectance,
		snowpack[0].K_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.4 ",julday(current_date)-2449000);
	snowpack[0].APAR_diffuse = compute_radiative_fluxes(
		verbose_flag,
		PAR_diffuse_snow,
		total_extinction,
		snowpack[0].PAR_reflectance,
		snowpack[0].PAR_absorptance);
	/*--------------------------------------------------------------*/
	/*	Compute Lstar snow					*/
	/*								*/
	/*	net Lstar into snowpack is estimate assuming		*/
	/* 	temperature of snowpack = air temperature		*/
	/*	until the snowpack is ripe at which time		*/
	/*	temperature of snowpack is 0.0				*/
	/*	combine emissivity of atmosphere and canopy above	*/
	/*	the snowpack is calculated as in:			*/
	/*								*/
	/*	Croley, T.E, II, 1989, 					*/
	/*	Verifiable Evaporation modelling on the Laurentian	*/
	/*	Great Lakes, Water Resources Research, 25:781-792	*/
	/*	which is similar to US Army Corp of Engineers		*/
	/*	calculation                                             */
	/*--------------------------------------------------------------*/
	ess_at = ( 1 - snowpack[0].overstory_fraction) * (0.53 + 0.065
		* pow(ea / 100.0, 0.5) ) * ( 1 + 0.4 * cloud_fraction)
		+ snowpack[0].overstory_fraction;
	if ((T_air > 0.0) && (snowpack[0].energy_deficit >= 0.0))
		Lstar_snow =  41.868 * ( (ess_at) * SBC * pow((T_air+273), 4.0) - 663);
	else
		Lstar_snow =  41.868 * (ess_at - 1.0) * SBC * pow((T_air+273), 4.0);
	Q_radiation_net = snowpack[0].Kstar_direct
		+ snowpack[0].Kstar_diffuse +	Lstar_snow;
	if (verbose_flag > 1) {
		printf("\n%4d %4d %4d -777.5 ",current_date.day, current_date.month,
			current_date.year);
		printf("%10.3f %10.3f %10.3f %10.3f", snowpack[0].Kstar_direct,
			snowpack[0].Kstar_diffuse, Lstar_snow, T_air);
	}

	/*--------------------------------------------------------------*/
	/*	Sublimation						*/
	/* 	sublimation will only occur when Tair < 0;		*/
	/*--------------------------------------------------------------*/
	if (T_air < 0.0) {
		snowpack[0].sublimation = compute_snow_sublimation (verbose_flag,
			T_air, ea, snowpack[0].water_equivalent_depth,
			windspeed, snowpack[0].overstory_height, Q_radiation_net,pa);
		Q_radiation_net -= snowpack[0].sublimation * latent_heat_sublimation;
	}
	/*--------------------------------------------------------------*/
	/*  Perform Degree day accumulation								*/
	/*--------------------------------------------------------------*/
	snowpack[0].energy_deficit = max((snowpack[0].energy_deficit+T_air),
		maximum_energy_deficit);
	/*--------------------------------------------------------------*/
	/*  Compute Radiation Melt		                                */
	/*--------------------------------------------------------------*/
	if (Q_radiation_net < 0.0) Q_radiation_net = 0.0;
	if ((T_air > 0.0) && (snowpack[0].energy_deficit >= 0.0))
		rad_melt = max((Q_radiation_net / pho_water/latent_heat_melt), 0.0);
	else
		rad_melt = max((Q_radiation_net / pho_water / latent_heat_vapour), 0.0);
	/*--------------------------------------------------------------*/
	/*  Compute Temperature Melt.		                            */
	/*	since this term essentially covers the latent and sensible */
	/* 	heat fluxes from a melting snowpack, we need to make an 	*/
	/*	adjustment to accout for the effect of variation in wind	*/
	/*	speed due to forest cover over the snowpack		*/
	/*	 we assume from Dunne and Leopold (1978) a linear	*/
	/*	reduction in windspped with forest cover		*/
	/*--------------------------------------------------------------*/
	v_factor = (1 - 0.8 *  snowpack[0].overstory_fraction);
	if ((T_air > 0.0) && (snowpack[0].energy_deficit >= 0.0))
		T_melt = melt_Tcoef * T_air * v_factor;
	else
		T_melt = 0.0;
	/*--------------------------------------------------------------*/
	/*	Calculate Rain on Snow melting				*/
	/*--------------------------------------------------------------*/
	Q_advected_precip =  pho_water * (T_air ) * (cw * rain );
	if ((T_air > 0.0) && (snowpack[0].energy_deficit >= 0.0))
		precip_melt = Q_advected_precip  / pho_water / latent_heat_vapour;
	else
		precip_melt =  0.0;
	if (verbose_flag > 1) {
		printf("\n%4d %4d %4d -777.6 ",current_date.day, current_date.month,
			current_date.year);
		printf("%10.6f %10.6f %10.6f %10.6f", rad_melt,T_melt, precip_melt,
			snowpack_total_water_depth);
	}
	/*--------------------------------------------------------------*/
	/*  Calculate Total Melt										*/
	/*--------------------------------------------------------------*/
	if ((T_melt + rad_melt + precip_melt) > snowpack_total_water_depth)
		melt = snowpack_total_water_depth;
	else
		melt = T_melt + rad_melt + precip_melt;
	/*--------------------------------------------------------------*/
	/*	Age the surface of the snowpack								*/
	/*--------------------------------------------------------------*/
	snowpack[0].surface_age += 1;
	/*--------------------------------------------------------------*/
	/*	Remove melt from snowpack									*/
	/*--------------------------------------------------------------*/
	snowpack[0].T = 0.0;
	snowpack[0].water_equivalent_depth -= melt;
	snowpack[0].water_depth = 0.0;

	if (snowpack[0].sublimation > snowpack[0].water_equivalent_depth) 
		snowpack[0].sublimation = snowpack[0].water_equivalent_depth;
	/*--------------------------------------------------------------*/
	/*	update snowpack height.					*/
	/*--------------------------------------------------------------*/
	snowpack[0].height = snowpack[0].water_equivalent_depth / snow_density;
	return(melt);
} /*end snowpack_daily.c*/
