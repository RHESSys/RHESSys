/*--------------------------------------------------------------*/
/*                                                              */ 
/*		create_random_distrb					*/
/*                                                              */
/*  NAME                                                        */
/*		create_random_distrb					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  create_random_distrb( struct world_object *world)			*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/* gasdev taken from Numerical Recipes in C			*/
/* World Wide Web sample page from NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5) */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h" 
#include <time.h> 
/*--------------------------------------------------------------*/
/* "Minimal random number generator of Park and Miller with Bays-Durham shuffle and added safeguards.  
/* Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint values).
/* Call with idum a negative integer to initialize; thereafter, do not alther idum between succesive deviates in a sequence
/* RNMX should approximate the largest floating value that is less than 1. */
/*--------------------------------------------------------------*/

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define EDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

float ran1(long *idum)

{
	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	float temp;
	
	if (*idum <= 0 || !iy) {         /*Initialize*/
		if (-(*idum) < 1) *idum=1; /*Be sure to prevent idum=0*/
		else *idum = -(*idum);
		for (j=NTAB+7; j>=0; j--) { /*Load the shuffle table (after 8 warm-ups)*/
			k=(*idum)/IQ;
			*idum=IA*(*idum-k*IQ)-IR*k;
			if (*idum < 0) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}
	k=(*idum)/IQ;		/*Start here when not initializing*/
	*idum=IA*(*idum-k*IQ)-IR*k;	/*Compute idum=(IS*idum) % IM without overflows by Schrage's method*/
	if (*idum < 0) *idum += IM;
	j=iy/EDIV;				/*Will be in the range 0..NTAB-1*/
	iy=iv[j];				/*Output previously stored value and refill the shuffle table*/
	iv[j] = *idum;

	temp = AM*iy;
	if ((temp=AM*iy) > RNMX) return RNMX;	
	else return temp;
}

#include <math.h>
/*--------------------------------------------------------------*/
/*Returns a normally distributed deviate with zero mean and unit variance, using ran1(idum) as the source of uniform deviates*/
/*--------------------------------------------------------------*/

float gasdev ()
{
	static int iset=0;
	static float gset;
	float fac,rsq,v1,v2;
	
		if (iset == 0) {
		do {
 /* We dont have an extra deviate handy, so pick two uniform numbers in the square extending from -1 to +1 in each direction, see if they are in the unit circle*/
			v1=2.0*rand()/(RAND_MAX+1.0)-1.0; 
			v2=2.0*rand()/(RAND_MAX+1.0)-1.0;
			rsq=v1*v1+v2*v2;
			} while (rsq >= 1.0 || rsq == 0.0);		/*and if they are not, try again*/
			fac=sqrt(-2.0*log(rsq)/rsq);
			/*Now make the Box-Muller transformation to get two normal deviates.  Return one and save the other for next time*/
			gset=v1*fac;
			iset=1;				/*set flag*/
			return v2*fac;
		} else {				/*We have an extra deviate handy, so unset the flag and return it*/
			iset=0;
			return gset;
		}
	}
double normdist(double mean, double std)
{
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	double result;

	float gasdev();

	result = gasdev()*std+mean;
	
	return(result);
}


double unifdist(double min, double max)
{
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	double result, range;


    int random_integer = rand(); 
		range = max-min;

	if (range > 0)
		result = random_integer/(RAND_MAX+1.0)*range+min;
	else
		result = 0;

	return(result);
}
