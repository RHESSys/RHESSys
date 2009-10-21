/*--------------------------------------------------------------*/
/* 								*/
/*					compute_soil_water_potential	*/
/*								*/
/* 	compute_soil_water_potential - computes pre-dawn leaf water potential*/
/*							*/
/*	NAME						*/
/*							*/
/*	SYNOPSIS					*/
/*	double	compute_soil_water_potential(			*/
/*			int	,			*/
/*			int	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	,			*/
/*			double	);			*/
/*							*/
/*	OPTIONS						*/
/*	int	verbose_flag,				*/
/*	int	curve,					*/
/*  	double  p1 - curve parameter 1                  */
/*      double  p2 - curve parameter 2                  */
/*      double  p_0 - porosity		                */
/*      double  p porosity decay                  	*/
/*	double	psi_max (m)				*/
/*	double	sat_deficit- (m)			*/
/*	double	unsat_storage - (m water)		*/
/*	double  Tsoil (C)				*/
/*							*/
/*							*/
/*	DESCRIPTION					*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_soil_water_potential(
									 int	verbose_flag,
									 int	curve,
									 double	p1,
									 double 	p2,
									 double  p_0,
									 double  p,
									 double  psi_max,
									 double	sat_deficit,
									 double	unsat_storage,
									 double  Tsoil)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double  psi;
	/*--------------------------------------------------------------*/
	/*      Make sure p and p_0 are non zero.                       */
	/*--------------------------------------------------------------*/
	p = max(p,0.00000001);
	p_0 = max(p_0,0.00000001);
	/*--------------------------------------------------------------*/
	/*	compute soil water storage				*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Now we use Clapp and Hornberger relationship 	 	*/
	/*	or the Can Genuchten relation ship to			*/
	/*	define psi-theta curve					*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	If stot == 0 we set psi to its maximum value	*/
	/*--------------------------------------------------------------*/
	if  ((unsat_storage == 0.0) && (sat_deficit > 0.0000000001)){
		psi = psi_max;
	}
	else if ( sat_deficit <= 0.0000000001 ){
		psi = p1;
	}
	else{
		/*--------------------------------------------------------------*/
		/*	Compute predawn LWP without soil temp adjustment.	*/
		/*	this is maintained in m of water tension		*/
		/*--------------------------------------------------------------*/
		if ( curve == 1 ){
			psi = max(( p1 * pow ((unsat_storage /	sat_deficit) , -1/p2) ), p1);
		}
		else{
			psi = max(p1 * pow( pow(sat_deficit/unsat_storage,1/(1-1/p2))-1 ,
				1/p2 ), p1);
		}
	}
	/*--------------------------------------------------------------*/
	/*      Increase LWP is soil is cold.                           */
	/*      (I am not sure where the factor of 2.0 came from        */
	/*--------------------------------------------------------------*/
	if ( Tsoil < 0.0000000001 ) psi = psi * 2.0;
	psi = min(psi, psi_max);
	return(psi);
} /*end compute_soil_water_potential)*/
