/*--------------------------------------------------------------*/
/* 																*/
/*						hillslope_hourly						*/
/*																*/
/*	NAME														*/
/*	hillslope_hourly 											*/
/*				 - performs cycling and output of a hillslope	*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void hillslope_hourly(										*/
/*					struct	world_object		*,				*/
/*					struct	basin_object		*,				*/
/*					struct 	hillslope_object 	*,				*/
/*					struct 	command_line_object *,				*/
/*					struct 	tec_entry 			*,				*/
/*					struct 	date 				);				*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	hillslope.			  The routine also prints out results	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 9, 1997 C. Tague										*/
/*	- included if/print statement to get rid of random error	*/
/*		which changed the value of zone.Kdown_diffuse			*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void		hillslope_hourly(
							 struct	world_object		*world,
							 struct	basin_object		*basin,
							 struct 	hillslope_object 	*hillslope,
							 struct 	command_line_object *command_line,
							 struct 	tec_entry 			*event,
							 struct 	date 				current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void zone_hourly (
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	zone, i, j;
	double	slow_store, fast_store;
	double  hourly_gw_Qout;
	double	gw_Qout_ratio;
	struct patch_object *patch;
	
	/*--------------------------------------------------------------*/
	/*	Allocate the hillslope houly parameter array.				*/
	/*--------------------------------------------------------------*/
	if ( (hillslope[0].hourly = (struct hillslope_hourly_object *)
		calloc(1,sizeof(struct hillslope_hourly_object))) == NULL ){
		fprintf(stderr,"FATAL ERROR: in hillslope_hourly\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/* do redistribution of saturated zone at patch level based on 	*/
	/* previous time steps hillslope level soilwater 				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/* an alternative to TOPMODEL could go here (with a flag)		*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Simulate zones requested.								*/
	/*--------------------------------------------------------------*/
	for ( zone=0 ; zone < hillslope[0].num_zones ; zone++ ){
		if ( hillslope[0].zones[zone][0].Kdown_diffuse > 1.0e100)
			printf("\n Date %d %d %d %d is Zone Hourly Diffuse is %10.6f",
			current_date.year,
			current_date.month,
			current_date.day,
			current_date.hour,
			hillslope[0].zones[zone][0].Kdown_diffuse);
		zone_hourly(
			world,
			basin,
			hillslope,
			hillslope[0].zones[zone],
			command_line,
			event,
			current_date );
	}
	/*--------------------------------------------------------------*/
	/*	Destroy the hillslope hourloy object.						*/
	/*--------------------------------------------------------------*/
	free( hillslope[0].hourly );
	/*----------------------------------------------------------------------*/
	/*	compute groundwater losses					*/
	/*	this part is transplanted from hillslope_daily_F.c	    	*/
	/*----------------------------------------------------------------------*/
	hillslope[0].hourly_base_flow = 0.0;
	hillslope[0].gw.hourly_Qout = 0.0;
	hillslope[0].gw.hourly_NO3out = 0.0;
	hillslope[0].gw.hourly_NH4out = 0.0;
	hillslope[0].gw.hourly_DONout = 0.0;
	hillslope[0].gw.hourly_DOCout = 0.0;
	hillslope[0].hourly_streamflow_NO3 = 0.0;
	hillslope[0].hourly_streamflow_NH4 = 0.0;
	hillslope[0].hourly_streamflow_DOC = 0.0;
	hillslope[0].hourly_streamflow_DON = 0.0;

	if ((command_line[0].gw_flag > 0) && (hillslope[0].gw.storage > ZERO) && (command_line[0].gwtoriparian_flag==0)) {
	    if (hillslope[0].defaults[0][0].gw_loss_fast_threshold < ZERO) {	
		      hillslope[0].gw.hourly_Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
				  hillslope[0].defaults[0][0].gw_loss_coeff / 24;
	    }
	    else {
		slow_store = min(hillslope[0].defaults[0][0].gw_loss_fast_threshold, hillslope[0].gw.storage);
	      	hillslope[0].gw.hourly_Qout = slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_coeff/24; 
		fast_store = max(0.0,hillslope[0].gw.storage - hillslope[0].defaults[0][0].gw_loss_fast_threshold);
		hillslope[0].gw.hourly_Qout += fast_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_fast_coeff/24; 
	    }

		hillslope[0].hourly_base_flow += hillslope[0].gw.hourly_Qout;
		
		hillslope[0].gw.hourly_NH4out = hillslope[0].gw.hourly_Qout * hillslope[0].gw.NH4 / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_NO3out = hillslope[0].gw.hourly_Qout * hillslope[0].gw.NO3 / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_DONout = hillslope[0].gw.hourly_Qout * hillslope[0].gw.DON / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_DOCout = hillslope[0].gw.hourly_Qout * hillslope[0].gw.DOC / hillslope[0].gw.storage;
	      	hillslope[0].gw.NO3out += hillslope[0].gw.hourly_NO3out;
		hillslope[0].gw.NH4out += hillslope[0].gw.hourly_NH4out;
		hillslope[0].gw.DOCout += hillslope[0].gw.hourly_DOCout;
		hillslope[0].gw.DONout += hillslope[0].gw.hourly_DONout;

		hillslope[0].gw.storage -= hillslope[0].gw.hourly_Qout;


		hillslope[0].hourly_streamflow_NO3 += hillslope[0].gw.hourly_NO3out;
		hillslope[0].hourly_streamflow_NH4 += hillslope[0].gw.hourly_NH4out;
		hillslope[0].hourly_streamflow_DON += hillslope[0].gw.hourly_DONout;
		hillslope[0].hourly_streamflow_DOC += hillslope[0].gw.hourly_DOCout;
		hillslope[0].gw.NH4 -= hillslope[0].gw.hourly_NH4out;
		hillslope[0].gw.NO3 -= hillslope[0].gw.hourly_NO3out;
		hillslope[0].gw.DON -= hillslope[0].gw.hourly_DONout;
		hillslope[0].gw.DOC -= hillslope[0].gw.hourly_DOCout;

		hillslope[0].streamflow_NO3 +=hillslope[0].hourly_streamflow_NO3;
		hillslope[0].streamflow_NH4 +=hillslope[0].hourly_streamflow_NH4;
		hillslope[0].streamflow_DOC +=hillslope[0].hourly_streamflow_DOC;
		hillslope[0].streamflow_DON +=hillslope[0].hourly_streamflow_DON;

	}
	
	if ((command_line[0].gw_flag > 0) && (hillslope[0].gw.storage > ZERO) && (command_line[0].gwtoriparian_flag == 1)) {
		hillslope[0].gw.hourly_Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
					hillslope[0].defaults[0][0].gw_loss_coeff / 24;


		if (hillslope[0].defaults[0][0].gw_loss_fast_threshold < ZERO) {	
			hillslope[0].gw.hourly_Qout = hillslope[0].gw.storage * hillslope[0].slope / 1.571 * 
					hillslope[0].defaults[0][0].gw_loss_coeff / 24;
		}
		else {
			slow_store = min(hillslope[0].defaults[0][0].gw_loss_fast_threshold, hillslope[0].gw.storage);
			hillslope[0].gw.hourly_Qout = slow_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_coeff/24; 
			fast_store = max(0.0,hillslope[0].gw.storage - hillslope[0].defaults[0][0].gw_loss_fast_threshold);
			hillslope[0].gw.hourly_Qout += fast_store * hillslope[0].slope / 1.571 * hillslope[0].defaults[0][0].gw_loss_fast_coeff/24; 
	      	}

		hillslope[0].gw.hourly_NH4out = hillslope[0].gw.hourly_Qout * hillslope[0].gw.NH4 / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_NO3out = hillslope[0].gw.hourly_Qout * hillslope[0].gw.NO3 / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_DONout = hillslope[0].gw.hourly_Qout * hillslope[0].gw.DON / hillslope[0].gw.storage;
		hillslope[0].gw.hourly_DOCout = hillslope[0].gw.hourly_Qout * hillslope[0].gw.DOC / hillslope[0].gw.storage;

		gw_Qout_ratio = hillslope[0].gw.hourly_Qout/hillslope[0].gw.storage;

		if (hillslope[0].riparian_area > ZERO){
			hourly_gw_Qout = hillslope[0].gw.hourly_Qout * hillslope[0].area / hillslope[0].riparian_area;
		}
		else {
			hillslope[0].hourly_streamflow_NO3 += hillslope[0].gw.hourly_NO3out;
			hillslope[0].hourly_streamflow_NH4 += hillslope[0].gw.hourly_NH4out;
			hillslope[0].hourly_streamflow_DON += hillslope[0].gw.hourly_DONout;
			hillslope[0].hourly_streamflow_DOC += hillslope[0].gw.hourly_DOCout;
			
			hillslope[0].hourly_base_flow += hillslope[0].gw.hourly_Qout;
			hourly_gw_Qout = 0.0;
		}

  		for ( i=0 ; i<hillslope[0].num_zones ; i++ ){
			for	 (j =0; j < hillslope[0].zones[i][0].num_patches ; j++) {
				patch = hillslope[0].zones[i][0].patches[j];
				if (patch[0].soil_defaults[0][0].ID == 42) {
						patch[0].sat_deficit -= hourly_gw_Qout;
						patch[0].soil_ns.sminn += hourly_gw_Qout * gw_Qout_ratio * hillslope[0].gw.NH4;
						patch[0].soil_ns.nitrate += hourly_gw_Qout * gw_Qout_ratio * hillslope[0].gw.NO3;
						patch[0].soil_ns.DON += hourly_gw_Qout * gw_Qout_ratio * hillslope[0].gw.DON;
						patch[0].soil_cs.DOC += hourly_gw_Qout * gw_Qout_ratio * hillslope[0].gw.DOC;
						}
			}
		}


		hillslope[0].gw.storage -= hillslope[0].gw.hourly_Qout;	
		hillslope[0].gw.NH4 -= hillslope[0].gw.hourly_NH4out;
		hillslope[0].gw.NO3 -= hillslope[0].gw.hourly_NO3out;
		hillslope[0].gw.DON -= hillslope[0].gw.hourly_DONout;
		hillslope[0].gw.DOC -= hillslope[0].gw.hourly_DOCout;
	      	hillslope[0].gw.NO3out += hillslope[0].gw.hourly_NO3out;
		hillslope[0].gw.NH4out += hillslope[0].gw.hourly_NH4out;
		hillslope[0].gw.DOCout += hillslope[0].gw.hourly_DOCout;
		hillslope[0].gw.DONout += hillslope[0].gw.hourly_DONout;

		hillslope[0].streamflow_NO3 +=hillslope[0].hourly_streamflow_NO3;
		hillslope[0].streamflow_NH4 +=hillslope[0].hourly_streamflow_NH4;
		hillslope[0].streamflow_DOC +=hillslope[0].hourly_streamflow_DOC;
		hillslope[0].streamflow_DON +=hillslope[0].hourly_streamflow_DON;

	}
	hillslope[0].gw.Qout += hillslope[0].gw.hourly_Qout; // this is the daily gw.Qout, used in hillslop_daily_F
	hillslope[0].base_flow += hillslope[0].hourly_base_flow; // daily base_flow
 


} /*end hillslope_hourly.c*/
