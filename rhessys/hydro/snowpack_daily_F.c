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
						 struct zone_object *zone,
						 struct patch_object *patch,
						 struct	snowpack_object	*snowpack,
						 double theta_noon,
						 double	T_air,
						 double ea,
						 double gasnow,
						 double pa,
						 double  cloud_fraction,
						 double	rain,
						 double	snow,
						 double	*Kdown_direct_snow,
						 double *Kup_direct_snow,
						 double	*Kdown_diffuse_snow,
						 double *Kup_diffuse_snow,
						 double	*PAR_direct_snow,
						 double	*PAR_diffuse_snow,
						 double  maximum_energy_deficit,
						 double	snow_water_capacity,
						 double	light_ext_coef,
						 double  melt_Tcoef,
						 double overstory_fraction,
						 double area_fraction,
						 int	update_flag)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	double	compute_radiative_fluxes(
		int,
		double	*,
		double *,
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
		double,
		double,
		double *);
	
	double  compute_stability_correction(
										 int ,
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
	double ess_can, B, tau, Qext, KstarH, KstarH2, alb_can, Ldown, Tss, skyview, Lstar_snow_old, Lup_snow;
	double radsubl;
	double dum;
	double start_age, b, fz, r1, r2, r3, fage, albvis, albir;
	double height, surface_age, energy_deficit, water_equivalent_depth;
	double K_reflectance, K_absorptance, PAR_reflectance, PAR_absorptance;
	double Kstar_direct, Kstar_diffuse, APAR_direct, APAR_diffuse;
	double Rnet, sublimation, Q_LE, Q_melt, Q_rain;
	double snow_thresh, rr, aep;
	
	water_equivalent_depth = snowpack[0].water_equivalent_depth;
	surface_age = snowpack[0].surface_age;
	energy_deficit = snowpack[0].energy_deficit;
	
	K_reflectance = 0.0;
	K_absorptance = 0.0;
	PAR_reflectance = 0.0;
	PAR_absorptance = 0.0;
	Kstar_direct = 0.0;
	Kstar_diffuse = 0.0;
	APAR_direct = 0.0;
	APAR_diffuse = 0.0;
	Rnet = 0.0;
	sublimation = 0.0;
	Q_LE = 0.0;
	Q_melt = 0.0;
	Q_rain = 0.0;
	
	snow_thresh = 0.0;
	rr = 0.0;
	aep = 0.0;
	
	
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
	pho_air = (1.292 - ( 0.00428 * T_air )); /* density in kg/m3 and T in C */
	snow_density = 0.1;
	latent_heat_vapour = 2.257e+3;
	latent_heat_melt = 3.35e+2;
	latent_heat_sublimation = latent_heat_vapour + latent_heat_melt;
	patch[0].stability_correction = 1.0;
	dum = 0.0;
	
	if ( verbose_flag == -5 ){
		printf("\nSNOWPACK START: Kdowndirpch=%lf Kdowndifpch=%lf gasnow=%lf SED=%lf alb_flag=%d b=%lf r3=%lf", 
			   *Kdown_direct_snow/86.4, 
			   *Kdown_diffuse_snow/86.4, 
			   gasnow, 
			   energy_deficit,
			   patch[0].soil_defaults[0][0].snow_albedo_flag,
			   patch[0].soil_defaults[0][0].bats_b,
			   patch[0].soil_defaults[0][0].bats_r3);
	}
	
	/* Snow surface temp estimate from Brubaker 1996 as referenced in Dingman */
	/*Tss = min(T_air-2.5,0.0);*/
	Tss = min(zone[0].tdewpoint, 0.0);
	
	/* Reset snow surface age if snowing */
	if ( snow >= 0.005 )
		surface_age = 0.0;
	
	/*patch[0].stability_correction = compute_stability_correction(
						 verbose_flag,
						 0.0,
						 Tss,
						 0.0,
						 2.0,
						 zone[0].metv.tavg,
						 patch[0].windsnow);*/
	
	
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

	/*--------------------------------------------------------------*/
	/* since we are not currently modeling snowpack[0].water_depth */
	/* which is liquid water in the snowpack, this is set to zero */
	/* we leave this as placeholder for future development 		*/
	/*--------------------------------------------------------------*/
	snowpack[0].water_depth = 0.0;

	snowpack_total_water_depth = snowpack[0].water_depth
		+ water_equivalent_depth;
	/*--------------------------------------------------------------*/
	/*	Snowpack height calculated based on relative snow density	*/
	/*																*/
	/*--------------------------------------------------------------*/
	height = water_equivalent_depth / snow_density;
	/*--------------------------------------------------------------*/
	/*	Treat K_reflectance as snowpack albedo						*/
	/* 	albedo changes with snow pack age as given by				*/
	/*		Laramie and Schaake, 1972								*/
	/* 	assume for now K_absorptance is  (1-albedo)    				*/
	/*--------------------------------------------------------------*/
	
	if (patch[0].soil_defaults[0][0].snow_albedo_flag == 2) {
		/* BATS albedo scheme */
		/* see Molotch & Bales 2006 */
		/******************************************************/
		start_age = snowpack[0].surface_age;
		b = patch[0].soil_defaults[0][0].bats_b; /*per BATS, default=2*/
		if (cos(theta_noon) < 0.5)
			fz = 1/b * ( (1+b) / (1 + 2 * b * cos(theta_noon)) - 1.0 );
		else
			fz = 0.0;

		if ( snow >= 0.005 ) {
			surface_age = 0.0;
			fage = 0.0;
			}
		else {
			r1 = exp(5000*(1/273.16 - 1/(Tss+KELVIN))); /*impact of vapor diffusion on snow surface grain growth*/
			r2 = min(pow(r1,10),1.0); /*effect of meltwater refreeze*/
			r3 = patch[0].soil_defaults[0][0].bats_r3; /*effect of dust and soot, default=0.3*/
			surface_age = start_age + 0.0864 * (r1+r2+r3); /* 0.0864 is conversion from 1e-6/s to /d */
			fage = surface_age/(1+surface_age);
			}
		albvis = 0.95 * (1-0.2*fage) + 0.4 * fz * (1-0.95*(1-0.2*fage));
		albir = 0.65 * (1-0.5*fage) + 0.4 * fz * (1-0.65*(1-0.5*fage));
		K_reflectance = 0.5 * (albvis + albir);
		/******************************************************/
		//start_age = snowpack[0].surface_age;
		//b = patch[0].soil_defaults[0][0].bats_b; /*per BATS, default=2*/
		//snow_thresh = 0.005;
		//if (cos(theta_noon) < 0.5)
		//	fz = 1/b * ( (1+b) / (1 + 2 * b * cos(theta_noon)) - 1.0 );
		//else
		//	fz = 0.0;
		//r1 = exp(5000*(1/273.16 - 1/(Tss+KELVIN))); /*impact of vapor diffusion on snow surface grain growth*/
		//r2 = min(pow(r1,10),1.0); /*effect of meltwater refreeze*/
		//r3 = patch[0].soil_defaults[0][0].bats_r3; /*effect of dust and soot, default=0.3*/
		//surface_age = start_age + 0.0864 * (r1+r2+r3);
		//if ( snow > 0.0 ) {
		//	if (snow_thresh > 0.0)
		//		surface_age = max(surface_age*(1-snow/snow_thresh),0.0);
		//	else 
		//		surface_age = 0.0;
		//}
		
		//fage = surface_age/(1+surface_age);
		//albvis = 0.95 * (1-0.2*fage) + 0.4 * fz * (1-0.95*(1-0.2*fage));
		//albir = 0.65 * (1-0.5*fage) + 0.4 * fz * (1-0.65*(1-0.5*fage));
		//K_reflectance = 0.5 * (albvis + albir);
		/* Transition albedo to bare ground value per UEB */
		//aep = 0.1; /* albedo extinction parameter per UEB */
		//if ( height < aep ) {
		//	rr = (1-height/aep)*exp(-1.0*height*0.5/aep);
		//	K_reflectance = rr * patch[0].soil_defaults[0][0].albedo * (1-rr) * K_reflectance;
		//}
		/******************************************************/
		/* end BATS albedo */		
		}
	else {
		/* Army Corps standard age albedo scheme with coefficients from Laramie and Schaake (1972) */
		K_reflectance = 0.85;
		if (surface_age > 0.0) {
			if ( (energy_deficit < 0.0)){
				K_reflectance = 0.85
					* pow(0.94,pow(1.0*surface_age, 0.58));
				}
			else {
				K_reflectance = 0.85
				/*	* pow(0.94,pow(1.0*surface_age, 0.58));*/
				    * pow(0.82,pow(1.0*surface_age, 0.46));
				}
			}
		surface_age += 1;
		/* end age albedo */
		}
	
	K_reflectance = max(K_reflectance,0.4);

	PAR_reflectance = K_reflectance;
	K_absorptance = 1.0 - K_reflectance;
	PAR_absorptance = 1.0 - K_reflectance;
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
	Kstar_direct = compute_radiative_fluxes(
		verbose_flag,
		Kdown_direct_snow,
		Kup_direct_snow,
		total_extinction,
		K_reflectance,
		K_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.2 ",julday(current_date)-2449000);
	APAR_direct = compute_radiative_fluxes(
		verbose_flag,
		PAR_direct_snow,
		&dum,
		total_extinction,
		PAR_reflectance,
		PAR_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.3 ",julday(current_date)-2449000);
	Kstar_diffuse = compute_radiative_fluxes(
		verbose_flag,
		Kdown_diffuse_snow,
		Kup_diffuse_snow,
		total_extinction,
		K_reflectance,
		K_absorptance);
	if( verbose_flag > 2)
		printf("\n%8d -777.4 ",julday(current_date)-2449000);
	APAR_diffuse = compute_radiative_fluxes(
		verbose_flag,
		PAR_diffuse_snow,
		&dum,
		total_extinction,
		PAR_reflectance,
		PAR_absorptance);

	/* ----------------------- */
	/* MOVED TO COMPUTE_LSTAR */
	/* Calculating here just for comparison */
	/* Clear sky emissivity from Satterlund 1979 as applied in Mahat & Tarboten 2012 UEB */
	ess_at = cloud_fraction + (1.0 - cloud_fraction) * 1.08 * (1.0 - exp(-pow(ea/100,(T_air+273)/2016)));
	/* Ldown model from Pomeroy et al 2009 */
	ess_can = 0.98;
	B = 0.023; /*0.023*/
	/*Qext = 0.5; /*veg def param*/
	alb_can = 0.13;
	Qext = 1.081 * ((3.14159/2) - theta_noon) * cos(((3.14159/2) - theta_noon));
	tau = exp(-Qext*patch[0].lai / sin((3.14159/2) - theta_noon));
	KstarH2 = (zone[0].Kdown_direct + zone[0].Kdown_diffuse) * (1 - alb_can - tau * (1-snowpack[0].K_reflectance));
	KstarH = patch[0].Kstar_canopy; /* Using RHESSys canopy absorb estimates */
	skyview = 1.0 - overstory_fraction;
	Ldown = (SBC*86400/1000) * pow((T_air+273), 4.0) * ( skyview * ess_at + (1 - skyview) * ess_can ) + (B * KstarH);
	/*patch[0].Ldown_subcanopy = Ldown;*/
	Lup_snow = ess_snow * ((SBC*86400/1000) * pow((Tss+273), 4.0)) + (1.0 - ess_snow) * Ldown;
	Lstar_snow_old = Ldown - Lup_snow;
	/* ----------------------- */
	
	Rnet = Kstar_direct + Kstar_diffuse + patch[0].Lstar_snow;
		
	if ( verbose_flag == -5 ){
	printf("\nSNOWPACK pre sublim:lai=%lf theta=%lf Qext=%lf tau=%lf Kdownzone=%lf Kdowndirpch=%lf Kdowndifpch=%lf Kstarsnow=%lf \nsnow_refl=%lf age=%lf KstarH=%lf KstarH2=%lf OF=%lf ess_at=%lf Ldown=%lf Tss=%lf Lupsnow=%lf Lstarsnow_old=%lf Lstarsnow=%lf \ndayl=%lf ext=%lf Rnet=%lf",
		   patch[0].lai,
		   theta_noon,
		   Qext,
		   tau,
		   (zone[0].Kdown_direct + zone[0].Kdown_diffuse)/86.4,
		   *Kdown_direct_snow/86.4, 
		   *Kdown_diffuse_snow/86.4,
		   (Kstar_direct + Kstar_diffuse)/86.4,
		   K_reflectance,
		   surface_age,
		   KstarH/86.4,
		   KstarH2/86.4,
		   overstory_fraction,
		   ess_at,
		   Ldown/86.4,
		   Tss,
		   Lup_snow/86.4,
		   Lstar_snow_old/86.4, 
		   patch[0].Lstar_snow/86.4,
		   zone[0].metv.dayl,
		   total_extinction,
		   Rnet/86.4);
	}
	
	if (verbose_flag > 1) {
		printf("\n%4d %4d %4d -777.5 ",current_date.day, current_date.month,
			current_date.year);
		printf("%10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f\n", snowpack[0].Kstar_direct,
			   snowpack[0].Kstar_diffuse, patch[0].Lstar_snow, T_air, snowpack[0].overstory_fraction, 
			   ea, cloud_fraction, ess_at);		
	}


	/*--------------------------------------------------------------*/
	/*	Sublimation						*/
	/* 	sublimation will only occur when Tair < 0;		*/
	/*--------------------------------------------------------------*/
	if (energy_deficit < 0.0) {
		if ( verbose_flag == -5 ){
			printf("\nSNOWPACK pre sublimation");
		}
		sublimation = compute_snow_sublimation (verbose_flag,
										T_air, 
										Tss,
										ea, 
										water_equivalent_depth * area_fraction, 
										gasnow * patch[0].stability_correction, 
										snowpack[0].overstory_height,/*not used*/ 
										Rnet, 
										pa,
										&(radsubl));
		/*Q_radiation_net -= snowpack[0].sublimation * latent_heat_sublimation * pho_water;*/
				
		if ( verbose_flag == -5 ){
			printf("\nSNOWPACK post sublim:T=%lf ea=%lf swe=%lf gasnow=%lf ht=%lf K=%lf L=%lf pa=%lf subl=%lf",
			   T_air,
			   ea,
			   water_equivalent_depth * area_fraction,
			   gasnow,
			   snowpack[0].overstory_height, 
			   (Kstar_direct + Kstar_diffuse)/86.4,
			   patch[0].Lstar_snow/86.4,
			   pa,
			   sublimation);
		}

	}
	
	Q_LE = -1.0 * sublimation * latent_heat_sublimation * pho_water;
	Q_melt = max(Rnet + Q_LE, 0.0);
	
	/*--------------------------------------------------------------*/
	/*  Compute Radiation Melt		                                */
	/*--------------------------------------------------------------*/
	/*if (Q_radiation_net < 0.0) Q_radiation_net = 0.0;*/
	/*if ( (T_air > 0.0) && (snowpack[0].energy_deficit >= 0.0))
		rad_melt = max((Q_radiation_net / pho_water/latent_heat_melt), 0.0);
	else
		rad_melt = max((Q_radiation_net / pho_water / latent_heat_vapour), 0.0);*/
	if (energy_deficit >= 0.0)
		rad_melt = max((Q_melt / pho_water / latent_heat_melt), 0.0);
	else
		rad_melt = 0.0;

	/*--------------------------------------------------------------*/
	/*  Compute Temperature Melt.		                            */
	/*	since this term essentially covers the latent and sensible */
	/* 	heat fluxes from a melting snowpack, we need to make an 	*/
	/*	adjustment to accout for the effect of variation in wind	*/
	/*	speed due to forest cover over the snowpack		*/
	/*	 we assume from Dunne and Leopold (1978) a linear	*/
	/*	reduction in windspped with forest cover		*/
	/*--------------------------------------------------------------*/
	v_factor = (1 - 0.8 *  overstory_fraction);
	/*v_factor = 1.0;*/
	if ((T_air > 0.0) && (energy_deficit >= 0.0))
		T_melt = melt_Tcoef * T_air * v_factor;
	else
		T_melt = 0.0;
	/*--------------------------------------------------------------*/
	/*	Calculate Rain on Snow melting				*/
	/*--------------------------------------------------------------*/
	/* From Dingman p. 199. Using Tss (snow surface) as a proxy for snow temp... OK since rain is interacting with surface layer? */
	/* Assumes rain temp = dewpoint temp and freezing point is 0C */
	if (Tss >= 0) 
		Q_advected_precip =  max(0.0, pho_water * (zone[0].tdewpoint) * (cw * rain)); /* T here is actually (Train - Tfreezepoint) so don't need to convert to K */
	else
		Q_advected_precip =  max(0.0, pho_water * (zone[0].tdewpoint) * (cw * rain) + pho_water * latent_heat_melt * rain);

	if (energy_deficit >= 0.0)
		precip_melt = Q_advected_precip  / pho_water / latent_heat_melt; /* changed from latent heat of vapor */
	else
		precip_melt =  0.0;
	
	Q_rain = Q_advected_precip;
	
	if (verbose_flag > 1) {
		printf("\n%4d %4d %4d -777.6 ",current_date.day, current_date.month,
			current_date.year);
		printf("%10.6f %10.6f %10.6f %10.6f", rad_melt,T_melt, precip_melt,
			snowpack_total_water_depth);
	}
	/*--------------------------------------------------------------*/
	/*  Calculate Total Melt										*/
	/*--------------------------------------------------------------*/
	melt = min((T_melt + rad_melt + precip_melt), 
			   snowpack_total_water_depth * area_fraction);
	
	/*--------------------------------------------------------------*/
	/*  Perform Degree day accumulation								*/
	/*--------------------------------------------------------------*/
	energy_deficit = min(max((energy_deficit+T_air),
						 maximum_energy_deficit),0.001);
	
	
	
	if ( verbose_flag == -5 ){
		printf("\nSNOWPACK end: swe=%lf melt=%lf T_melt=%lf rad_melt=%lf precip_melt=%lf Qmelt=%lf Kupsnow=%lf",
			   snowpack_total_water_depth*area_fraction*1000,
			   melt*1000,
			   T_melt*1000,
			   rad_melt*1000,
			   precip_melt*1000,
			   Q_melt/86.4,
			   (*Kup_direct_snow+*Kup_diffuse_snow)/86.4);
	}
	
			
	/*--------------------------------------------------------------*/
	/*	update snowpack variables					*/
	/*--------------------------------------------------------------*/	
	snowpack[0].K_reflectance += area_fraction * K_reflectance;
	snowpack[0].K_absorptance += area_fraction * K_absorptance;
	snowpack[0].PAR_reflectance += area_fraction * PAR_reflectance;
	snowpack[0].PAR_absorptance += area_fraction * PAR_absorptance;
	snowpack[0].Kstar_direct += area_fraction * Kstar_direct;
	snowpack[0].Kstar_diffuse += area_fraction * Kstar_diffuse;
	snowpack[0].APAR_direct += area_fraction * APAR_direct;
	snowpack[0].APAR_diffuse += area_fraction * APAR_diffuse;
	snowpack[0].Rnet += area_fraction * Rnet;
	snowpack[0].sublimation += area_fraction * sublimation;
	snowpack[0].Q_LE += area_fraction * Q_LE;
	snowpack[0].Q_melt += area_fraction * Q_melt;
	snowpack[0].Q_rain += area_fraction * Q_rain;
	
	/*--------------------------------------------------------------*/
	/*	update snowpack tracking variables if flagged				*/
	/*--------------------------------------------------------------*/		
	if ( update_flag == 1 ) {
		snowpack[0].energy_deficit = energy_deficit;
		snowpack[0].surface_age = surface_age;
		snowpack[0].T = energy_deficit;
	}
	
	
	return(melt);
} /*end snowpack_daily.c*/
