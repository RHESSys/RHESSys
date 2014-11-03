/*--------------------------------------------------------------*/
/* 								*/
/*		compute_infiltration_scm				*/
/*								*/
/*	NAME							*/
/*	compute_infiltration - estimates vertical 		*/
/*		drainage into soil		.		*/
/*								*/
/*								*/
/*	SYNOPSIS					*/
/*	compute_infiltration_scm(	*/
/*				double  ,       */
/*				double	,		*/	
/*				double	,	)	*/
/*								*/
/*	returns:						*/
/*	infiltration - (m water) infiltration 			*/
/*								*/
/*	OPTIONS							*/
/*	double time_int 					*/
/*	double infiltration_rate - design infiltration rate [m/hr]		*/
/*	double detention store			*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes infiltration based on a standard rate				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_infiltration_scm(double duration,
							 double infiltration_rate,
							 double detention_store)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double infiltration; // m of water
	
	/*------------------------------------------------------*/
	/*	Computation				 							*/
	/*------------------------------------------------------*/	
	// I believe "duration" (fed in as time_int) is in days (following from compute_infiltration)
	// Infilration_rate should be parameterized m/d
	
	infiltration = min(detention_store, infiltration_rate * duration);

	return(infiltration);
} /*compute_infiltration_scm*/
