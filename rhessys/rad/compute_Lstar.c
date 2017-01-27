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
#include <assert.h>

#include "phys_constants.h"
#include "rhessys.h"


void	compute_Lstar(int	verbose_flag,
					  struct	basin_object	*basin,
					  struct	zone_object	*zone,
					  struct	patch_object	*patch)
{
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	daylength;
	double	nightlength;
	double Tss, Tpond, Tsoil, Tcan, ess_at = 0.0, B, KstarH, skyview;
	double Lup_canopy, Lup_canopy_day, Lup_canopy_night, Ldown_canopy;
	double Lup_soil, Lup_snow, Lup_pond;
	
	
	/* Zero out fluxes */
	Lup_canopy_day = 0.0;
	Lup_canopy_night = 0.0;
	Lup_canopy = 0.0;
	Ldown_canopy = 0.0;
	patch[0].Ldown_subcanopy = 0.0;
	Lup_snow = 0.0;
	Lup_soil = 0.0;
	Lup_pond = 0.0;
	KstarH = 0.0;
	
	double Ldown_canopy_night = 0.0;
	double Ldown_canopy_day = 0.0;
	double Ldown_subcanopy_night = 0.0;
	double Ldown_subcanopy_day = 0.0;
	double Lup_soil_night = 0.0;
	double Lup_soil_day = 0.0;
	double Lup_snow_night = 0.0;
	double Lup_snow_day = 0.0;
	double Lup_pond_night = 0.0;
	double Lup_pond_day = 0.0;

	
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
	/*Tsoil = patch[0].Tsoil;*/
	Tsoil = zone[0].metv.tavg; /* not soil temp since that is integrated over full soil column and we just need surface */
	if (patch[0].snow_stored > ZERO)
		Tcan = min(zone[0].metv.tavg,0.0); /* assume canopy is air temp but maxes at 0 if snow present */
	else
		Tcan = patch[0].T_canopy;
	
	if (patch[0].overstory_fraction > 0.0) {
		
		skyview = 1.0 - patch[0].overstory_fraction;
		
		/*** Ldown model from Pomeroy et al 2009 ***/
		/*** Instead of modeling canopy temperature explicitly, they use an empirical factor to convert		***/
		/*** a portion of absorbed shortwave radiation to longwave emission. Pomeroy uses ess_can=0.98 		***/
		/*** and B=0.023. Here we use ess_veg default.														***/		
		B = 0.023; /* factor that controls how much of absorbed SW rad is converted to LW emission */
		/* Using RHESSys canopy absorb estimates but adding back in fraction converted to LW since this was already removed in canopy daily F*/
		KstarH = patch[0].Kstar_canopy; 
		
		/*patch[0].Ldown_subcanopy = (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0) 
								* ( skyview * ess_at + (1 - skyview) * ess_can ) + (B * KstarH);*/
		/*** End Ldown model from Pomeroy et al 2009 ***/
	
		/* With day/night split (not highly sensitive, but needed so that we can partition evap. between day and night) */
#ifdef DEBUG
		// Remove asserts once debugged
		assert(zone->metv.tday != -999.0);
		assert(zone->metv.tnight != -999.0);
		assert(zone->Ldown_night != -999.0);
		assert(zone->Ldown_day != -999.0);
#endif

		Ldown_canopy_night = ess_veg * SBC * pow( zone[0].metv.tnight + 273.0, 4.0 ) * nightlength / 1000.0;
		Lup_canopy_night = Ldown_canopy_night + (1 - ess_veg) * zone[0].Ldown_night;
		Ldown_canopy_day = ess_veg * SBC * pow( zone[0].metv.tday +	273.0, 4.0 ) * daylength / 1000.0;
		Lup_canopy_day = Ldown_canopy_day + (1 - ess_veg) * zone[0].Ldown_day;
		Ldown_canopy = Ldown_canopy_day  + Ldown_canopy_night;
		Lup_canopy = Lup_canopy_day + Lup_canopy_night;
		Ldown_subcanopy_night = ( skyview * zone[0].Ldown_night )
								+ ((1 - skyview) * Ldown_canopy_night);
		Ldown_subcanopy_day = ( skyview * zone[0].Ldown_day )
								+ ((1 - skyview) * Ldown_canopy_day);
		patch[0].Ldown_subcanopy = Ldown_subcanopy_night + Ldown_subcanopy_day;

	}
	else {
		/* zero out L canopies if no canopy present */
		Lup_canopy_day = 0.0;
		Lup_canopy_night = 0.0;
		Lup_canopy = 0.0;
		Ldown_canopy = 0.0;
		patch[0].Ldown_subcanopy = zone[0].Ldown;
		skyview = 1.0;
	}
	
	/* Determine surface longwave emissions */
	if ( patch[0].snowpack.water_equivalent_depth > 0 ) {
		/* snow case */
		Lup_snow_night = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * nightlength / 1000.0
				+ (1 - ess_snow) * Ldown_subcanopy_night;
		Lup_snow_day = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * daylength / 1000.0
				+ (1 - ess_snow) * Ldown_subcanopy_day;
		Lup_snow = Lup_snow_night + Lup_snow_day;

		/* ignoring reflected L between snow and soil surfaces */
		Lup_soil_night = Lup_soil_day = Lup_soil = 0.0;
	}
	else if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
		/* ponded water case */
		Lup_pond_night = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * nightlength / 1000.0
				+ (1 - ess_water) * Ldown_subcanopy_night;
		Lup_pond_day = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * daylength / 1000.0
				+ (1 - ess_water) * Ldown_subcanopy_day;
		Lup_pond = Lup_pond_night + Lup_pond_day;

