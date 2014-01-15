/*--------------------------------------------------------------*/
/* 																*/
/*						basin_daily_F							*/
/*																*/
/*	NAME														*/
/*	basin_daily_F												*/
/*			 - performs cycling and output of a basin			*/
/*				for beginning of the  day 						*/
/*				with hillslopes simulated in parallel			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void basin_daily_F( 										*/
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
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void	basin_daily_F(
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
	void hillslope_daily_F(
		long,
		struct	world_object *,
		struct 	basin_object *,
		struct	hillslope_object *,
		struct	command_line_object *, 
		struct	tec_entry *,
		struct	date );

	void	compute_subsurface_routing(
		struct command_line_object *,
		struct basin_object *,
		int, struct	date);
	
	double	compute_stream_routing(
		struct command_line_object *,
		struct stream_network_object *,
		int, struct	date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	h, z, p;
	double	scale;
	struct	hillslope_object *hillslope;
	struct	patch_object *patch; 
	/*--------------------------------------------------------------*/
	/*	Simulate the hillslopes in this basin for the whole day		*/
	/*--------------------------------------------------------------*/
	for ( h = 0 ; h < basin[0].num_hillslopes; h ++ ){
		hillslope_daily_F(	day,
			world,
			basin,
			basin[0].hillslopes[h],
			command_line, 
			event,
			current_date );
	}


	/*--------------------------------------------------------------*/
	/*  For routing option - route water between patches within     */
	/*      the basin                                               */
	/*--------------------------------------------------------------*/
   	 if ( command_line[0].routing_flag == 1) {
		compute_subsurface_routing(command_line,
			basin,
			basin[0].defaults[0][0].n_routing_timesteps,
			current_date);
	}

	/*--------------------------------------------------------------*/
	/*  For stream routing option - route water between patches within     */
	/*      the basin                                               */
	/*--------------------------------------------------------------*/
    	if ( command_line[0].stream_routing_flag == 1) {
		 basin[0].stream_list.streamflow=compute_stream_routing(command_line,
			basin[0].stream_list.stream_network,
			basin[0].stream_list.num_reaches,
                        current_date);
	}

	/*--------------------------------------------------------------*/
	/* update basin patch accumulator				*/
	/*--------------------------------------------------------------*/
	update_basin_patch_accumulator(command_line,
					basin,
					current_date);

	/*--------------------------------------------------------------*/
	/* update hillslope accumulator					*/
	/*--------------------------------------------------------------*/
	update_hillslope_accumulator(command_line,
					basin);

	return;
} /*end basin_daily_F*/
