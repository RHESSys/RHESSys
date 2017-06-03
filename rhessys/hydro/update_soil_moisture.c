/*--------------------------------------------------------------*/
/* 																*/
/*		 		update_soil_moisture							*/
/*																*/
/*	update_soil_moisture - updates soil moisture in a patch		*/
/*																*/
/*	NAME														*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "phys_constants.h"
#include "rhessys.h"

void	update_soil_moisture(
					  int	verbose_flag,
					  double	infiltration,
					  double	net_inflow,
					  struct	patch_object	*patch,
					  struct 	command_line_object *command_line,
					  struct	date 			current_date) 
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/* allow infiltration of surface N				*/
	/*--------------------------------------------------------------*/
	if ((command_line[0].grow_flag > 0) && (infiltration > ZERO)) {
		patch[0].soil_ns.nitrate += infiltration / net_inflow * patch[0].surface_NO3;
		patch[0].soil_ns.sminn += infiltration / net_inflow * patch[0].surface_NH4;
		patch[0].surface_NO3 -= infiltration / net_inflow * patch[0].surface_NO3;
		patch[0].surface_NH4 -= infiltration / net_inflow * patch[0].surface_NH4;
		patch[0].soil_ns.DON += infiltration / net_inflow * patch[0].surface_DON;
		patch[0].soil_cs.DOC += infiltration / net_inflow * patch[0].surface_DOC;
		patch[0].surface_DOC -= infiltration /net_inflow * patch[0].surface_DOC;
		patch[0].surface_DON -= infiltration /net_inflow * patch[0].surface_DON;
		}
	

	if ( command_line[0].verbose_flag > 1 )
		printf("\n%4d %2d %2d  -333.1 ",
		current_date.year, current_date.month, current_date.day);
	/*--------------------------------------------------------------*/
	/*	Determine if the infifltration will fill up the unsat	*/
	/*	zone or not.						*/
	/*	We use the strict assumption that sat deficit is the	*/
	/*	amount of water needed to saturate the soil.		*/
	/*--------------------------------------------------------------*/
	if ( infiltration > patch[0].sat_deficit - patch[0].unsat_storage - patch[0].rz_storage) {
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
		/*--------------------------------------------------------------*/
		/*		Reverse the cap rise as it likely did not happen*/
		/*--------------------------------------------------------------*/
		patch[0].potential_cap_rise += patch[0].cap_rise;
		patch[0].cap_rise = 0;
	}									
	else if ((patch[0].sat_deficit > patch[0].rootzone.potential_sat) &&
		(infiltration > patch[0].rootzone.potential_sat - patch[0].rz_storage)) {
		/*------------------------------------------------------------------------------*/
		/*		Just add the infiltration to the rz_storage and unsat_storage	*/
		/*------------------------------------------------------------------------------*/
		patch[0].unsat_storage += infiltration - (patch[0].rootzone.potential_sat - patch[0].rz_storage);
		patch[0].rz_storage = patch[0].rootzone.potential_sat;
	}								
		/* Only rootzone layer saturated - perched water table case */
	else if ((patch[0].sat_deficit > patch[0].rootzone.potential_sat) &&
		(infiltration <= patch[0].rootzone.potential_sat - patch[0].rz_storage)) {
		/*--------------------------------------------------------------*/
		/*		Just add the infiltration to the rz_storage	*/
		/*--------------------------------------------------------------*/
		patch[0].rz_storage += infiltration;
	}
	else if ((patch[0].sat_deficit <= patch[0].rootzone.potential_sat) &&
		(infiltration <= patch[0].sat_deficit - patch[0].rz_storage - patch[0].unsat_storage)) {
		patch[0].rz_storage += patch[0].unsat_storage;		/* transfer left water in unsat storage to rootzone layer */
		patch[0].unsat_storage = 0;
		patch[0].rz_storage += infiltration;
		patch[0].field_capacity = 0;
	}
	
} /*end update_soil_moisture*/
