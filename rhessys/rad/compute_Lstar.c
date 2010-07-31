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
	/*--------------------------------------------------------------*/
	/*	Use daylength for longwave computations.	*/
	/*--------------------------------------------------------------*/
	daylength = zone[0].metv.dayl;
	nightlength = seconds_per_day - daylength;
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
	patch[0].Lup_soil = ess_soil * SBC * pow( patch[0].Tsoil + 273.0 , 4.0)
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
	if ( patch[0].snowpack.water_equivalent_depth > 0 ){
		/*--------------------------------------------------------------*/
		/*	Compute with a snow layer									*/
		/*--------------------------------------------------------------*/
		patch[0].Lstar_soil = patch[0].Lup_snow - patch[0].Lup_soil;
	/*	patch[0].Lstar_snow = patch[0].Lup_canopy
			+ patch[0].Lup_soil - 2 *patch[0].Lup_snow;*/
		patch[0].Lstar_canopy = patch[0].Lup_snow
			+ zone[0].Ldown - 2 * patch[0].Lup_canopy;
	}
	else{
		/*--------------------------------------------------------------*/
		/*	Compute without a snow layer								*/
		/*--------------------------------------------------------------*/
		patch[0].Lstar_soil = patch[0].Lup_canopy - patch[0].Lup_soil;
		patch[0].Lstar_canopy = patch[0].Lup_soil + zone[0].Ldown
			- 2 * patch[0].Lup_canopy;
	/*	patch[0].Lstar_snow = 0.0;*/
	} /*end if - else snowpack */
} /*end compute_Lstar*/