		/* ignoring reflected L between water and soil surfaces */
		Lup_soil_night = Lup_soil_day = Lup_soil = 0.0;
	}
	else {
		/* bare soil case */
		Lup_snow_night = Lup_snow_day = Lup_snow = 0.0;

		Lup_soil_night = ess_soil * SBC * pow( Tsoil + 273.0 , 4.0) * nightlength / 1000.0
				+ (1 - ess_soil) * Ldown_subcanopy_night;
		Lup_soil_day = ess_soil * SBC * pow( Tsoil + 273.0 , 4.0) * daylength / 1000.0
				+ (1 - ess_soil) * Ldown_subcanopy_day;
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
		patch[0].Lstar_snow_night = Ldown_subcanopy_night - Lup_snow_night;
		patch[0].Lstar_snow_day = Ldown_subcanopy_day - Lup_snow_day;
		patch[0].Lstar_snow = patch[0].Lstar_snow_night + patch[0].Lstar_snow_day;

		patch[0].Lstar_soil_night = patch[0].Lstar_soil_day = patch[0].Lstar_soil = 0.0;
		patch[0].Lstar_pond_night = patch[0].Lstar_pond_day = patch[0].Lstar_pond = 0.0;

	} else if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
		/*--------------------------------------------------------------*/
		/*	Compute with ponded water									*/
		/*--------------------------------------------------------------*/
		patch[0].Lstar_pond_night = Ldown_subcanopy_night - Lup_pond_night;
		patch[0].Lstar_pond_day = Ldown_subcanopy_day - Lup_pond_day;
		patch[0].Lstar_pond = patch[0].Lstar_pond_night + patch[0].Lstar_pond_day;

		patch[0].Lstar_snow_night = patch[0].Lstar_snow_day = patch[0].Lstar_snow = 0.0;
		patch[0].Lstar_soil_night = patch[0].Lstar_soil_day = patch[0].Lstar_soil = 0.0;

	} else {
		/*--------------------------------------------------------------*/
		/*	Compute with bare soil										*/
		/*--------------------------------------------------------------*/
		patch[0].Lstar_soil_night = Ldown_subcanopy_night - Lup_soil_night;
		patch[0].Lstar_soil_day = Ldown_subcanopy_day - Lup_soil_day;
		patch[0].Lstar_soil = patch[0].Lstar_soil_night + patch[0].Lstar_soil_day;

		patch[0].Lstar_snow_night = patch[0].Lstar_snow_day = patch[0].Lstar_snow = 0.0;
		patch[0].Lstar_pond_night = patch[0].Lstar_pond_day = patch[0].Lstar_pond = 0.0;
	}
	
	/* Compute canopy Lstar */
	if (patch[0].overstory_fraction == 0.0) {
		patch[0].Lstar_canopy_night = patch[0].Lstar_canopy_day = patch[0].Lstar_canopy = 0.0;
	} else {
		patch[0].Lstar_canopy_night = (1 - skyview) * (Lup_snow_night + zone[0].Ldown_night - Lup_canopy_night - Ldown_canopy_night);
		patch[0].Lstar_canopy_day = (1 - skyview) * (Lup_snow_day + zone[0].Ldown_day - Lup_canopy_day - Ldown_canopy_day);
		patch[0].Lstar_canopy = patch[0].Lstar_canopy_night + patch[0].Lstar_canopy_day;
	}
	
	patch[0].Lup = (1 - skyview) * Lup_canopy + skyview * (Lup_snow + Lup_pond + Lup_soil);
	patch[0].Lup_soil = Lup_soil;
	
	if ( verbose_flag == -5 ){
		printf("\n     LSTAR: SWE=%lf ess_at=%lf ess_veg=%lf ess_snow=%lf ess_soil=%lf Kdownzone=%lf Ldownzone=%lf\n          Kstar_canopy=%lf OF=%lf Ldownsub=%lf Tsnow=%lf Tss=%lf Tsoil=%lf Tcan=%lf Tair=%lf skyview=%lf",
			   patch[0].snowpack.water_equivalent_depth,
			   ess_at,
			   ess_veg,
			   ess_snow,
			   ess_soil,
			   (zone[0].Kdown_direct + zone[0].Kdown_diffuse)/86.4,
			   zone[0].Ldown/86.4, 
			   KstarH/86.4,
			   patch[0].overstory_fraction,
			   patch[0].Ldown_subcanopy/86.4, 
			   patch[0].snowpack.T, 
			   Tss, 
			   Tsoil, 
			   Tcan,
			   zone[0].metv.tavg,
			   skyview);
		printf("\n          Lup_can=%lf Lup_sno=%lf Lup_soil=%lf Lup_pond=%lf Lstar_can=%lf Lstar_sno=%lf Lstar_soil=%lf Lstar_pond=%lf\n          det=%lf B*Kstar=%lf Ldown_avg=%lf Ldown_split=%lf Lup=%lf",
			   Lup_canopy/86.4,
			   Lup_snow/86.4, 
			   Lup_soil/86.4, 
			   Lup_pond/86.4,
			   patch[0].Lstar_canopy/86.4, 
			   patch[0].Lstar_snow/86.4, 
			   patch[0].Lstar_soil/86.4,
			   patch[0].Lstar_pond/86.4, 
			   patch[0].detention_store,
			   B * KstarH/86.4, 
			   (ess_veg * (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0))/86.4,
			   Ldown_canopy/86.4,
			   patch[0].Lup/86.4);
	}
	
} /*end compute_Lstar*/
