/*--------------------------------------------------------------*/
/* 								*/
/*		update_litter_interception_capacity				*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_litter_interception_capacity -  rain intercepted by patch and 	*/
/*		updates rain flaaing through.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	update_litter_interception_capacity(				*/
/*			struct	canopy_strata_object	*);	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Updates litter interception capacity and wind		*/
/*	attenuation coeffient variables - these should		*/
/*	be dynamic i.e as there is more litter, litter		*/
/*	pai should grow; specific interception capacity		*/
/*	should be a function of the type of litter		*/
/*	for now though a simple switch is implemented		*/
/*	i.e no litter or litter					*/
/*	also compute litter depth				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	update_litter_interception_capacity(						double litter_moist_coef,
											double litter_density,
											struct litter_c_object	*litr_cs,
											struct litter_object	*litter)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double total_litter_C;
	
	/*--------------------------------------------------------------*/
	/*	 litter is assumed to have a mositure capacity of 	*/
	/*	given by litter_moist_coef which is a vegetation paramter */
	/*--------------------------------------------------------------*/
	total_litter_C = litr_cs->litr1c + litr_cs->litr2c
		+ litr_cs->litr3c + litr_cs->litr4c;


	litter->rain_capacity =  total_litter_C * litter_moist_coef * litter->cover_fraction; 

	if (litter->rain_capacity < ZERO) litter->rain_capacity = 0;
	
	if (total_litter_C > ZERO){
		litter->proj_pai = 1.0*litter->cover_fraction;
		litter->depth = total_litter_C / litter_density;
	}
	else{
		litter->proj_pai = 0.0;
		litter->depth = 0.0;
	}
	return;
} /*end update_litter_interception_capacity*/

