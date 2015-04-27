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

double	compute_Lstar_canopy(
					  int	verbose_flag,
					  double *Ldown,
					  double KstarH,
					  double snow_stored,
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
	
	/*--------------------------------------------------------------*/
	/*	Use daylength for longwave computations.	*/
	/*--------------------------------------------------------------*/
	daylength = zone[0].metv.dayl;
	nightlength = seconds_per_day - daylength;
	
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

	
	Ldownini= *Ldown;
	
	
	Lup_canopy = ess_veg * SBC * pow( Tcan + 273.0, 4.0 ) * 86400 / 1000.0
					+ (1 - ess_veg) * *Ldown; /* added canopy reflected Ldown from atmosphere */
	Ldown_canopy = ess_veg * SBC * pow( Tcan + 273.0, 4.0 ) * 86400 / 1000.0;
		
	/*KstarH -= B * KstarH;*/ /*removing in canopy stratum daily f*/
	
	/* snow case */
	if ( patch[0].snowpack.water_equivalent_depth > 0 ) {
		Lup_snow = ess_snow * SBC * pow( 273.0 + Tss ,4.0) * seconds_per_day / 1000.0 
								+ (1 - ess_snow) * Ldown_canopy;
		/* ignoring reflected L between snow and soil surfaces */
		Lup_soil = 0.0;
	}
	else {
		/* ponded water case */
		if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
			Lup_pond = ess_water * SBC * pow( 273.0 + Tpond ,4.0) * seconds_per_day / 1000.0 
									+ (1 - ess_water) * Ldown_canopy;
			/* ignoring reflected L between water and soil surfaces */
			Lup_soil = 0.0;
		}
		/* bare soil case */
		else {
			Lup_snow = 0.0;
			Lup_soil = ess_soil * SBC * pow( Tsoil + 273.0 , 4.0) * seconds_per_day / 1000.0
								+ (1 - ess_soil) * Ldown_canopy;
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
		Lstar = Lup_snow + *Ldown - Lup_canopy - Ldown_canopy;
	}
	else {
		/*--------------------------------------------------------------*/
		/*	Compute with ponded water									*/
		/*--------------------------------------------------------------*/		
		if ( patch[0].detention_store > (patch[0].litter.rain_capacity - patch[0].litter.rain_stored) ) {
			Lstar = Lup_pond + *Ldown - Lup_canopy - Ldown_canopy;
		}
		else {
			/*--------------------------------------------------------------*/
			/*	Compute with bare soil										*/
			/*--------------------------------------------------------------*/
			Lstar = Lup_soil + *Ldown - Lup_canopy - Ldown_canopy;
		}
	}
	
	/*Lstar += B * KstarH;*/
	*Ldown = Ldown_canopy;
	
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
   if (Tcan > 0 && Lstar < 0) Lstar=0.0;
	
    return( Lstar );	
	
} /*end compute_Lstar*/
