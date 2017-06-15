/*--------------------------------------------------------------*/
/* 																*/
/*		 		compute_Lstar						*/
/*																*/
/*	compute_Lstar - updates longwave balances in a patch			*/
/*																*/
/*	NAME														*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

#include "phys_constants.h"
#include "rhessys.h"


void compute_Lstar_canopy(int	verbose_flag,
						  double KstarH,
						  double snow_stored,
						  struct	zone_object	*zone,
						  struct	patch_object	*patch,
						  struct	canopy_strata_object *stratum)
{
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	daylength;
	double	nightlength;
	double Lup_canopy, Ldown_canopy;
	double Lup_snow, Lup_soil, Lup_pond;
	double Tss, Tpond, Tcan, Tsoil, ess_can, B, skyview;
	double Lstar, Ldownini;
	
	
	/* Zero out fluxes */
	Lup_canopy = 0.0;
	Ldown_canopy = 0.0;
	Lup_snow = 0.0;
	Lup_soil = 0.0;
	Lup_pond = 0.0;
	Lstar = 0.0;
	B = 0.023;
	
	double Lup_canopy_night = 0.0;
	double Lup_canopy_day = 0.0;
	double Ldown_canopy_night = 0.0;
	double Ldown_canopy_day = 0.0;
	double Lup_soil_night = 0.0;
	double Lup_soil_day = 0.0;
	double Lup_snow_night = 0.0;
	double Lup_snow_day = 0.0;
	double Lup_pond_night = 0.0;
	double Lup_pond_day = 0.0;

	double Lstar_night = 0.0;
	double Lstar_day = 0.0;

	/*--------------------------------------------------------------*/
	/*	Use daylength for longwave computations.	*/
	/*--------------------------------------------------------------*/
	daylength = zone[0].metv.dayl;
	nightlength = SECONDS_PER_DAY - daylength;
	
	/* Snow surface temp estimate from Brubaker 1996 as referenced in Dingman */
	/*Tss = patch[0].snowpack.T;*/
	/*Tss = min(zone[0].metv.tavg-2.5,0.0);*/
	Tss = min(zone[0].tdewpoint,0.0);
	
	/* Surface temperatures assumed same as air temperature */
	Tpond = zone[0].metv.tavg;
	Tsoil = zone[0].metv.tavg;
	if (snow_stored > ZERO)
		Tcan = min(zone[0].metv.tavg,0.0);
	else
		Tcan = zone[0].metv.tavg;

	
	Ldownini = patch[0].Ldown;
	
	// Tcan was using arithmatic average air temperature (tavg), which differs from the daytime average
	//   air temperature (tday), which is used in compute_Lstar().  Now using daytime (tday) and
	//   nighttime (tnight) average air temperature to be consistent with compute_Lstar().
	Ldown_canopy_night = ess_veg * SBC * pow( zone[0].metv.tnight + 273.0, 4.0 ) * nightlength / 1000.0;
	Lup_canopy_night = Ldown_canopy_night + (1 - ess_veg) * patch[0].Ldown_night;
	Ldown_canopy_day = ess_veg * SBC * pow( zone[0].metv.tday +	273.0, 4.0 ) * daylength / 1000.0;
	Lup_canopy_day = Ldown_canopy_day + (1 - ess_veg) * patch[0].Ldown_day;
	Ldown_canopy = Ldown_canopy_day  + Ldown_canopy_night;
	Lup_canopy = Lup_canopy_day + Lup_canopy_night;
	
	if ( patch[0].snowpack.water_equivalent_depth > 0 ) {
		/* snow case */
		Lup_snow_night = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * nightlength / 1000.0
				+ (1 - ess_snow) * Ldown_canopy_night;
		Lup_snow_day = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * daylength / 1000.0
				+ (1 - ess_snow) * Ldown_canopy_day;
		Lup_snow = Lup_snow_night + Lup_snow_day;

		/* ignoring reflected L between snow and soil surfaces */
		Lup_soil_night = Lup_soil_day = Lup_soil = 0.0;
	}
	else if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
		/* ponded water case */
		Lup_pond_night = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * nightlength / 1000.0
				+ (1 - ess_water) * Ldown_canopy_night;
		Lup_pond_day = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * daylength / 1000.0
				+ (1 - ess_water) * Ldown_canopy_day;
		Lup_pond = Lup_pond_night + Lup_pond_day;

		/* ignoring reflected L between water and soil surfaces */
		Lup_soil_night = Lup_soil_day = Lup_soil = 0.0;
	} else {
		/* bare soil case */
		Lup_snow_night = Lup_snow_day = Lup_snow = 0.0;

		Lup_soil_night = ess_soil * SBC * pow( Tsoil + 273.0 , 4.0) * nightlength / 1000.0
				+ (1 - ess_soil) * Ldown_canopy_night;
		Lup_soil_day = ess_soil * SBC * pow( Tsoil + 273.0 , 4.0) * daylength / 1000.0
				+ (1 - ess_soil) * Ldown_canopy_day;
		Lup_soil = Lup_soil_night + Lup_soil_day;
	}

		
	/*--------------------------------------------------------------*/
	/*	Compute daily Lstars.										*/
	/*																*/
	/*	We consider the canopy as a homogenous slab with temp		*/
	/*	equal to the air temp of the canopy and do not distinguish	*/
	/*	between strata.												*/
	/*--------------------------------------------------------------*/
	if ( patch[0].snowpack.water_equivalent_depth > 0 ){
		/*--------------------------------------------------------------*/
		/*	Compute with a snow layer									*/
		/*--------------------------------------------------------------*/
		Lstar = Lup_snow + patch[0].Ldown - Lup_canopy - Ldown_canopy;
		Lstar_night = Lup_snow_night + patch[0].Ldown_night - Lup_canopy_night - Ldown_canopy_night;
		Lstar_day = Lup_snow_day + patch[0].Ldown_day - Lup_canopy_day - Ldown_canopy_day;
	}
	else if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
		/*--------------------------------------------------------------*/
		/*	Compute with ponded water									*/
		/*--------------------------------------------------------------*/
		Lstar = Lup_pond + patch[0].Ldown - Lup_canopy - Ldown_canopy;
		Lstar_night = Lup_pond_night + patch[0].Ldown_night - Lup_canopy_night - Ldown_canopy_night;
		Lstar_day = Lup_pond_day + patch[0].Ldown_day - Lup_canopy_day - Ldown_canopy_day;
	} else {
		/*--------------------------------------------------------------*/
		/*	Compute with bare soil										*/
		/*--------------------------------------------------------------*/
		Lstar = Lup_soil + patch[0].Ldown - Lup_canopy - Ldown_canopy;
		Lstar_night = Lup_soil_night + patch[0].Ldown_night - Lup_canopy_night - Ldown_canopy_night;
		Lstar_day = Lup_soil_day + patch[0].Ldown_day - Lup_canopy_day - Ldown_canopy_day;
	}

	patch[0].Ldown = Ldown_canopy;
	patch[0].Ldown_night = Ldown_canopy_night;
	patch[0].Ldown_day = Ldown_canopy_day;
	
	if ( verbose_flag == -5 ){
		printf("\n     LSTAR CAN: SWE=%lf ess_veg=%lf ess_snow=%lf ess_soil=%lf Ldownzone=%lf\n          Ldown=%lf Kstar_canopy=%lf OF=%lf Ldownsub=%lf Tsnow=%lf Tss=%lf Tsoil=%lf Tcan=%lf",
			   patch[0].snowpack.water_equivalent_depth,
			   ess_veg,
			   ess_snow,
			   ess_soil,
			   zone[0].Ldown/86.4,
			   Ldownini/86.4, 
			   KstarH/86.4,
			   patch[0].overstory_fraction,
			   Ldown_canopy/86.4, 
			   patch[0].snowpack.T, 
			   Tss, 
			   patch[0].Tsoil,
			   Tcan);
		printf("\n          Lup_can=%lf Lup_sno=%lf Lup_soil=%lf Lup_pond=%lf Lstar_can=%lf \n          det=%lf B=%lf B*Kstar=%lf Ldown_avg=%lf Ldown_post=%lf",
			   Lup_canopy/86.4,
			   Lup_snow/86.4, 
			   Lup_soil/86.4, 
			   Lup_pond/86.4,
			   Lstar/86.4, 
			   patch[0].detention_store,
			   B,
			   B * KstarH/86.4, 
			   (ess_veg * (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0))/86.4,
			   Ldown_canopy/86.4);
	}

	/* Longwave from the canopy is unstable - due to poor estimation of cnaopy temperature - we prevent large longwave losses (that lead to  */
	/* underestimation of the canopy net radiation (and transpiration), by not letting Lstar_canopy be strongly negative during growing season */
   if (Tcan > 0.0 && Lstar < 0.0) Lstar = 0.0;
   if (zone[0].metv.tnight > 0.0 && Lstar_night < 0.0) Lstar_night = 0.0;
   if (zone[0].metv.tday > 0.0 && Lstar_day < 0.0) Lstar_day = 0.0;
	
   stratum[0].Lstar = Lstar;
   stratum[0].Lstar_night = Lstar_night;
   stratum[0].Lstar_day = Lstar_day;
	
} /*end compute_Lstar*/
