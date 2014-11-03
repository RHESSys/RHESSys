/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_N_settled				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_N_settled				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_N_settled(double	,		*/
/*					 double	,		*/
/*					 double	);		*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"


double	compute_N_settled(double surface_N,
			double scm_ave_height,
               double settling_rate)
{
     /*------------------------------------------------------*/
     /* Local variable Definition    */
     /*------------------------------------------------------*/
     double nsettled;

     /*------------------------------------------------------*/
     /* Computation of settling: kg/d    */
     /*------------------------------------------------------*/
     nsettled = (settling_rate / scm_ave_height) * surface_N;
	 nsettled = max(surface_N, 0.0);
	
	return(nsettled);

} /* end compute_N_settled */

