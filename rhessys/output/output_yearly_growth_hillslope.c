/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_growth_hillslope				*/
/*																*/
/*	output_yearly_growth_hillslope - creates output files objects.*/
/*																*/
/*	NAME														*/
/*	output_yearly_growth_hillslope								*/
/*			- outputs current contents of a hillslope.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_growth_hillslope( int basinID			*/
/*					struct	hillslope_object	*hillslope,		*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs spatial structure according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_yearly_growth_hillslope(
				int basinID,
				struct	hillslope_object	*hillslope,
				struct	date	date,
				FILE *outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*  Local function definition                                   */
	/*--------------------------------------------------------------*/

	/* Not Implemented Yet */

} /*end output_growth_hillslope*/
