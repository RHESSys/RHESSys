/*--------------------------------------------------------------*/
/* 																*/
/*						basin_daily_I							*/
/*																*/
/*	NAME														*/
/*	basin_daily_I												*/
/*			 - performs cycling and output of a basin			*/
/*				for beginning of the  day 						*/
/*				with hillslopes simulated in parallel			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void basin_daily_I( 										*/
/*						 long	,								*/
/*						 struct world_object *,					*/
/*						 struct basin_object *,					*/
/*						 struct command_line_object *,			*/
/*						 struct tec_entry *,					*/
/*						 struct date)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	hillslopes in the basin. The routine also prints out results*/
/*	where specified by current tec events files.				*/
/*																*/
/*	Computes day length at the latitude of the basin.			*/
/*	Executes hillslope daily simulation for all hillslopes. 	*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*	The execution of simulate_hillslope_daily could be done in 	*/
/*	parallel with one processor per hillslope.  				*/
/*																*/
/*	If message passing is required during the simulate_hillslope*/
/*	_daily processes a message structure could be created in 	*/
/*	the basin_daily array.										*/
/*																*/
/*	March 14, 1997 - RAF					*/
/*	Added computation of theta_noon for radiation	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

void		basin_daily_I(
						  long	day,
						  struct	world_object	*world,
						  struct	basin_object 	*basin,
						  struct	command_line_object *command_line,
						  struct	tec_entry		*event,
						  struct	date			current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void hillslope_daily_I(
		long,
		struct	world_object *,
		struct 	basin_object *,
		struct	hillslope_object *,
		struct	command_line_object *,
		struct	tec_entry *,
		struct	date );
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double	cosegeom;
	double	sinegeom;
	double	coshss;
	double	hss;
    //160420LML int	hillslope;
	/*--------------------------------------------------------------*/
	/*	Daylength (seconds)											*/
	/*	Now using GBGC computation taken from Jones,		*/
	/*	Plans and Microclimate.					*/
	/*	Note that I am assuming this is the time when the solar		*/
	/*		zenith angle has a cosine >0.   						*/
	/*	Note that we may want to consider daylengths with both		*/
	/*		diffuse and direct irradiance and only diffuse irradianc*/
	/*		This can be computed using the zone level horizons.		*/
	/*--------------------------------------------------------------*/
	cosegeom = cos(basin[0].latitude*DtoR) * world[0].cos_declin;
	sinegeom = sin(basin[0].latitude*DtoR) * world[0].sin_declin;
	coshss = -(sinegeom) / cosegeom;
	if (coshss < -1.0) coshss = -1.0; /* 24-hr daylight */
	if (coshss > 1.0) coshss = 1.0;   /* 0-hr daylight */
	hss = acos(coshss);  /* hour angle at sunset (radians) */
	basin[0].daylength= 2.0 * hss * SECPERRAD;
	/*--------------------------------------------------------------*/
	/*	Find solar zenith angle at noon (not elevation angle)	*/
	/*	Form Linacre.						*/
	/*--------------------------------------------------------------*/
	basin[0].theta_noon =  basin[0].latitude*DtoR - world[0].declin;
	/*--------------------------------------------------------------*/
	/*	Simulate the hillslopes in this basin for the whole day		*/
	/*--------------------------------------------------------------*/
    #pragma omp parallel for
    for (int hillslope = 0 ; hillslope < basin[0].num_hillslopes; hillslope ++ ){
		hillslope_daily_I(
			day,
			world,
			basin,
			basin[0].hillslopes[hillslope],
			command_line,
			event,
			current_date );
	}
	return;
} /*end basin_daily_I.c*/
