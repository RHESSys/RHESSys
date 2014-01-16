/*--------------------------------------------------------------*/
/* 																*/
/*						patch_hourly							*/
/*																*/
/*	NAME														*/
/*	patch_hourly 												*/
/*				 - performs cycling and output of a patch		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void patch_hourly(											*/
/*						struct	world_object *,   				*/	
/*						struct	basin_object *,   				*/	
/*						struct	hillslope_object *,				*/	
/*						struct	zone_object *,   				*/	
/*						struct 	patch_object *,					*/
/*						struct 	command_line_object *,			*/
/*						struct  tec_entry   *,					*/
/*						struct  date );							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	canopy_stata in the patch.									*/ 
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include "rhessys.h"

void		patch_hourly(
						 struct world_object *world,
						 struct basin_object *basin,
						 struct hillslope_object *hillslope,
						 struct zone_object *zone,
						 struct patch_object *patch,
						 struct command_line_object *command_line,
						 struct	tec_entry	*event,
						 struct	date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void   canopy_stratum_hourly (
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct patch_object *,
		struct canopy_strata_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	double	compute_infiltration(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	/*--------------------------------------------------------------*/
	/*	Local Variable Declarations.								*/
	/*--------------------------------------------------------------*/
	int	stratum;
	int	layer;
	double  net_inflow, duration, infiltration;
	/*--------------------------------------------------------------*/
	/*	process any hourly rainfall				*/
	/*--------------------------------------------------------------*/
	if ( zone[0].hourly_rain_flag == 1)
		patch[0].hourly[0].rain_throughfall = zone[0].hourly[0].rain;
	else
		patch[0].hourly[0].rain_throughfall = 0.0;
	/*--------------------------------------------------------------*/
	/*	Cycle through the canopy strata								*/
	/*	above the snowpack					*/
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		if ( (patch[0].layers[layer].height > patch[0].snowpack.height) ){
			patch[0].rain_throughfall_final = 0.0;
			for (stratum=0 ;stratum<patch[0].layers[layer].count; stratum++ ){
				canopy_stratum_hourly(
					world,
					basin,
					hillslope,
					zone,
					patch,
					patch[0].canopy_strata[stratum],
					command_line,
					event,
					current_date );
			}
			/*--------------------------------------------------------------*/
			/*	process any hourly throughfallthat falls on a snowpack */
			/*--------------------------------------------------------------*/
			patch[0].hourly[0].rain_throughfall = patch[0].rain_throughfall_final;
		}
	}
	if (patch[0].snowpack.water_equivalent_depth > 0.0) {
		patch[0].snowpack.water_equivalent_depth
			+= patch[0].hourly[0].rain_throughfall;
		patch[0].hourly[0].rain_throughfall = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	Cycle through the canopy strata								*/
	/*	below the snowpack					*/
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		if ( (patch[0].layers[layer].height <= patch[0].snowpack.height) ){
			patch[0].rain_throughfall_final = 0.0;
			for ( stratum=0;stratum<patch[0].layers[layer].count; stratum++ ){
				canopy_stratum_hourly(
					world,
					basin,
					hillslope,
					zone,
					patch,
					patch[0].canopy_strata[stratum],
					command_line,
					event,
					current_date );
			}
		}
		patch[0].hourly[0].rain_throughfall = patch[0].rain_throughfall_final;
	}
	/*--------------------------------------------------------------*/
	/*	include any detention storage as throughfall		*/
	/*--------------------------------------------------------------*/
	if (zone[0].hourly_rain_flag == 1) {

	/*--------------------------------------------------------------*/
	/* 	Above ground Hydrologic Processes			*/
	/* 	compute infiltration into the soil			*/
	/*	from snowmelt or rain_throughfall			*/
	/*	for now assume that all water infilatrates		*/
	/*--------------------------------------------------------------*/
	net_inflow = patch[0].hourly[0].rain_throughfall;
	if (net_inflow > 0.0) {
		/*--------------------------------------------------------------*/
		/*      - if rain duration is zero, then input is from snow     */
		/*      melt  assume full daytime duration                      */
		/*--------------------------------------------------------------*/
		if (zone[0].hourly[0].rain_duration <= ZERO)
			duration = 60*60/(86400);
		else
			duration = zone[0].hourly[0].rain_duration/(86400);
		
		infiltration = compute_infiltration(
			command_line[0].verbose_flag,
			patch[0].sat_deficit_z,
			patch[0].rootzone.S,
			patch[0].Ksat_vertical,
			patch[0].soil_defaults[0][0].Ksat_0_v,
			patch[0].soil_defaults[0][0].mz_v,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			net_inflow,
			duration,
			patch[0].soil_defaults[0][0].psi_air_entry);
			
		//printf("hourly patch called \n");
	}
	else infiltration = 0.0;

	if (infiltration < 0.0)
		printf("\nInfiltration %lf < 0 for %d on %d",
			infiltration,
			patch[0].ID, current_date.day);
	/*--------------------------------------------------------------*/
	/* determine fate of hold infiltration excess in detention store */
	/* infiltration excess will removed during routing portion	*/
	/*--------------------------------------------------------------*/
	patch[0].detention_store = (net_inflow - infiltration);
				
	if ( infiltration > patch[0].sat_deficit - patch[0].unsat_storage - patch[0].rz_storage){
		/*--------------------------------------------------------------*/
		/*		Yes the unsat zone will be filled so we may	*/
		/*		as well treat the unsat_storage and infiltration*/
		/*		as water added to the water table.		*/
		/*--------------------------------------------------------------*/
		patch[0].sat_deficit -= (infiltration + patch[0].unsat_storage + patch[0].rz_storage);
		/*--------------------------------------------------------------*/
		/*		There is no unsat_storage left.			*/
		/*--------------------------------------------------------------*/
		patch[0].unsat_storage = 0;
		patch[0].rz_storage = 0;
		patch[0].field_capacity = 0;
		patch[0].rootzone.field_capacity =0;
		/*--------------------------------------------------------------*/
		/*		Reverse the cap rise as it likely did not happen*/
		/*--------------------------------------------------------------*/
		patch[0].potential_cap_rise += patch[0].cap_rise;
		patch[0].cap_rise = 0;
	}
	else if ((patch[0].sat_deficit_z > patch[0].rootzone.depth) &&
		(infiltration > patch[0].rootzone.potential_sat - patch[0].rz_storage)) {
		/*----------------------------------------------------------------------*/
		/*		Just add the infiltration remains to the unsat_storage	*/
		/*----------------------------------------------------------------------*/
		patch[0].unsat_storage += infiltration - (patch[0].rootzone.potential_sat - patch[0].rz_storage);
		patch[0].rz_storage = patch[0].rootzone.potential_sat;
	}
	else if ((patch[0].sat_deficit_z > patch[0].rootzone.depth) &&
		(infiltration <= patch[0].rootzone.potential_sat - patch[0].rz_storage)) {
		/*--------------------------------------------------------------*/
		/*		Just add the infiltration to the rz_storage	*/
		/*--------------------------------------------------------------*/
		patch[0].rz_storage += infiltration;
	}
	else if ((patch[0].sat_deficit_z <= patch[0].rootzone.depth) &&
		(infiltration <= patch[0].sat_deficit - patch[0].rz_storage)) {
		patch[0].rz_storage += patch[0].unsat_storage;		/* transfer left water in unsat storage to rootzone layer */
		patch[0].unsat_storage = 0;
		patch[0].rz_storage += infiltration;
		patch[0].field_capacity = 0;
	}

	} /* end if rain throughfall */
	/*--------------------------------------------------------------*/
	/*	Destroy the patch hourly object.							*/
	/*--------------------------------------------------------------*/
	return;
} /*end patch_hourly.c*/
