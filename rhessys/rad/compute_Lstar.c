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

void	compute_Lstar(
					  int	verbose_flag,
					  struct	basin_object	*basin,
					  struct	zone_object	*zone,
					  struct	patch_object	*patch) 
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	daylength;
	double	nightlength;
	double Tss, Tpond, ess_at, ess_can, B, KstarH, skyview, Ldown_canopy;
	
	
	/* Zero out fluxes */
	patch[0].Lup_canopy_day = 0.0;
	patch[0].Lup_canopy_night = 0.0;
	patch[0].Lup_canopy = 0.0;
	Ldown_canopy = 0.0;
	patch[0].Ldown_subcanopy = 0.0;
	patch[0].Lup_snow = 0.0;
	patch[0].Lup_soil = 0.0;
	patch[0].Lup_pond = 0.0;
	
	
	/*--------------------------------------------------------------*/
	/*	Use daylength for longwave computations.	*/
	/*--------------------------------------------------------------*/
	daylength = zone[0].metv.dayl;
	nightlength = seconds_per_day - daylength;
	
	/* Snow surface temp estimate from Brubaker 1996 as referenced in Dingman */
	/*Tss = patch[0].snowpack.T;*/
	/*Tss = min(zone[0].metv.tavg-2.5,0.0);*/
	Tss = zone[0].tdewpoint;
	
	/* Water temperature assumes same as air temperature */
	Tpond = zone[0].metv.tavg;
	
	/* Clear sky emissivity from Satterlund 1979 as applied in Mahat & Tarboten 2012 UEB */
	/* ess_at = zone[0].cloud_fraction + (1.0 - zone[0].cloud_fraction) * 1.08 
					* (1.0 - exp(-pow(zone[0].e_dewpoint/100,(zone[0].metv.tavg+273)/2016)));*/
	
	if (patch[0].overstory_fraction > 0.0) {
		/*** Ldown model from Pomeroy et al 2009 ***/
		/*** Instead of modeling canopy temperature explicitly, they use an empirical factor to convert		***/
		/*** a portion of absorbed shortwave radiation to longwave emission. Pomeroy uses ess_can=0.98 		***/
		/*** and B=0.023. Here we use ess_veg default.														***/
		
		B = 0.023; /* factor that controls how much of absorbed SW rad is converted to LW emission */
		KstarH = patch[0].Kstar_canopy; /* Using RHESSys canopy absorb estimates */
		skyview = 1.0 - patch[0].overstory_fraction;
		patch[0].Ldown_subcanopy = ( skyview * zone[0].Ldown ) 
										+ ((1 - skyview) * ess_veg * (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0))
										+ (B * KstarH);
		/*patch[0].Ldown_subcanopy = (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0) 
								* ( skyview * ess_at + (1 - skyview) * ess_can ) + (B * KstarH);*/
		/*** End Ldown model from Pomeroy et al 2009 ***/
	
		patch[0].Lup_canopy_night = ess_veg * SBC * pow( zone[0].metv.tnight + 273.0, 4.0 ) * nightlength / 1000.0;
		patch[0].Lup_canopy_day = ess_veg * SBC * pow( zone[0].metv.tday +	273.0, 4.0 ) * daylength / 1000.0;
		/* added canopy reflected Ldown from atmosphere */
		patch[0].Lup_canopy =   patch[0].Lup_canopy_day  + patch[0].Lup_canopy_night + (1 - ess_veg) * zone[0].Ldown;
		Ldown_canopy = patch[0].Lup_canopy_day  + patch[0].Lup_canopy_night;
	}
	else {
		/* zero out L canopies if no canopy present */
		patch[0].Lup_canopy_day = 0.0;
		patch[0].Lup_canopy_night = 0.0;
		patch[0].Lup_canopy = 0.0;
		Ldown_canopy = 0.0;
		patch[0].Ldown_subcanopy = zone[0].Ldown;
	}
	
	/* snow case */
	if ( patch[0].snowpack.water_equivalent_depth > 0 ) {
		patch[0].Lup_snow = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * seconds_per_day / 1000.0 
								+ (1 - ess_snow) * patch[0].Ldown_subcanopy;
		/* ignoring reflected L between snow and soil surfaces */
		patch[0].Lup_soil = 0.0;
	}
	else {
		/* ponded water case */
		if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
			patch[0].Lup_pond = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * seconds_per_day / 1000.0 
									+ (1 - ess_water) * patch[0].Ldown_subcanopy;
			/* ignoring reflected L between water and soil surfaces */
			patch[0].Lup_soil = 0.0;
		}
		/* bare soil case */
		else {
			patch[0].Lup_snow = 0.0;
			patch[0].Lup_soil = ess_soil * SBC * pow( patch[0].Tsoil + 273.0 , 4.0) * seconds_per_day / 1000.0
								+ (1 - ess_soil) * patch[0].Ldown_subcanopy;
		}
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
		patch[0].Lstar_soil = 0.0;
		patch[0].Lstar_snow = patch[0].Ldown_subcanopy - patch[0].Lup_snow;
		patch[0].Lstar_canopy = (1 - skyview) * (patch[0].Lup_snow + zone[0].Ldown - patch[0].Lup_canopy - Ldown_canopy);
	}
	else {
		/*--------------------------------------------------------------*/
		/*	Compute with ponded water									*/
		/*--------------------------------------------------------------*/		
		if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
			patch[0].Lstar_snow = 0.0;
			patch[0].Lstar_soil = 0.0;
			patch[0].Lstar_pond = patch[0].Ldown_subcanopy - patch[0].Lup_pond;
			patch[0].Lstar_canopy = (1 - skyview) * (patch[0].Lup_pond + zone[0].Ldown - patch[0].Lup_canopy - Ldown_canopy);
		}
		else {
			/*--------------------------------------------------------------*/
			/*	Compute with bare soil										*/
			/*--------------------------------------------------------------*/
			patch[0].Lstar_soil = patch[0].Ldown_subcanopy - patch[0].Lup_soil;
			patch[0].Lstar_canopy = (1 - skyview) * (patch[0].Lup_soil + zone[0].Ldown - patch[0].Lup_canopy - Ldown_canopy);
			patch[0].Lstar_snow = 0.0;
			patch[0].Lstar_pond = 0.0;
		}
	}
	if (patch[0].overstory_fraction == 0.0) {
		patch[0].Lstar_canopy = 0.0;
	}
	
	patch[0].Lup = (1 - skyview) * patch[0].Lup_canopy + skyview * (patch[0].Lup_snow + patch[0].Lup_pond + patch[0].Lup_soil);
	
	
	if ( verbose_flag == -5 ){
		printf("\n     LSTAR: SWE=%lf ess_at=%lf ess_veg=%lf ess_snow=%lf ess_soil=%lf Kdownzone=%lf Ldownzone=%lf\n          Kstar_canopy=%lf OF=%lf Ldownsub=%lf Tsnow=%lf Tss=%lf Tsoil=%lf skyview=%lf",
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
			   patch[0].Tsoil, 
			   skyview);
		printf("\n          Lup_can=%lf Lup_sno=%lf Lup_soil=%lf Lup_pond=%lf Lstar_can=%lf Lstar_sno=%lf Lstar_soil=%lf Lstar_pond=%lf\n          det=%lf B*Kstar=%lf Ldown_avg=%lf Ldown_split=%lf",
			   patch[0].Lup_canopy/86.4,
			   patch[0].Lup_snow/86.4, 
			   patch[0].Lup_soil/86.4, 
			   patch[0].Lup_pond/86.4,
			   patch[0].Lstar_canopy/86.4, 
			   patch[0].Lstar_snow/86.4, 
			   patch[0].Lstar_soil/86.4,
			   patch[0].Lstar_pond/86.4, 
			   patch[0].detention_store,
			   B * KstarH/86.4, 
			   (ess_veg * (SBC*86400/1000) * pow((zone[0].metv.tavg+273), 4.0))/86.4,
			   Ldown_canopy/86.4);
	}
	
	
	/* OLD CODE */
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Compute patch level long wave radiation processes.			*/
	/*	We use yesterdays snowpack.									*/
	/*																*/
	/*	Also, we assume Ldown == Lup for layers in the patch so		*/
	/*	we only compute Lup.										*/
	/* 									*/
	/*kJ/m**2 = J/m **2/s/K **4 * (K ** 4) * sec_per_day/ day / KJ/1000 J		*/
	/*								*/
	/*	appropriate emissivities should be subsitiuted where possilbe */
	/*								*/
	/*--------------------------------------------------------------*/
	/*patch[0].Lup_soil = ess_soil * SBC * pow( patch[0].Tsoil + 273.0 , 4.0)
		* seconds_per_day / 1000.0;
	patch[0].Lup_canopy_night = ess_veg * SBC
		* pow( zone[0].metv.tnight	+ 273.0, 4.0 ) * nightlength / 1000.0;
	patch[0].Lup_canopy_day = ess_veg * SBC
		* pow( zone[0].metv.tday +	273.0, 4.0 ) * daylength / 1000.0;
	patch[0].Lup_canopy =   patch[0].Lup_canopy_day  + patch[0].Lup_canopy_night;
	if ( patch[0].snowpack.water_equivalent_depth > 0 ){
		patch[0].Lup_snow = ess_snow * SBC
			* pow( 273.0 + patch[0].snowpack.T ,4.0) * seconds_per_day / 1000.0;
	}
	else{
		patch[0].Lup_snow = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	Compute daily Lstars.										*/
	/*																*/
	/*	We consider the canopy as a homogenous slab with temp		*/
	/*	equal to the air temp of the canopy and do not distinguish	*/
	/*	between strata.												*/
	/*--------------------------------------------------------------*/
	/*if ( patch[0].snowpack.water_equivalent_depth > 0 ){
		/*--------------------------------------------------------------*/
		/*	Compute with a snow layer									*/
		/*--------------------------------------------------------------*/
		/*patch[0].Lstar_soil = patch[0].Lup_snow - patch[0].Lup_soil;
		patch[0].Lstar_snow = patch[0].Lup_canopy
			+ patch[0].Lup_soil - 2 *patch[0].Lup_snow;
		patch[0].Lstar_canopy = patch[0].Lup_snow
			+ zone[0].Ldown - 2 * patch[0].Lup_canopy;
	}
	else{
		/*--------------------------------------------------------------*/
		/*	Compute without a snow layer								*/
		/*--------------------------------------------------------------*/
		/*patch[0].Lstar_soil = patch[0].Lup_canopy - patch[0].Lup_soil;
		patch[0].Lstar_canopy = patch[0].Lup_soil + zone[0].Ldown
			- 2 * patch[0].Lup_canopy;
		patch[0].Lstar_snow = 0.0;
		patch[0].Ldown_subcanopy = patch[0].Lup_canopy;
	} /*end if - else snowpack */
} /*end compute_Lstar*/
