/*--------------------------------------------------------------*/
/* 																*/
/*						basin_hourly.c							*/
/* 																*/
/*	basin_hourly - performs basin cycling for an hour			*/
/*																*/
/*	NAME														*/
/*																*/
/*	basin_hourly - performs basin cycling for an hour			*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	void	basin_hourly( 										*/
/*					struct  world_object    *,					*/
/*					struct  basin_object    *,					*/
/*					struct  command_line_object *,				*/
/*					struct  tec_entry       *,					*/
/*					struct  date );								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on 					*/
/*	hillslopes in the basin.  The routine prints out results 	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	basin_hourly(
					 struct	world_object	*world,
					 struct 	basin_object 	*basin,
					 struct 	command_line_object *command_line,
					 struct	tec_entry		*event,
					 struct	date	current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void    hillslope_hourly (
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void	*alloc(	size_t, char *, char *);

	void    compute_subsurface_routing_hourly(
		struct command_line_object *command_line,
                struct basin_object *basin,
		int n_timesteps, 
		struct date current_date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
    //int	hillslope;
	int	ML;
	int	inx,i;
	double	air_mass_array[22]  =
	{0.0, 2.90,3.05,3.21,3.39, 3.69, 3.82, 4.07, 4.37, 4.72, 5.12,
	5.60,6.18,6.88,7.77,8.90,10.39,12.44,15.36,19.79,26.96,30.00};
	struct	hillslope_object *hillslope_p;
	struct	zone_object *zone_p;
	struct	patch_object *patch;
	/*--------------------------------------------------------------*/
	/*	Initialize the hillslope and zone object.			*/
	/*--------------------------------------------------------------*/
	hillslope_p = basin[0].hillslopes[0];
	zone_p = hillslope_p[0].zones[0];
	
	basin[0].hourly = (struct basin_hourly_object * ) alloc(
		sizeof( struct basin_hourly_object),"hourly","basin_hourly");
	/*--------------------------------------------------------------*/
	/*	Compute basin hourly forcings.								*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Figure out solar geometry and air  mass number even if		*/
	/*	we are reading in radiation's at the zone level.  We do this*/
	/*	sinze we anticipate having few basins so that the cost		*/
	/*	of these computations at the basin level is minor.			*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*  Hour angle (in radians)                                 */
	/*                                                              */
	/*  Eq. 8, Appendix D, "MTCLIM"                             */
	/*                                                             */
	/*  hour_angle =  (hour*3600-43200)*0.0041667*DtoR;         */
	/*                                                              */
	/*  If I understand the equation properly it states that    */
	/*  that at noon (43200 seconds in to the day) the hour     */
	/*  angle is zero (the sun is at zenith).                   */
	/*  Although the solar noon changes with date the maximum   */
	/*  deviation is 16 minutes so we ignore it for now.        */
	/*  e.g. Table A7.1 Appendix &, Jones, "Plants and          */
	/*                                  Microclimate."              */
	/*--------------------------------------------------------------*/
	basin[0].hourly[0].hour_angle
		= (current_date.hour*3600-43200)*0.0041667*DtoR;
	basin[0].hourly[0].cos_hour_angle = cos(basin[0].hourly[0].hour_angle);
	basin[0].hourly[0].sin_hour_angle = sin(basin[0].hourly[0].hour_angle);
	/*--------------------------------------------------------------*/
	/*  Cosine of Solar Zenith Angle (no units)                 */
	/*                                                             */
	/*  Eq. 11, Appendix D, "MTCLIM"                            */
	/*--------------------------------------------------------------*/
	basin[0].hourly[0].cos_sza = world[0].cos_declin
		* basin[0].cos_latitude * basin[0].hourly[0].cos_hour_angle
		+ world[0].sin_declin *	basin[0].sin_latitude;
	
	if ( command_line[0].verbose_flag > 5 )
		printf("\n-111.1 cos_sza= %f cod_declin=%f cos_l= %f coshh=%f sindec=%f sinlat=%f",
		basin[0].hourly[0].cos_sza,world[0].cos_declin,
		basin[0].cos_latitude,basin[0].hourly[0].cos_hour_angle,
		world[0].sin_declin,basin[0].sin_latitude);
	/*--------------------------------------------------------------*/
	/*	We only bother to compute solar geometry if the sun is up 	*/
	/*--------------------------------------------------------------*/
	if ( basin[0].hourly[0].cos_sza > 0 ){
		/*--------------------------------------------------------------*/
		/*		basin day length		(seconds) 						*/
		/*																*/
		/*		increment daylight length by one hour.					*/
		/*	We use the close dform equivalent in basin_daily.c			*/
		/*--------------------------------------------------------------*/
		/*		basin[0].daylength += 3600; */
		/*--------------------------------------------------------------*/
		/*		Optical Air Mass (no units)                             */
		/*                                                              */
		/* 		Eq. 6., Appendix D "MTCLIM"                             */
		/*		Following Eq 6 we adjust based on C code of Rhessys 	*/
		/*			which did not match Appendix D of "MTCLIM"			*/
		/*--------------------------------------------------------------*/
		basin[0].hourly[0].optical_air_mass
			= 1.0	/(basin[0].hourly[0].cos_sza + 1.0e-7);
		if ( basin[0].hourly[0].optical_air_mass > 2.9 ) {
			ML = ( (int) (acos(basin[0].hourly[0].cos_sza)/0.0174533)) - 69;
			if ( ML < 1 ) ML = 1;
			if ( ML > 21 ) ML = 21;
			basin[0].hourly[0].optical_air_mass = air_mass_array[ML];
		}
		/*--------------------------------------------------------------*/
		/*		Precompute 	some angle formulae							*/
		/*--------------------------------------------------------------*/
		basin[0].hourly[0].cos_declin_cos_hourangle
			= world[0].cos_declin * basin[0].hourly[0].cos_hour_angle;
		basin[0].hourly[0].cos_declin_sin_hourangle
			= world[0].cos_declin * basin[0].hourly[0].sin_hour_angle;
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Simulate the hillslopes.		*/
	/*	Note that solar geometry except for cos_sza may be garbage	*/
	/*	if cos_sza < 0 (no daylight).								*/
	/*--------------------------------------------------------------*/
    #pragma omp parallel for
    for (int hillslope=0 ; hillslope < basin[0].num_hillslopes ;hillslope++ ){
		hillslope_hourly(
			world,
			basin,
			basin[0].hillslopes[hillslope],
			command_line,
			event,
			current_date);
	}
	


	
	/*--------------------------------------------------------------*/
	/*	Destroy the basin hourly parameter arrayu.					*/
	/*--------------------------------------------------------------*/
	free( basin[0].hourly );

	/*--------------------------------------------------------------*/
	/*	do subsurface routing					*/
	/*--------------------------------------------------------------*/
	/* this part is nearly the same as in the basin_daily_F		*/

	if ( command_line[0].routing_flag == 1 && zone_p[0].hourly_rain_flag==1) { 
		compute_subsurface_routing_hourly(command_line,
			basin,
			basin[0].defaults[0][0].n_routing_timesteps,
			current_date);
	}


	return;
} /*end basin_hourly.c*/
