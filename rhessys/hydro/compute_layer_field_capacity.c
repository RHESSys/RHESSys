/*--------------------------------------------------------------*/
/* 								*/
/*		compute_layer_field_capacity				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_layer_field_capacity - computes unsat zone field 	*/
/*		capacity using an embedded psi-theta curve.	*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_layer_field_capacity(				*/
/*			int	,				*/
/*			int	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	)				*/
/*								*/
/*	returns:						*/
/*	field_capacity (m water) - amount of water at f.c.	*/
/*								*/
/*	OPTIONS							*/
/*	int verbose_flag 					*/
/*	int	curve - psi-theta curve selector		*/
/*	double	pore_size_index - curve parameter 2				*/
/*	double	p3 - curve parameter 3				*/
/*	double	p4 - curve parameter 4				*/
/*	double	psi_air_entry (Pa) -  air entry pressure.	*/
/*	double	p_0 - porosity at the surface			*/
/*	double	p - porosity decay parameter			*/
/*	double	z - (m) water table depth			*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine computes the field capacity in the unsat 	*/
/*	zone given parameters for a selected psi-theta curve.	*/
/*	We assume that the psi-theta curve is using:	 	*/
/*								*/
/*	theta =  theta_actual  / porosity 	             	*/
/*								*/
/*	where							*/
/*	 theta_actual is the volume of water per unit vol soil	*/
/*	 porosity is the volume of pores water per unit vol soil*/
/*	 we assume that both theta_actual and porosity are	*/
/*	effective values (i.e. theta_residual is already	*/
/*	accounted).						*/
/*								*/
/*								*/
/*	The strategy used for computing field capacity is:	*/
/*								*/
/*	i) discretize the soil column into n layers from the	*/
/*	water table to the soil surface.  			*/
/*	ii) for each layer, 					*/
/*		a) compute psi as distance above water table	*/
/*			scaled by 1/weight density of water	*/
/*		b) use the theta-psi curve to estimate theta	*/
/*		c) use the porosity decay curve with depth to	*/
/*			estimate the porosity.			*/
/*		d) solve the equation given above for 		*/
/*			theta_actual given theta_min 	 	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	This is essential a numerical integration exercise.	*/
/*	We use constant intervals given that, while the		*/
/*	thea-psi curve decreases quickly with increasing psi	*/
/*	(depth above the water table) the porosity - z curve	*/
/*	decreases quickly with depth from the surface.		*/
/*								*/
/*	we only define field capacity within the soil		*/
/*	thus is z or z_surface is < 0, (ie ponded water)	*/
/*	we set them to zero for field cap calculations		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"
double	compute_layer_field_capacity(
							   int	verbose_flag,
							   int	curve,
							   double	psi_air_entry,
							   double	pore_size_index,
							   double	p3,
							   double	p4,
							   double	p_0,
							   double	p,
							   double	z_water_table,
							   double	z_layer,
							   double	z_surface)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration			*/ 
	/*--------------------------------------------------------------*/
	double	compute_field_capacity(
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
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	partial_field_capacity;
	double	full_field_capacity, field_capacity;
	
	/*--------------------------------------------------------------*/
	/* field capacity of the layer is the difference between field */
	/* capacity of full unsat zone - field capacity between		*/
	/* bottom of the layer and water table				*/
	/*--------------------------------------------------------------*/




	partial_field_capacity = 0.0;
	full_field_capacity = compute_field_capacity( verbose_flag,
						curve,
						psi_air_entry,
						pore_size_index,
						p3,
						p4,
						p_0,
						p,
						z_water_table,
						z_water_table,
						z_surface);

	if (z_layer <  z_water_table)
		partial_field_capacity = compute_field_capacity( verbose_flag,
						curve,
						psi_air_entry,
						pore_size_index,
						p3,
						p4,
						p_0,
						p,
						z_water_table,
						z_water_table,
						z_layer);

	field_capacity = full_field_capacity - partial_field_capacity;

	return(field_capacity);

} /*compute_layer_field_capacity*/ 
