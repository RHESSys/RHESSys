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
	double gw_Qout,gw_Qout_ratio;
	struct patch_object *patch;
	
	
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
		hillslope[0].base_flow = top_model(
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
		hillslope[0].base_flow = 0.0;
	}

	/*----------------------------------------------------------------------*/
	/*	compute groundwater losses					*/
	/* 	updated to consider two possible deeper concept gw models	*/
	/*  see review in Stoelzie e tal, 2014, Hydrological Processes		*/
	/*----------------------------------------------------------------------*/
	if ((command_line[0].gw_flag > 0) && (hillslope[0].gw.storage > ZERO) && (command_line[0].gwtoriparian_flag==0)) {
	
	
	/*	if (hillslope[0].defaults[0][0].gw_loss_fast_threshold < ZERO) {	
			hillslope[0].gw.Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
					hillslope[0].defaults[0][0].gw_loss_coeff;
		}
		else {
			slow_store = min(hillslope[0].defaults[0][0].gw_loss_fast_threshold, hillslope[0].gw.storage);
			hillslope[0].gw.Qout = slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_coeff; 
			fast_store = max(0.0,hillslope[0].gw.storage - hillslope[0].defaults[0][0].gw_loss_fast_threshold);
			hillslope[0].gw.Qout += slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_fast_coeff; 
			}

	*/
		hillslope[0].gw.NH4out = hillslope[0].gw.Qout * hillslope[0].gw.NH4 / hillslope[0].gw.storage;
		hillslope[0].gw.NO3out = hillslope[0].gw.Qout * hillslope[0].gw.NO3 / hillslope[0].gw.storage;
		hillslope[0].gw.DONout = hillslope[0].gw.Qout * hillslope[0].gw.DON / hillslope[0].gw.storage;
		hillslope[0].gw.DOCout = hillslope[0].gw.Qout * hillslope[0].gw.DOC / hillslope[0].gw.storage;

		hillslope[0].streamflow_NO3 += hillslope[0].gw.NO3out;
		hillslope[0].streamflow_NH4 += hillslope[0].gw.NH4out;
		hillslope[0].streamflow_DON += hillslope[0].gw.DONout;
		hillslope[0].streamflow_DOC += hillslope[0].gw.DOCout;
		hillslope[0].base_flow += hillslope[0].gw.Qout;
		//hillslope[0].gw.storage -= hillslope[0].gw.Qout; the storage has been updated in hillslop hourly already
		hillslope[0].gw.NH4 -= hillslope[0].gw.NH4out;
		hillslope[0].gw.NO3 -= hillslope[0].gw.NO3out;
		hillslope[0].gw.DON -= hillslope[0].gw.DONout;
		hillslope[0].gw.DOC -= hillslope[0].gw.DOCout;
		}


	if ((command_line[0].gw_flag > 0) && (hillslope[0].gw.storage > ZERO) && (command_line[0].gwtoriparian_flag == 1)) {

	/*	hillslope[0].gw.Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
					hillslope[0].defaults[0][0].gw_loss_coeff;


		if (hillslope[0].defaults[0][0].gw_loss_fast_threshold < ZERO) {	
			hillslope[0].gw.Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
					hillslope[0].defaults[0][0].gw_loss_coeff;
		}
		else {
			slow_store = min(hillslope[0].defaults[0][0].gw_loss_fast_threshold, hillslope[0].gw.storage);
			hillslope[0].gw.Qout = slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_coeff; 
			fast_store = max(0.0,hillslope[0].gw.storage - hillslope[0].defaults[0][0].gw_loss_fast_threshold);
			hillslope[0].gw.Qout += slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_fast_coeff; 
			}
	*/
		hillslope[0].gw.NH4out = hillslope[0].gw.Qout * hillslope[0].gw.NH4 / hillslope[0].gw.storage;
		hillslope[0].gw.NO3out = hillslope[0].gw.Qout * hillslope[0].gw.NO3 / hillslope[0].gw.storage;
		hillslope[0].gw.DONout = hillslope[0].gw.Qout * hillslope[0].gw.DON / hillslope[0].gw.storage;
		hillslope[0].gw.DOCout = hillslope[0].gw.Qout * hillslope[0].gw.DOC / hillslope[0].gw.storage;

		gw_Qout_ratio = hillslope[0].gw.Qout/hillslope[0].gw.storage;


		if (hillslope[0].riparian_area > ZERO)
			gw_Qout = hillslope[0].gw.Qout * hillslope[0].area/hillslope[0].riparian_area;
		else {
			hillslope[0].streamflow_NO3 += hillslope[0].gw.NO3out;
			hillslope[0].streamflow_NH4 += hillslope[0].gw.NH4out;
			hillslope[0].streamflow_DON += hillslope[0].gw.DONout;
			hillslope[0].streamflow_DOC += hillslope[0].gw.DOCout;
			hillslope[0].base_flow += hillslope[0].gw.Qout;
			gw_Qout = 0.0;
		}

		for ( i=0 ; i<hillslope[0].num_zones ; i++ ){
			for	 (j =0; j < hillslope[0].zones[i][0].num_patches ; j++) {
				patch = hillslope[0].zones[i][0].patches[j];
				if (patch[0].soil_defaults[0][0].ID == 42) {
						patch[0].sat_deficit -= gw_Qout;
						patch[0].soil_ns.sminn += gw_Qout * gw_Qout_ratio * hillslope[0].gw.NH4;
						patch[0].soil_ns.nitrate += gw_Qout * gw_Qout_ratio * hillslope[0].gw.NO3;
						patch[0].soil_ns.DON += gw_Qout * gw_Qout_ratio * hillslope[0].gw.DON;
						patch[0].soil_cs.DOC += gw_Qout * gw_Qout_ratio * hillslope[0].gw.DOC;
						}
			}
		}


		//hillslope[0].gw.storage -= hillslope[0].gw.Qout;
		hillslope[0].gw.NH4 -= hillslope[0].gw.NH4out;
		hillslope[0].gw.NO3 -= hillslope[0].gw.NO3out;
		hillslope[0].gw.DON -= hillslope[0].gw.DONout;
		hillslope[0].gw.DOC -= hillslope[0].gw.DOCout;


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
