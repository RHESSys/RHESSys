
/*--------------------------------------------------------------*/
/* 																*/
/*					compute_mean_hillslope_parameters							*/
/*																*/
/*	compute_mean_hillslope_parameters.c - creates a hillslope object			*/
/*																*/
/*	NAME														*/
/*	compute_mean_hillslope_parameters.c - creates a hillslope object			*/
/*																*/
/*	SYNOPSIS													*/
/* 	void compute_mean_hillslope_parameters(									*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file,								*/
/*			int		num_world_base_stations,					*/
/*			struct	base_station_object	**world_base_stations,	*/
/*			struct	hillslope_object	**hillslope_list,		*/
/*			struct	default_object		*defaults)				*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	computes mean hillslope soil parameters based on its patches		*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*	Original code, January 16, 2003.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void compute_mean_hillslope_parameters( struct hillslope_object *hillslope)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int i,j;	
	
	/*--------------------------------------------------------------*/
	/*	Based on  zones in this hillslope .						*/
	/*	and calculate mean hillslope parameters 			*/
	/*--------------------------------------------------------------*/
	hillslope[0].area = 0.0;
	hillslope[0].aggdefs.m = 0.0;
	hillslope[0].aggdefs.psi_air_entry = 0.0;
	hillslope[0].aggdefs.porosity_0 = 0.0;
	hillslope[0].aggdefs.porosity_decay = 0.0;
	hillslope[0].aggdefs.N_decay_rate = 0.0;
	hillslope[0].aggdefs.active_zone_z = 0.0;
	hillslope[0].aggdefs.soil_depth = 0.0;
	hillslope[0].aggdefs.NH4_absorption_rate = 0.0;
	hillslope[0].aggdefs.NO3_absorption_rate = 0.0;
	
	for ( i=0 ; i<hillslope[0].num_zones ; i++ ){
		for	 (j =0; j < hillslope[0].zones[i][0].num_patches ; j++) {
			hillslope[0].area += hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.m +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].m *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.psi_air_entry +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].psi_air_entry*
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_0 +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_0 *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_decay +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_decay *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.N_decay_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].N_decay_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.active_zone_z +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].active_zone_z *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.soil_depth +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].soil_depth *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NH4_absorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NH4_absorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NO3_absorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NO3_absorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
		}
	}
	hillslope[0].aggdefs.psi_air_entry /= hillslope[0].area;
	hillslope[0].aggdefs.m /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_0 /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_decay /= hillslope[0].area;
	hillslope[0].aggdefs.N_decay_rate /= hillslope[0].area;
	hillslope[0].aggdefs.active_zone_z /= hillslope[0].area;
	hillslope[0].aggdefs.soil_depth /= hillslope[0].area;
	hillslope[0].aggdefs.NO3_absorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.NH4_absorption_rate /= hillslope[0].area;

	return;

	} /* end compute_mean_hillslope_parameters */
