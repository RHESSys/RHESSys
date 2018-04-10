/*--------------------------------------------------------------*/
/* 								*/
/*		compute_field_capacity				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_field_capacity - computes unsat zone field 	*/
/*		capacity using an embedded psi-theta curve.	*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_field_capacity(				*/
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
/*   double	psi_air_entry,					*/
/*    double	pore_size_index,				*/
/*   double	p3,						*/
/*   double	p4,						*/
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
double	compute_field_capacity(
							   int	verbose_flag,
							   int	curve,
							   double	psi_air_entry,
							   double	pore_size_index,
							   double	p3,
							   double	p4,
							   double	p_0,
							   double	p,
							   double	z_water_table,
							   double	z,
							   double	z_surface)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	double	compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	depth;
	double	field_capacity;
	double	max_field_capacity;
	double	porosity;
	double	psi;
	double	theta;
	double	theta_actual;
	/*--------------------------------------------------------------*/
	/*	Initialize field capacity at 0.				*/
	/*--------------------------------------------------------------*/
	field_capacity = 0;
	/*--------------------------------------------------------------*/
	/*	Make sure p and p_0 are non zero.			*/
	/*--------------------------------------------------------------*/
	p = max(p,0.00000001);
	p_0 = max(p_0,0.00000001);
	if (z < ZERO) z = 0.0;
	if (z_surface < ZERO) z_surface = 0.0;

	/*--------------------------------------------------------------*/
	/*	Only do numerical int. if porosity varies with depth    */
	/*	otherwise use analytical solution 		 	*/
	/*--------------------------------------------------------------*/
	if ( p < 999.0 ) { 
		/*--------------------------------------------------------------*/
		/*	Loop through each interval.				*/
		/*	Only if the water table is not at or above the surface.	*/
		/*--------------------------------------------------------------*/
		if ( z > 0 ){
			for ( depth = z ; depth >z_surface ; depth = depth - INTERVAL_SIZE){
				psi = (z-depth);
				/*--------------------------------------------------------------*/
				/*		Switch between differnt theta-psi curves	*/
				/*--------------------------------------------------------------*/
				if ( psi > psi_air_entry ){
					switch(curve) {
					case 1:
						theta = pow((psi_air_entry /psi),pore_size_index);
						break;
					case 2:
						theta = pow(1+pow(psi/psi_air_entry,p3),-pore_size_index);
						break;
					case 3: 
						theta = exp((log(psi)-p3)/p4);
						break;
					}
				}
				else{
					theta = 1;
				}
				porosity = p_0 * exp( -1 * depth / p);
				theta_actual = theta * porosity ;
				field_capacity += theta_actual * INTERVAL_SIZE;
			}
		}
	}
	else{
		/* Changed field capacity to eqn in Dingman p. 235 using fixed pressure head */
		/* of -340cm (constant over depth) vs. older method that varied field capacity */
		/* with depth and overshot (field capacity sat > 1) close to sat zone. */
		field_capacity = p_0 * ( pow((psi_air_entry / 3.4),pore_size_index) ) * (z - z_surface);
		/*field_capacity = 1.0 / (1.0 - pore_size_index) * p_0
			* pow( psi_air_entry, pore_size_index)
			* (pow( z - z_surface, 1-pore_size_index));*/
	}
	/*--------------------------------------------------------------*/
	/*	Limit field capacity to at most the porosity 		*/
	/*--------------------------------------------------------------*/
	max_field_capacity = compute_delta_water(
		verbose_flag,
		p_0,
		p,
		z_water_table,
		z,
		z_surface);

	field_capacity = min(field_capacity,max_field_capacity);
	field_capacity = max(field_capacity, 0.0);	
	
	return(field_capacity);
} /*compute_field_capacity*/ 
