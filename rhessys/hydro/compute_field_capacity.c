/*------------------------------------------------------------------*/
/*                                                                  */
/*                compute_field_capacity                            */
/*                                                                  */
/*                                                                  */
/*  NAME                                                            */
/*        compute_field_capacity - computes unsat zone field        */
/*                capacity using an embedded psi-theta curve.       */
/*                                                                  */
/*  SYNOPSIS                                                        */
/*        double        compute_field_capacity(                     */
/*                        int        ,                              */
/*                        int        ,                              */
/*                        double        ,                           */
/*                        double        ,                           */
/*                        double        ,                           */
/*                        double        ,                           */
/*                        double        ,                           */
/*                        double        ,                           */
/*                        double        )                           */
/*                                                                  */
/*        returns:                                                  */
/*        field_capacity (m water) - amount of water at f.c.        */
/*                                                                  */
/*  OPTIONS                                                         */
/*        int       verbose_flag                                    */
/*        int       curve - psi-theta curve selector                */
/*        double    psi_air_entry,                                  */
/*        double    pore_size_index,                                */
/*        double    p3,                                             */
/*        double    p4,                                             */
/*        double    p_0 - porosity at the surface                   */
/*        double    p - porosity decay parameter                    */
/*        double    z - (m) water table depth                       */
/*                                                                  */
/*  DESCRIPTION                                                     */
/*                                                                  */
/*        This routine computes the field capacity in the unsat     */
/*        zone given parameters for a selected psi-theta curve.     */
/*        We assume that the psi-theta curve is using:              */
/*                                                                  */
/*        theta =  theta_actual  / porosity                         */
/*                                                                  */
/*        where                                                     */
/*         theta_actual is the volume of water per unit vol soil    */
/*         porosity is the volume of pores water per unit vol soil  */
/*         we assume that both theta_actual and porosity are        */
/*        effective values (i.e. theta_residual is already          */
/*        accounted).                                               */
/*                                                                  */
/*                                                                  */
/*        The strategy used for computing field capacity is:        */
/*                                                                  */
/*        i) discretize the soil column into n layers from the      */
/*        water table to the soil surface.                          */
/*        ii) for each layer,                                       */
/*                a) compute psi as distance above water table      */
/*                        scaled by 1/weight density of water       */
/*                b) use the theta-psi curve to estimate theta      */
/*                c) use the porosity decay curve with depth to     */
/*                        estimate the porosity.                    */
/*                d) solve the equation given above for             */
/*                        theta_actual given theta_min              */
/*                                                                  */
/*  PROGRAMMER NOTES                                                */
/*                                                                  */
/*        This is essential a numerical integration exercise.       */
/*        We use general Adaptive Simpson's integration routine     */
/*        integrate_fldcap()  from  "compute_final_unsat.c"         */
/*                                                                  */
/*        we only define field capacity within the soil             */
/*        thus is z or z_surface is < 0, (ie ponded water)          */
/*        we set them to zero for field cap calculations            */
/*------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_field_capacity( int	    verbose_flag,
							    int	    curve,
							    double	psi_air_entry,
							    double	pore_size_index,
							    double	p3,
							    double	p4,
							    double	p_0,
							    double	p_decay,
							    double	z_water_table,
							    double	z,
							    double	z_surface )
{
	/*------------------------------*/
	/*	Local function declaration  */
	/*------------------------------*/

    double integrate_fldcap( double  za,        /*  layer-top    soil-depth (M)                   */
                             double  zb,        /*  layer-bottom soil-depth (M)                   */
                             double  zsat,      /*  water-table  soil-depth (M)                   */
                             int     curve,     /*  patch->soil_defaults[0][0].theta_psi_curve    */
                             double  psiair,    /*  patch->soil_defaults[0][0].psi_air_entry (Pa) */
                             double  pordex,    /*  patch->soil_defaults[0][0].pore_size_index    */
                             double  p3,        /*  patch->soil_defaults[0][0].p3                 */
                             double  p4,        /*  patch->soil_defaults[0][0].p4                 */
                             double  por0,      /*  patch->soil_defaults[0][0].porosity_0         */
                             double  pord ) ;   /*  patch->soil_defaults[0][0].porosity_decay     */

	return integrate_fldcap( z_surface, 
                             z, 
                             z_water_table,
                             curve,
                             psi_air_entry,
							 pore_size_index,
							 p3,
							 p4,
							 p_0,
							 p_decay ) ;    

} /*compute_field_capacity*/ 
