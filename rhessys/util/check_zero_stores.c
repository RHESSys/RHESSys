/*--------------------------------------------------------------*/
/* 								*/
/*		check_zero_stores				*/
/*								*/
/*								*/
/*	NAME							*/
/*		check_zero_stores
/*								*/
/*	SYNOPSIS						*/
/*	double	check_zero_stores(				*/
/*			  struct  soil_c_object   *               */
  /*                      struct  soil_n_object   *               */
  /*                      struct  litter_c_object *               */
  /*                      struct  litter_n_object *               */
/*				)				*/	
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	zeros all daily patch fluxes (carbon and nitrogen)	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int check_zero_stores(
   struct  soil_c_object   *cs_soil,
   struct  soil_n_object   *ns_soil,
   struct  litter_c_object *cs_litr,
   struct  litter_n_object *ns_litr)

{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;

	if (cs_litr->litr1c < ZERO) cs_litr->litr1c = ZERO;
	if (cs_litr->litr2c < ZERO) cs_litr->litr2c = ZERO;
	if (cs_litr->litr3c < ZERO) cs_litr->litr3c = ZERO;
	if (cs_litr->litr4c < ZERO) cs_litr->litr4c = ZERO;

	if (ns_litr->litr1n < ZERO) ns_litr->litr1n = ZERO;
	if (ns_litr->litr2n < ZERO) ns_litr->litr2n = ZERO;
	if (ns_litr->litr3n < ZERO) ns_litr->litr3n = ZERO;
	if (ns_litr->litr4n < ZERO) ns_litr->litr4n = ZERO;

	if (cs_soil->soil1c < ZERO) cs_soil->soil1c = ZERO;
	if (cs_soil->soil2c < ZERO) cs_soil->soil2c = ZERO;
	if (cs_soil->soil3c < ZERO) cs_soil->soil3c = ZERO;
	if (cs_soil->soil4c < ZERO) cs_soil->soil4c = ZERO;

	if (ns_soil->soil1n < ZERO) ns_soil->soil1n = ZERO;
	if (ns_soil->soil2n < ZERO) ns_soil->soil2n = ZERO;
	if (ns_soil->soil3n < ZERO) ns_soil->soil3n = ZERO;
	if (ns_soil->soil4n < ZERO) ns_soil->soil4n = ZERO;

	if (ns_soil->nitrate < ZERO) ns_soil->nitrate = ZERO;
	if (ns_soil->sminn < ZERO) ns_soil->sminn = ZERO;

	return (!ok);
}/* end check_zero_stores */





