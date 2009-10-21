/*--------------------------------------------------------------*/
/*								*/
/*		atm_pres					*/
/*								*/
/*	NAME							*/
/*	atm_pres.c						*/	
/*								*/
/*	SYNOPSIS						*/
/*	double	atm_pres( double	);			*/
/*								*/
/*	OPTIONS							*/
/*	double	elevation - (m) elev. above sealevel		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Input:							*/
/*	double	elevation - (m) elev. above sealevel		*/
/*								*/	
/*	Ouptut:							*/
/*	Pair - daily atmospheric pressure 	(Pa)		*/
/*								*/	
/*	From the discussion on atmospheric statics in :		*/
/*	Iribane, J.V. and W.L. Godson, 1981.  Atmospheric	*/
/*		Thermodynamic, 2nd. Ed., D. Reidel Publishing	*/
/*		Company, Dordrecht, The Netherlands, p. 168.	*/
/*	Fomn bbgc						*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Fomn bbgc						*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

double	atm_pres( double	elevation )
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	double	Pair;
	double	t1;
	double	t2;
	
	/*--------------------------------------------------------------*/
	/*	Compute atm_pres					*/
	/*--------------------------------------------------------------*/
	t1 = 1.0 - ( LR_STD * elevation ) / T_STD;
	t2 = G_STD / ( LR_STD * ( R / MA ));
	Pair = P_STD * pow(t1,t2);
	return(Pair);
} /*end atm_pres*/
