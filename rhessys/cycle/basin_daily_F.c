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


	for ( h = 0 ; h < basin[0].num_hillslopes; h++ ){
		hillslope = basin[0].hillslopes[h];
		for (z = 0; z < hillslope[0].num_zones; z++) {
			for (p=0; p < hillslope[0].zones[z][0].num_patches; p++) {
				patch = hillslope[0].zones[z][0].patches[p];

		scale = patch[0].area / hillslope[0].area;

		if((command_line[0].output_flags.monthly == 1)&&(command_line[0].h != NULL)){
			hillslope[0].acc_month.snowpack += (patch[0].snowpack.water_equivalent_depth) * scale;
			hillslope[0].acc_month.streamflow += (patch[0].streamflow) * scale;
			hillslope[0].acc_month.et += (patch[0].transpiration_unsat_zone
				+ patch[0].evaporation_surf + 
				patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone +
				patch[0].transpiration_sat_zone + patch[0].evaporation)*scale;
			hillslope[0].acc_month.denitrif += patch[0].ndf.denitrif*scale;
			hillslope[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate*scale;
			hillslope[0].acc_month.mineralized += patch[0].ndf.net_mineralized*scale;
			hillslope[0].acc_month.uptake += patch[0].ndf.sminn_to_npool*scale;
			hillslope[0].acc_month.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
			hillslope[0].acc_month.DON_loss+= patch[0].ndf.total_DON_loss * scale;
			hillslope[0].acc_month.length += 1;
			hillslope[0].acc_month.stream_NO3 += patch[0].streamflow_N * scale;
			hillslope[0].acc_month.psn += patch[0].net_plant_psn * scale;
			hillslope[0].acc_month.lai += patch[0].lai * scale;
		}
		if((command_line[0].output_flags.yearly == 1)&&(command_line[0].h != NULL)){
			hillslope[0].acc_year.length += 1;
			hillslope[0].acc_year.stream_NO3 += patch[0].streamflow_N * scale;
			hillslope[0].acc_year.denitrif += patch[0].ndf.denitrif * scale;
			hillslope[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate*scale;
			hillslope[0].acc_year.mineralized += patch[0].ndf.net_mineralized*scale;
			hillslope[0].acc_year.uptake += patch[0].ndf.sminn_to_npool*scale;
			hillslope[0].acc_year.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
			hillslope[0].acc_year.DON_loss += patch[0].ndf.total_DON_loss * scale;
			hillslope[0].acc_year.psn += patch[0].net_plant_psn * scale;
			hillslope[0].acc_year.et += (patch[0].evaporation 
				+ patch[0].evaporation_surf + 
				patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone +
				patch[0].transpiration_unsat_zone + patch[0].transpiration_sat_zone)
						* scale;
			hillslope[0].acc_year.streamflow += (patch[0].streamflow)*scale;
			hillslope[0].acc_year.lai += patch[0].lai * scale;
			}

		} /* end patch */
		} /* end zone */
	} /* end hillslope */

	return;
} /*end basin_daily_F*/
