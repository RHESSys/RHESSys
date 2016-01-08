/*--------------------------------------------------------------*/
/* 																*/
/*						hillslope_daily_F						*/
/*																*/
/*	NAME														*/
/*	hillslope_daily 											*/
/*				 - performs cycling and output of a hillslope	*/
/*					for end of the day. 						*/	
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void hillslope_daily_F(										*/
/*						 long	,								*/
/*						 struct world_daily_object *,			*/
/*						 struct basin_daily_object *,			*/
/*						 struct hillslope_object *,				*/
/*						 struct command_line_object *,  		*/
/*						 struct tec_entry *,					*/
/*						 struct date)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	basin in the hillslope. The routine also prints out results	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	redistributes water based on saturation deficit				*/
/*	executes zone start of day simulations						*/
/*																*/
/*	Note that if Topmodel is used for redistribution of water	*/
/*	one should be aware that in general TOPMODEL assumes uniform*/
/*	sub-hillslope forcings - i.e. one zone.						*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 7 1997 C. Tague										*/
/* 	- moveed baseflow routine to here from patch level			*/
/*																*/
/*																*/
/*	Sep 15 1997 RAF						*/
/*	Now we only call top model for baseflow if routing is 	*/
/*	not used.						*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void		hillslope_daily_F(
							  long	day,
							  struct	world_object *world,
							  struct	basin_object *basin,
							  struct 	hillslope_object *hillslope,
							  struct 	command_line_object *command_line,
							  struct 	tec_entry *event,
							  struct 	date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void zone_daily_F(
		long,
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	double	top_model(
		int,
		int,
		int,
		double,
		double,
		double,
		struct command_line_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct  zone_object ** ,
		struct	date );
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	i,j,zone;
	double slow_store, fast_store,scale;
	struct patch_object *patch;
	
	#pragma omp parallel for
	for ( zone=0 ; zone<hillslope[0].num_zones; zone++ ){
		zone_daily_F(	day,
			world,
			basin,
			hillslope,
			hillslope[0].zones[zone],
			command_line,
			event,
			current_date );
	}
	/*----------------------------------------------------------------------*/
	/*  baseflow calculations                                               */
	/*----------------------------------------------------------------------*/
	if (command_line[0].routing_flag == 0) {
		hillslope[0].base_flow += top_model(
			command_line[0].verbose_flag,
			command_line[0].grow_flag,
			hillslope[0].defaults[0][0].n_routing_timesteps,
			command_line[0].sen[M],
			command_line[0].sen[K],
			command_line[0].std_scale,
			command_line,
			basin,
			hillslope,
			hillslope[0].zones,
			current_date);
	}
	else{
		hillslope[0].base_flow += 0.0;
	}



	/*----------------------------------------------------------------------*/
	/*	accumulate monthly and yearly streamflow variables		*/
	/*----------------------------------------------------------------------*/
	scale = hillslope[0].area / basin[0].area;
	if((command_line[0].output_flags.monthly == 1)&&(command_line[0].b != NULL)){
		basin[0].acc_month.streamflow += (hillslope[0].base_flow) * scale;
		basin[0].acc_month.stream_NO3 += (hillslope[0].streamflow_NO3) * scale;
		basin[0].acc_month.stream_NH4 += (hillslope[0].streamflow_NH4) * scale;
		basin[0].acc_month.stream_DON += (hillslope[0].streamflow_DON) * scale;
		basin[0].acc_month.stream_DOC += (hillslope[0].streamflow_DOC) * scale;
		}
	if((command_line[0].output_flags.yearly == 1)&&(command_line[0].b != NULL)){
		basin[0].acc_year.streamflow += (hillslope[0].base_flow) * scale;
		basin[0].acc_year.stream_NO3 += (hillslope[0].streamflow_NO3) * scale;
		basin[0].acc_year.stream_NH4 += (hillslope[0].streamflow_NH4) * scale;
		basin[0].acc_year.stream_DON += (hillslope[0].streamflow_DON) * scale;
		basin[0].acc_year.stream_DOC += (hillslope[0].streamflow_DOC) * scale;
		}


	return;
} /*end hillslope_daily_F.c*/
