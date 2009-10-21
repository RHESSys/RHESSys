/*--------------------------------------------------------------*/
/* 								*/
/*		update_septic					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_septic -  					*/
/*		adds leakage from septic to soil N		*/
/*								*/
/*	SYNOPSIS						*/
/*	int update_septic(					*/
/*			struct date,				*/
/*			struct	soil_n_object	*		*/
/*			double)					*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*	preset code just uses a user assigned loading rate	*/
/*	and all of it is nitrate				*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_septic(
				  struct	date	current_date,
				  struct  patch_object   *patch)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.			*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 			*/
	/*------------------------------------------------------*/
	int ok = 1;

	patch[0].soil_ns.nitrate += patch[0].landuse_defaults[0][0].septic_NO3_load; 
	patch[0].unsat_storage += patch[0].landuse_defaults[0][0].septic_water_load;
		
	return (!ok);
} /* end update_septic.c */
