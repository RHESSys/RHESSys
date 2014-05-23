/*
* SUMMARY:      CalcAerodynamic.c - Calculate the aerodynamic resistances
* USAGE:        Part of DHSVM
*
* AUTHOR:       Bart Nijssen and Pascal Storck
* ORG:          University of Washington, Department of Civil Engineering
* E-MAIL:       nijssen@u.washington.edu, pstorck@u.washington.edu
* ORIG-DATE:    Thu Mar 27 18:00:10 1997
* DESCRIPTION:  Calculate the aerodynamic resistances
* DESCRIP-END.
* FUNCTIONS:    CalcAerodynamic()
* COMMENTS:
* $Id: CalcAerodynamic.c,v 1.4 2003/07/01 21:26:09 olivier Exp $     
*/
/*****************************************************************************
Function name: CalcAerodynamic()

Purpose      : Calculate the aerodynamic resistance for each vegetation
				layer, and the wind 2m above the layer boundary.  In case of
				an overstory, also calculate the wind in the overstory.
				The values are normalized based on a reference height wind
				speed, Uref, of 1 m/s.  To get wind speeds and aerodynamic
				resistances for other values of Uref, you need to multiply
				the here calculated wind speeds by Uref and divide the
				here calculated aerodynamic resistances by Uref
            
Required     :
int NVegLayers - Number of vegetation layers
char OverStory - flag for presence of overstory.  Only used if NVegLayers is equal to 1
float Zref     - Reference height for windspeed
float n        - Attenuation coefficient for wind in the overstory
float *Height  - Height of the vegetation layers (top layer first)
float Trunk    - Multiplier for Height_ov that indictaes the top of the trunk space
float *U       - Vector of length 2, with wind for vegetation layers
	If OverStory == TRUE the first value is the wind in
	the overstory canopy, and the second value the wind
	2m above the lower boundary.  Otherwise the first
	value is the wind 2m above the lower boundary and
	the second value is not used.
float *U2mSnow - Wind velocity 2m above the snow surface
float *Ra      - Vector of length 2, with aerodynamic resistance values. 
	If OverStory == TRUE the first value is the aerodynamic
	resistance for the overstory canopy, and the second
	value the aerodynamic resistance for the lower boundary. 
	Otherwise the first value is the aerodynamic resistance
	for the lower boundary and the second value is not used.
float *RaSnow  - Aerodynamic resistance for the snow surface.
	
Returns      : void
	
Modifies     :
float *U
float *U2mSnow
float *Ra     
float *RaSnow
  
Comments     :
*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double compute_ra_overstory(
							int verbose_flag, 
							double cn,
							double Trunk,
							double *U,
							double *Ucan,
							double *U2mSnow,
							double *Ustar,
							double Zref,
							double Height_ov,
							double Height_un,
							double *ga,
							double *gasnow)
{
	double d_Lower;
	double d_Upper;
	double K2;
	double Uh;
	double Ut;
	double Uw;
	double Z0_Lower;
	double Z0_Upper;
	double Zt;
	double Zw;
	double Rb;

	double U_ov, U_un, Ra_ov, Ra_un, RaSnow, ra;
	double VON_KARMAN, Z0_GROUND, Z0_SNOW, Z0_MULTIPLIER, D0_MULTIPLIER;
	double Uref;
	
	d_Lower = 0.0;
	d_Upper = 0.0;
	K2 = 0.0;
	Uh = 0.0;
	Ut = 0.0;
	Uw = 0.0;
	Z0_Lower = 0.0;
	Z0_Upper = 0.0;
	Zt = 0.0;
	Zw = 0.0;	
	Rb = 0.0;
	
		
	VON_KARMAN = 0.41;
	Z0_GROUND = 0.01;
	/*Z0_SNOW = 0.005; /* Using 0.005. Reba 2012 use 0.003 for sheltered or 0.0001 for exposed snow in Reynolds Creek*/
	if (Height_ov == 0.0) Z0_SNOW = 0.0001;
	else Z0_SNOW = 0.003;
		
	Z0_MULTIPLIER = 0.13;
	D0_MULTIPLIER = 0.63;	
		
	K2 = VON_KARMAN * VON_KARMAN;

	Uref = *U; 
	if (Uref <= ZERO) Uref = 0.1;
	
	/* If reference wind height is < canopy height, set to 2m above canopy */
	if (Zref <= Height_ov) Zref = Height_ov + 2.0;
	
	
	*Ustar = VON_KARMAN * Uref / ( log(Zref-(D0_MULTIPLIER*Height_ov)) / (Z0_MULTIPLIER*Height_ov) );

	

	/* No OverStory, thus maximum one soil layer */
		
	if (Height_ov == 0.0) {	
		Z0_Lower = Z0_GROUND;
		d_Lower = 0;
		
		/* No snow */
		U_ov = log((2. + Z0_Lower) / Z0_Lower) / log((Zref - d_Lower) / Z0_Lower);
		Ra_ov = log((2. + Z0_Lower) / Z0_Lower) * log((Zref - d_Lower) / Z0_Lower) / K2;
		U_un = U_ov;
		Ra_un = Ra_ov;

		/* Snow */
		*U2mSnow = log((2. + Z0_SNOW) / Z0_SNOW) / log(Zref / Z0_SNOW);
		RaSnow = log((2. + Z0_SNOW) / Z0_SNOW) * log(Zref / Z0_SNOW) / K2;
	  }

	/* Overstory present, one or two vegetation layers possible */
	else {
		Z0_Upper = Z0_MULTIPLIER * Height_ov;
		d_Upper = D0_MULTIPLIER * Height_ov;

		if (Height_un == 0.0) {
			Z0_Lower = Z0_GROUND;
			d_Lower = 0;
			}
		else {
			Z0_Lower = Z0_MULTIPLIER * Height_un;
			d_Lower = D0_MULTIPLIER * Height_un;
			}
		
		Zw = 1.5 * Height_ov - 0.5 * d_Upper;
		Zt = Trunk * Height_ov;
		/*if (Zt < (Z0_Lower + d_Lower))
			fprintf(stderr,"Trunk space height below \"center\" of lower boundary\n");*/
		
		/* Resistance for overstory */
		Ra_ov = log((Zref - d_Upper) / Z0_Upper) / K2 *
					(Height_ov / (cn * (Zw - d_Upper)) *
					 (exp(cn * (1 - (d_Upper + Z0_Upper) / Height_ov)) - 1) + 
					 (Zw - Height_ov) / (Zw - d_Upper) + 
					 log((Zref - d_Upper) / (Zw - d_Upper)));

		/* Wind at different levels in the profile */
		Uw = log((Zw - d_Upper) / Z0_Upper) / log((Zref - d_Upper) / Z0_Upper);
		Uh = Uw - (1 - (Height_ov - d_Upper) / (Zw - d_Upper)) / 
						log((Zref - d_Upper) / Z0_Upper);
		U_ov = Uh * exp(cn * ((Z0_Upper + d_Upper) / Height_ov - 1.));
		Ut = Uh * exp(cn * (Zt / Height_ov - 1.));
		
		/* Short herbaceous overstory */
		if ((Zt <= ZERO) & (Height_ov <= 2.0)) {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: Herbaceous Overstory Case");
			}
			/* assume this is short herbaceous cover and surface processes have same wind and resistance */
			U_un = U_ov;
			Ra_un = Ra_ov;
			*U2mSnow = U_ov;
			RaSnow = Ra_ov;
		}
		
		else {
		/* resistance at the lower boundary */

		/* No snow */
		/* case 1: the wind profile to a height of 2m above the lower boundary is
		   entirely logarithmic */
		if (Zt > (2. + Z0_Lower + d_Lower)) {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: No Snow Case 1");
			}
			U_un = Ut * log((2. + Z0_Lower) / Z0_Lower) / log((Zt - d_Lower) / Z0_Lower);
			Ra_un = log((2. + Z0_Lower) / Z0_Lower) * log((Zt - d_Lower) / Z0_Lower) / (K2 * Ut);
			}

		/* case 2: the wind profile to a height of 2m above the lower boundary
		   is part logarithmic and part exponential, but the top of the overstory
		   is more than 2 m above the lower boundary */
		else if (Height_ov > (2. + Z0_Lower + d_Lower)) {
			if (verbose_flag == -5) {
				printf("\n          RA No Snow Case 2");
			}
			U_un = Uh * exp(cn * ((2. + Z0_Lower + d_Lower) / Height_ov - 1.));
			Ra_un = log((Zt - d_Lower) / Z0_Lower) * log((Zt - d_Lower) / Z0_Lower) /
					(K2 * Ut) + Height_ov * log((Zref - d_Upper) / Z0_Upper) / 
					(cn * K2 * (Zw - d_Upper)) * (exp(cn * (1 - Zt / Height_ov)) -
					exp(cn * (1 - (Z0_Lower + d_Lower + 2.) / Height_ov)));
			}

		/* case 3: the top of the overstory is less than 2 m above the lower
		   boundary.  The wind profile above the lower boundary is part logarithmic
		   and part exponential, but only extends to the top of the overstory */
		else {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: No Snow Case 3");
			}
			/*if (Zt == 0.0) {
				/* Resistance for overstory */
				/*U_un = Uh;
				/*Ra_un = pow(log((Zref - d_Lower) / Z0_Lower),2) / (K2 * Uh);*/
				/*Ra_un = log((Zref - d_Lower) / Z0_Lower) / K2 *
					(Height_ov / (cn * (Zw - d_Lower)) *
					 (exp(cn * (1 - (d_Lower + Z0_Lower) / Height_ov)) - 1) + 
					 (Zw - Height_ov) / (Zw - d_Lower) + 
					 log((Zref - d_Lower) / (Zw - d_Lower)));
			}
			else {*/
				U_un = min(Uh,Ut);
				Ra_un = log((Zt - d_Lower) / Z0_Lower) * log((Zt - d_Lower) / Z0_Lower) / 
					(K2 * Ut) + Height_ov * log((Zref - d_Upper) / Z0_Upper) / 
					(cn * K2 * (Zw - d_Upper)) * (exp(cn * (1 - Zt / Height_ov)) - 1);
			/*}*/
			/*fprintf(stderr,
				  "WARNING:  Top of overstory is less than 2 meters above the lower boundary\n");*/
			}

		/* Snow */
		/* case 1: the wind profile to a height of 2m above the lower boundary is
		   entirely logarithmic */
		if (Zt > (2. + Z0_SNOW)) {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: Snow Case 1");
			}
			*U2mSnow = Ut * log((2. + Z0_SNOW) / Z0_SNOW) / log(Zt / Z0_SNOW);
			RaSnow = log((2. + Z0_SNOW) / Z0_SNOW) * log(Zt / Z0_SNOW) / (K2 * Ut);
			}

		/* case 2: the wind profile to a height of 2m above the lower boundary
		   is part logarithmic and part exponential, but the top of the overstory
		   is more than 2 m above the lower boundary */
		else if (Height_ov > (2. + Z0_SNOW)) {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: Snow Case 2");
			}
			*U2mSnow = Uh * exp(cn * ((2. + Z0_SNOW) / Height_ov - 1.));
			RaSnow = log(Zt / Z0_SNOW) * log(Zt / Z0_SNOW) /
						(K2 * Ut) + Height_ov * log((Zref - d_Upper) / Z0_Upper) / 
						(cn * K2 * (Zw - d_Upper)) * (exp(cn * (1 - Zt / Height_ov)) -
						exp(cn * (1 - (Z0_SNOW + 2.) / Height_ov)));
			}

		/* case 3: the top of the overstory is less than 2 m above the lower boundary.
		   The wind profile above the lower boundary is part logarithmic and part
		   exponential, but only extends to the top of the overstory */
		else {
			if (verbose_flag == -5) {
				printf("\n          RA OVERSTORY: Snow Case 3");
			}
			/*if (Zt == 0.0) {
				/* Resistance for overstory */
				/* *U2mSnow = Uh;
				RaSnow = pow(log((Zref - d_Lower) / Z0_SNOW),2) / (K2 * Uh);
			}
			else {*/
				*U2mSnow = min(Uh,Ut);
				RaSnow = log(Zt / Z0_SNOW) * log(Zt / Z0_SNOW) /
						(K2 * Ut) + Height_ov * log((Zref - d_Upper) / Z0_Upper) /
						(cn * K2 * (Zw - d_Upper)) * (exp(cn * (1 - Zt / Height_ov)) - 1);
			/*}*/
			/*fprintf(stderr,
				  "WARNING:  Top of overstory is less than 2 meters above the lower boundary\n");*/
			}
			
		} /*endif non-herbaceous short */
		
	} /*endif overstory*/
		
	*U = U_un * Uref;
	*Ucan = U_ov * Uref;
	*U2mSnow = *U2mSnow * Uref;
	
	/* Boundary resistance for leaves */
	/*Rb = 1/(0.01 * pow(Uref/0.001,0.5));*/
	Rb = 0.0;
	
	*ga = 1/(Ra_un / Uref);
	*gasnow = 1/(RaSnow / Uref);
	
	/* NO DEPENDENCE IF WIND SPEED LESS THAN 2, OTHERWISE DEPENDENT ON WIND SPEED */
	/* REMOVE??? */
	/*if (Uref >=2)
		*gasnow = 1/(RaSnow / Uref);
	else
		*gasnow = 1/RaSnow;*/
	ra = (Ra_ov / Uref + Rb);
	
	if (verbose_flag == -5) {
		printf("\n               RA OVERSTORY: Htov=%lf Zref=%lf Zw=%lf Zt=%lf Uref=%lf Ucan=%lf U_un=%lf Usnow=%lf Uw=%lf Uh=%lf Ut=%lf\n               ra_ov=%lf ra_un=%lf ra_snow=%lf", Height_ov, Zref, Zw, Zt, Uref, *Ucan, *U, *U2mSnow, Uw, Uh, Ut, ra, 1.0/(*ga), 1.0/(*gasnow));
	}
	
	return(ra);
}
