/*--------------------------------------------------------------*/
/*								*/
/*		compute_direct_radiative_fluxes			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_direct_radiative_fluxes 	-		*/
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
/*					double)			*/
/*								*/
/*	OPTIONS							*/
/*	*flux_down_ptr (Kj/m2*day) - downwelling flux.		*/
/*	extinction_coef (DIM) - extinction coef for nonopaquenes*/
/*	gap_fraction (DIM)- canopy gap fraction from  above	*/
/*	proj_pai (DIM)- projected area of layer element		*/
/*	theta_noon (rad) - solar zenith angle at noon		*/
/*	reflectance_canopy (DIM) - reflectance of this layer	*/
/*	reflectance_bkgd (DIM) - reflectance of lower layer	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Uses equation of Chen et. al, "BEPS", 1997.		*/
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
/*	PROGRAMMER NOTES					*/
/*								*/
/*	We keep the terminology general here since both Kdown	*/
/*	and PAR can be attenuated using this routine.		*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_direct_radiative_fluxes(	int	verbose_flag,
										double	*flux_down_ptr,
										double  *flux_up_ptr,
										double	extinction_coef,
										double	gap_fraction,
										double	proj_pai,
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
	double  unreflected_fraction;
	double  day_correction;
	double  noon_extinction;
	double  noon_transmission;
	double  day_absorption;
	/*--------------------------------------------------------------*/
	/*      We use Jing's Method if lai > 1                         */
	/*      For lai < 1 we can approximate transmission weighted    */
	/*      for solar zenith angle as 10% of the transmission at    */
	/*      solar noon.                                             */
	/*--------------------------------------------------------------*/
	*flux_up_ptr = *flux_down_ptr * reflectance_canopy;
	unreflected_fraction = ( 1- reflectance_canopy);
	noon_extinction = extinction_coef*(1-gap_fraction) * proj_pai/cos(theta_noon);
	if ( verbose_flag == -5 ){
		printf("\n          DIRRADFLUX: noonext=%lf extcoef=%lf thetanoon=%lf",noon_extinction, extinction_coef, theta_noon);
	}
	if ( noon_extinction < -0.3  ){
		noon_transmission = exp(noon_extinction);
		day_correction  =  -1 * (( 1 - reflectance_bkgd )
			* ((theta_noon - 3.14159/2)*sin(theta_noon)
			+ cos(theta_noon))) / ((3.14159/2-theta_noon)*(1-sin(theta_noon)));
		day_correction  =  -1 * (( 1 - reflectance_bkgd )
			* ((theta_noon - 3.14159/2)*sin(theta_noon)
			+ cos(theta_noon))) / ((3.14159/2-theta_noon)*(1-sin(theta_noon)));
		day_absorption =  unreflected_fraction + day_correction
			* noon_transmission;
	}
	else{
		/*--------------------------------------------------------------*/
		/*	We induce a 10% reduction in transmission by using	*/
		/*	the approximation that at a noon_extinction of 1	*/
		/*	an increase of 10% gives a net reduction of		*/
		/*	10% in transmission.					*/
		/*	This is nice in that we reduction in transmission 	*/
		/*	drops of for really small leaf areas (i.e. we		*/
		/*	have a limit oof 100% transmssion at lai=0)		*/
		/*--------------------------------------------------------------*/
		noon_extinction = ( noon_extinction *1.1 );
		/*--------------------------------------------------------------*/
		/*      This is just Beer's law integrated over a uniform       */
		/*      vertical leaf area distn.                               */
		/*--------------------------------------------------------------*/
		day_absorption = unreflected_fraction * ( 1.0 - exp(noon_extinction) );
	}
	flux_absorbed = *flux_down_ptr * day_absorption;
	flux_transmitted = *flux_down_ptr * (1 - reflectance_canopy - day_absorption );
	*flux_down_ptr = flux_transmitted;
	if( verbose_flag > 2)
		printf("%8.1f %8.1f %8.1f ", *flux_up_ptr,flux_absorbed,
		flux_transmitted);
	if ( verbose_flag == -5 ){
		printf("\n               fluxrefl=%lf fluxabs=%lf fluxtrans=%lf dayabs=%lf canrefl=%lf", *flux_up_ptr,flux_absorbed,
		   flux_transmitted, day_absorption, reflectance_canopy);
	}
	return( flux_absorbed );
} /*end compute_radiative_fluxes*/
