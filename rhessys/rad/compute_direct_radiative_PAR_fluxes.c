/*--------------------------------------------------------------*/
/*								*/
/*		compute_direct_radiative_PAR_fluxes			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_direct_radiative_PAR_fluxes 	-		*/
/*		updates downwelling direct radiative flux	*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_radiative_fluxes(			*/
/*					int			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double)			*/
/*								*/
/*	OPTIONS							*/
/*	*flux_down_ptr (Kj/m2*day) - downwelling flux.		*/
/*	extinction_coef (DIM) - extinction coef for nonopaquenes*/
/*	gap_fraction (DIM)- canopy gap fraction from  above	*/
/*	proj_pai (DIM)- projected area of layer element		*/
/*	proj_lai (DIM)- projected area of leaf layer element	*/
/*	all_lai (DIM)- all sided area of leaf layer element	*/
/*	theta_noon (rad) - solar zenith angle at noon		*/
/*	reflectance_canopy (DIM) - reflectance of this layer	*/
/*	reflectance_bkgd (DIM) - reflectance of lower layer	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	NOTE: This computes MEAN APAR on a plant element in	*/
/*	the stratum.  This result is intended for stomatal 	*/
/*	conductance and NOT for energy budgets.  However,	*/
/*	as a side effect this routine DOES update the actual	*/
/*	magnitude of transmitted and reflected flux.		*/
/*								*/
/*	Uses equation of Chen et. al, "BEPS", 1997.		*/
/*	for total canopy APAR and then integrates over a 	*/
/*	vertical leaf distn.					*/
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
/*	opaque - the extinction coefficent can correct for this.*/
/*								*/
/*	This code also then computes the mean PAR irradiance in	*/
/*	regions of canopies coverd by a canoy element and not	*/
/*  	a gap by:						*/
/*								*/
/*	i) assuming a uniform vertical leaf area distn.		*/
/*	ii) assuming that the irradiance at height indexed by	*/
/*		leaf area L in the canopy is specified by	*/
/*		the equation of Chen et. al. adequately		*/
/*	iii) that the orradiance in a gap is unattenuated	*/
/*	iv) that the weighted gap and non gap irradiance in a	*/
/*		layer is equal to the mean layer irradiance.	*/
/*								*/	 
/*	PROGRAMMER NOTES					*/
/*								*/
/*	We keep the terminology general here since both Kdown	*/
/*	and PAR can be attenuated using this routine.		*/
/*								*/
/*	RAF - May 1 1997					*/
/*	Had to handle case where proj_pai == 0 by hard setting	*/
/*	absorption and transmission.  It is up to the user to	*/
/*	adjust relfectance .					*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_direct_radiative_PAR_fluxes(	int	verbose_flag,
						double	*flux_down_ptr,                
						double	extinction_coef,
						double	gap_fraction,
						double	proj_pai,
						double	proj_lai,
						double	all_lai,
						double	theta_noon,
						double	reflectance_canopy,
						double	reflectance_bkgd)
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
	double	unreflected_fraction;
	double	noon_extinction;
	double	noon_transmission;
	/*--------------------------------------------------------------*/
	/*	We use Jing's Method if lai > 1				*/
	/*	For lai < 1 we can approximate transmission weighted	*/
	/*	for solar zenith angle as 90% of the transmission at	*/
	/*	solar noon.						*/
	/*--------------------------------------------------------------*/
	flux_reflected = *flux_down_ptr * reflectance_canopy;
	
	unreflected_fraction = ( 1- reflectance_canopy);
	noon_extinction = extinction_coef*(1-gap_fraction)*proj_pai/cos(theta_noon);
	
	if (( noon_extinction == 0 ) || (all_lai < ZERO)) {
		noon_transmission = 1;
		flux_absorbed = 0;
	}
	else if ( noon_extinction < -0.3 ){
		noon_transmission = exp(noon_extinction);
		flux_absorbed = *flux_down_ptr * (unreflected_fraction-noon_transmission);
		    	
	}
	else{
		/*--------------------------------------------------------------*/
		/*	We induce an approx. 10% reduction in transmssion	*/
		/*	at a noon_extinction of 1.0 by scaling it by 1.1.	*/
		/*this is good as that in the limit of 0 lai we get	*/
		/*	a transmsssion of 1.					*/
		/*--------------------------------------------------------------*/
		noon_extinction = ( noon_extinction *1.1 );
		noon_transmission = exp(noon_extinction); 		
		/*--------------------------------------------------------------*/
		/*	Use beers law to infer total absorbed PAR		*/
		/*--------------------------------------------------------------*/
		  flux_absorbed = *flux_down_ptr * (unreflected_fraction-noon_transmission);
			
	}
	if ( flux_absorbed < 0 ) flux_absorbed = 0;
	 flux_transmitted = *flux_down_ptr - flux_reflected - flux_absorbed;
	 	
	
	*flux_down_ptr = flux_transmitted;
	if( verbose_flag > 2)
		printf("%8.1f %8.1f %8.1f ",
		flux_reflected,flux_absorbed,flux_transmitted);
	return( flux_absorbed );
} /*end compute_direct_radiative_APAR_fluxes*/
