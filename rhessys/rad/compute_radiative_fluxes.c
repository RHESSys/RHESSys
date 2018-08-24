/*--------------------------------------------------------------*/
/*								*/
/*		compute_radiative_fluxes				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_radiative_fluxes-updates downwelling radiative flux*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_radiative_fluxes(	double	*,		*/
/*					double,			*/
/*					double,			*/
/*					double)			*/
/*								*/
/*	OPTIONS							*/
/*	*flux_down_ptr (Kj/m2*day) - downwelling flux.		*/
/*	total_extininction (DIM ) - arguement for extinction 	*/
/*	reflectance (DIM) - reflectance of plant elements	*/
/*	absorptance (DIM) - absorptance of plant elements	*/ 
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Makes use of a Beer's law modified for gap fraction	*/
/*	formulation to compute a single scattering approximation*/
/*	to attenuation of a single stream of radiative flux.	*/
/*								*/
/*	Updates the radiative flux via a pointer		*/
/*	and returns the flux absorbed by the medium.		*/
/*								*/
/*	Note that the reflectance and absorptance should be	*/
/*	specified for the typical region not part of a gap.	*/
/*	Ideally, this corresponds to a big leaf's reflectance 	*/
/*	and absorptance required to match the hemispherical	*/
/*	albedo of the canopy areas that are intercepting the	*/
/*	radiation.  The Beer's law formulation actually applies	*/
/*	if we assume that the light intercepting elements are	*/
/*	opaque.  The extinction coefficient corrects for the	*/
/*	following terms:					*/
/*								*/
/*	coefficient = G(theta) * transmittance / cos ( theta )	*/
/*								*/
/*	Where :	theta = angle of incidence between stratum and	*/
/*			direct beam				*/
/*		G*theta) = projection of typical canopy element	*/
/*			in plane normal to direct beam		*/
/*		transmittance = average transmittance of canopy	*/
/*			elements to downwelling radiation.	*/
/*								*/
/*	Reference: Norman and Campbell, ...			*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	We keep the terminology general here since both Kdown	*/
/*	and PAR can be attenuated using this routine.		*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_radiative_fluxes(int verbose_flag,
								 double	*flux_down_ptr,  
								 double *flux_up_ptr,
								 double	total_extinction,
								 double	reflectance,
								 double	absorptance)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	flux_reflected;
	double	flux_transmitted;
	double	flux_absorbed;
	double	opaque_transmittance;

	if( verbose_flag > 2)
		printf("%8.1f %8.1f %8.1f %8.1f ", *flux_down_ptr, total_extinction,
		reflectance, absorptance);
	/*--------------------------------------------------------------*/
	/*	We assume that the stratum reflectance has already	*/
	/*	been corrected for canopy pai and understory albedo	*/
	/*--------------------------------------------------------------*/
	*flux_up_ptr = *flux_down_ptr * reflectance ;
	/*--------------------------------------------------------------*/
	/*	Compute transmittance	using Beer's Law approx.	*/
	/*	This stage assumes that the canopy elements are 	*/
	/*	opaque							*/
	/*--------------------------------------------------------------*/
	opaque_transmittance = exp(-1.0 * total_extinction);
	flux_absorbed = *flux_down_ptr * absorptance * (1.0 - opaque_transmittance );
	flux_transmitted = *flux_down_ptr -  *flux_up_ptr - flux_absorbed ;
	*flux_down_ptr = flux_transmitted;
	if( verbose_flag > 2)
		printf("%8.1f %8.1f %8.1f %8.1f ", opaque_transmittance, *flux_up_ptr,
		flux_absorbed,flux_transmitted);
	if ( verbose_flag == -5 ){
		printf("\nRADFLUX: trans=%lf fluxrefl=%1f fluxabs=%lf fluxtrans=%lf ", opaque_transmittance, *flux_up_ptr,
		   flux_absorbed,flux_transmitted);
	}
	return( flux_absorbed );
} /*end compute_radiative_fluxes*/
