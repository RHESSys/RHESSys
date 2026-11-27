/*--------------------------------------------------------------*/
/* 								*/
/*		compute_transmissivity_curve			*/
/*								*/
/*	NAME							*/
/*	compute_transmissivity_curve - estimates transmissivity	*/
/*		assuming an exponential decay of transmivivity	*/
/*		with depth - note returned value is 		*/
/*		relative transmissivity (i.e per unit		*/
/*		Ksat at the surface)				*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_transmissivity_curve(				*/
/*				double	,			*/
/*				double	,			*/
/*				double	)			*/
/*								*/
/*	returns:						*/
/*	transmissivity - (unitless) multiplier for Ksat0 	*/
/*		to calculate transmissivity over range of	*/
/*		depths specified				*/
/*								*/
/*	OPTIONS							*/
/*	double	m - Ksat decay parameter			*/
/*	double	z - (m) depth to the water table		*/
/*	double  D - maximum depth				*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes transmissivity multiplier over range of 	*/
/*	depths between z and D; assumes an exponential decay	*/
/*	of Ksat with depth (decay given by m) until z_layer1	*/
/*	and then a constant conductivity below that layer	*/
/*	given by Ksat at the surface				*/
/*	Note that if m is 0, we assume that Ksat is constant    */
/*	with depth						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*	Field capacity per interval is cached per soil	*/
/*	parameter set (fc_cache); not thread safe, called	*/
/*	only during serial initialization.			*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

struct fc_cache_entry {
	int theta_psi_curve, num_intervals;
	double psi_air_entry, pore_size_index, p3, p4, porosity_0, porosity_decay,
		soil_depth, soil_water_cap, interval_size;
	double *fc;
	struct fc_cache_entry *next;
};
static struct fc_cache_entry *fc_cache = NULL;

double 	*compute_transmissivity_curve( 
					double  gamma,
					struct patch_object *patch,
					struct command_line_object *command_line
					)
{

	
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	double compute_field_capacity(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);


	double	compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);


	double	compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);

	 void    *alloc( size_t, char *, char *);


	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	/* Cleanup note: removed unused locals start and potential_sat. */

	int didx,initial;
	double	lower, depth, m;
	double	lower_z, depth_z;
	double fclayer;
	double transmissivity_layer;
	double *transmissivity;

	m = patch[0].soil_defaults[0][0].m;

	transmissivity = (double *) alloc((patch[0].num_soil_intervals+1) * sizeof(double),
					"trans","compute_transmissivity_cuve");


	/*--------------------------------------------------------------*/
	/*	for do not include surface overland flow or detention   */
	/*	storage here						*/
	/*--------------------------------------------------------------*/
	if (patch[0].soil_defaults[0][0].soil_water_cap > patch[0].soil_defaults[0][0].interval_size) {
		initial = patch[0].num_soil_intervals;
		depth = patch[0].soil_defaults[0][0].soil_water_cap;
		transmissivity[initial]=0.0;
		initial = initial-1;
		struct soil_default *sd = patch[0].soil_defaults[0];
		struct fc_cache_entry *ce;
		for (ce = fc_cache; ce != NULL; ce = ce->next) {
			if (ce->theta_psi_curve == sd->theta_psi_curve && ce->num_intervals == patch[0].num_soil_intervals
				&& ce->psi_air_entry == sd->psi_air_entry && ce->pore_size_index == sd->pore_size_index
				&& ce->p3 == sd->p3 && ce->p4 == sd->p4 && ce->porosity_0 == sd->porosity_0
				&& ce->porosity_decay == sd->porosity_decay && ce->soil_depth == sd->soil_depth
				&& ce->soil_water_cap == sd->soil_water_cap && ce->interval_size == sd->interval_size)
				break;
		}
		int fc_cached = (ce != NULL);
		if (!fc_cached) {
			ce = (struct fc_cache_entry *) alloc(sizeof(struct fc_cache_entry), "fc_cache", "compute_transmissivity_curve");
			ce->theta_psi_curve = sd->theta_psi_curve; ce->num_intervals = patch[0].num_soil_intervals;
			ce->psi_air_entry = sd->psi_air_entry; ce->pore_size_index = sd->pore_size_index;
			ce->p3 = sd->p3; ce->p4 = sd->p4; ce->porosity_0 = sd->porosity_0;
			ce->porosity_decay = sd->porosity_decay; ce->soil_depth = sd->soil_depth;
			ce->soil_water_cap = sd->soil_water_cap; ce->interval_size = sd->interval_size;
			ce->fc = (double *) alloc((patch[0].num_soil_intervals+1) * sizeof(double), "fc_cache", "compute_transmissivity_curve");
			ce->next = fc_cache;
			fc_cache = ce;
		}
		for (didx=initial; didx >= 0; didx -= 1) {
			lower = depth;
			depth = depth-patch[0].soil_defaults[0][0].interval_size;

			if (fc_cached) {
				fclayer = ce->fc[didx];
			}
			else {
			lower_z = compute_z_final(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0*lower);

			depth_z = compute_z_final(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0*depth);

	       		fclayer = compute_field_capacity(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].theta_psi_curve,
				patch[0].soil_defaults[0][0].psi_air_entry,
				patch[0].soil_defaults[0][0].pore_size_index,
				patch[0].soil_defaults[0][0].p3,
				patch[0].soil_defaults[0][0].p4,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				lower_z,
				depth_z);
			ce->fc[didx] = fclayer;
			}


		if (m > ZERO)
			transmissivity_layer = gamma  
			 * (exp ( -1.0 * (max(depth, 0.0)/ m)) - exp ( -1.0 * (lower/m))); 
		else
			transmissivity_layer =  gamma * (lower-depth);

		fclayer = max(patch[0].soil_defaults[0][0].interval_size-fclayer,0.0);

		transmissivity_layer = min(fclayer, transmissivity_layer/patch[0].area);  

		if (gamma > ZERO)
			transmissivity[didx] = transmissivity[didx+1]+transmissivity_layer * 
				patch[0].area / gamma;
		else
			transmissivity[didx] = transmissivity[didx+1];

		}
	}
	
	else {
		initial = 1;
		transmissivity[initial]=0.0;
		lower = patch[0].soil_defaults[0][0].soil_water_cap;
		depth = 0;
		if (m > ZERO)
			transmissivity[initial-1] =   (exp ( -1.0 * (max(depth, 0.0)/ m)) - exp ( -1.0 * (lower/m))); 
		else
			transmissivity[initial-1] =  (lower-depth);
		
		}

	return(transmissivity);

} /*compute_transmissivity_curve*/
