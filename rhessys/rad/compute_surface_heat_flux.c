/*--------------------------------------------------------------*/
/* 								*/
/*			compute_surface_heat_flux				*/
/*								*/
/*	compute_surface_heat_flux - computes heat flux	*/
/*		between surface of patch and a stratum.		*/
/*								*/
/*	NAME							*/
/*	compute_surface_heat_flux						*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_surface_heat_flux(				*/
/*							int,					*/
/*							double,					*/
/*							double,					*/
/*							double,					*/
/*							double,					*/
/*							double,					*/
/*							double,					*/
/*							double)					*/
/*													*/
/*	OPTIONS											*/
/*	snow_stored - (mm water) snow stored on stratum		*/
/*	rain_stored - (mm water) rain stored on stratum		*/
/*	rain_capacity - (MM WATER ) STRATUM CAPACITY FOR WATER	*/
/*	tsurface - (deg K) stratum surface temperature		*/
/*	tdeep - (deg K) stratum temperature at depth deltaZ	*/
/*	deltaZ - (m) depth for tdeep				*/
/*	min_heat_cap - (Jm-3K-1) like it says	*/
/*	max_heat_cap - (Jm-3K-1) like it says	*/
/*								*/
/*	returns: surface_heat_flux kJ				*/
/*	DESCRIPTION										*/
/*	Uses a change in mean profile temperature combined with */
/*	assumption that heat_cap is constant with depth.	*/
/*													*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Sep 2, 1997 RAF									*/
/*	New code replaces code found in moss routines.	*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_surface_heat_flux(
								  int verbose_flag,
								  double	snow_stored,
								  double	rain_stored,
								  double	rain_capacity,
								  double	tsurface_initial,
								  double	tsurface_final,
								  double	tdeep,
								  double	deltaz,
								  double	min_heat_cap,
								  double	max_heat_cap)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double 	surface_heat_flux;
	double	temp1;
	double	temp2;
	/*--------------------------------------------------------------*/
	/*	COnvert temps to kelvin					*/
	/*--------------------------------------------------------------*/
	tsurface_initial += 273;
	tsurface_final += 273;
	tdeep += 273;
	/*--------------------------------------------------------------*/
	/*	Determine if there is snow stored 	*/
	/*--------------------------------------------------------------*/
	surface_heat_flux = 0;
	if ( snow_stored == 0 ){
		/*--------------------------------------------------------------*/
		/*		Figure out intital mean profile temp (deg K)	*/
		/*--------------------------------------------------------------*/
		if (tsurface_initial!=tdeep){
			temp1 = (1 - tdeep/tsurface_initial) / log(tdeep/tsurface_initial )
				* tsurface_initial;
		}
		else{
			temp1 = tdeep;
		}
		/*--------------------------------------------------------------*/
		/*		Figure out final mean profile temp (deg K)	*/
		/*--------------------------------------------------------------*/
		if (tsurface_final!=tdeep){
			temp2 = (1 - tdeep/tsurface_final) / log(tdeep/tsurface_final )
				* tsurface_final;
		}
		else{
			temp2 = tdeep;
		}
		if ( rain_capacity > 0 ){
			surface_heat_flux = max(min_heat_cap +	(max_heat_cap - min_heat_cap)
				* (rain_stored /  rain_capacity),0)	/ deltaz	* ( temp1 - temp2 );
		}
		else{
			surface_heat_flux = min_heat_cap * deltaz * (temp1-temp2);
		}
	}
	return(surface_heat_flux/1000);
} /*end compute_surface_heat_flux*/
