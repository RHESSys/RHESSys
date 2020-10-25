/*--------------------------------------------------------------*/
/* 																*/
/*					output_fire						*/
/*																*/
/*	output_fire - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_fire - outputs current contents of a fire object.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_fire(										*/
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
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

void	output_fire( int basinID, int hillID, int zoneID, int patchID,
							  struct	canopy_strata_object	*stratum,
							  struct	date	current_date,
							  FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	fprintf(outfile,
		"%d %d %d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		stratum[0].defaults[0][0].ID,
		stratum[0].fe.m_cwdc_to_atmos,
		stratum[0].fe.m_cwdn_to_atmos,
		stratum[0].fe.canopy_target_height,
		stratum[0].fe.canopy_target_height_u_prop,
		stratum[0].fe.canopy_target_prop_mort,
		stratum[0].fe.canopy_target_prop_mort_consumed,
		stratum[0].fe.canopy_target_prop_mort_u_component,
		stratum[0].fe.canopy_target_prop_mort_o_component,
		stratum[0].fe.canopy_target_prop_c_consumed,
		stratum[0].fe.canopy_target_prop_c_remain,
		stratum[0].fe.canopy_target_prop_c_remain_adjusted,
		stratum[0].fe.canopy_target_prop_c_remain_adjusted_leafc,
		stratum[0].fe.canopy_subtarget_height,
		stratum[0].fe.canopy_subtarget_height_u_prop,
		stratum[0].fe.canopy_subtarget_prop_mort,
		stratum[0].fe.canopy_subtarget_prop_mort_consumed,
		stratum[0].fe.canopy_subtarget_prop_c_consumed,
		stratum[0].fe.canopy_subtarget_c,
		stratum[0].fe.understory_c_consumed);
	return;
} /*end output_fire*/
