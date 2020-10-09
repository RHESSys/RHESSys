/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_fire						    */
/*																*/
/*	output_yearly_fire - creates yearly output files objects.	*/
/*																*/
/*	NAME														*/
/*	output_yearly_fire - outputs a yearly fire object.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_fire(								    */
/*			 struct	canopy_stratum_object	*canopy_stratum,   */
/*			 struct	date	date,  				        		*/
/*			 FILE 	*outfile)				        			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs spatial structure according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES	(20200401)								*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_yearly_fire( int basinID, int hillID, int zoneID, int patchID,
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

	if(stratum[0].fe.acc_year.length == 0)
        stratum[0].fe.acc_year.length = 1;

	fprintf(outfile,
		"%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d\n",
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		stratum[0].defaults[0][0].ID, //7
		(stratum[0].fe.acc_year.m_cwdc_to_atmos), // carbon flux is different
		(stratum[0].fe.acc_year.m_cwdn_to_atmos),
		stratum[0].fe.canopy_target_height, //different
		(stratum[0].fe.acc_year.canopy_target_height_u_prop) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_mort) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_mort_consumed) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_mort_u_component) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_mort_o_component) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_c_consumed) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_c_remain) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_c_remain_adjusted) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_target_prop_c_remain_adjusted_leafc) / stratum[0].fe.acc_year.length,
		stratum[0].fe.canopy_subtarget_height, //different
		(stratum[0].fe.acc_year.canopy_subtarget_height_u_prop) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_subtarget_prop_mort) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_subtarget_prop_mort_consumed) / stratum[0].fe.acc_year.length,
		(stratum[0].fe.acc_year.canopy_subtarget_prop_c_consumed) / stratum[0].fe.acc_year.length,
		 stratum[0].fe.canopy_subtarget_biomassc, //different
		 // new output

		 stratum[0].fe.acc_year.litter_c_consumed,
		 stratum[0].fe.acc_year.understory_c_consumed, //exist
		 stratum[0].fe.acc_year.understory_leafc_consumed,
		 stratum[0].fe.acc_year.understory_stemc_consumed,
		 stratum[0].fe.acc_year.understory_rootc_consumed,

		 stratum[0].fe.acc_year.overstory_c_consumed,
		 stratum[0].fe.acc_year.overstory_leafc_consumed,
		 stratum[0].fe.acc_year.overstory_stemc_consumed,
		 stratum[0].fe.acc_year.overstory_rootc_consumed,

		 stratum[0].fe.acc_year.overstory_c_mortality,
		 stratum[0].fe.acc_year.overstory_leafc_mortality,
		 stratum[0].fe.acc_year.overstory_stemc_mortality,
		 stratum[0].fe.acc_year.overstory_rootc_mortality,

		 stratum[0].fe.acc_year.length,
		 stratum[0].fe.acc_year.length_overstory,
		 stratum[0].fe.acc_year.length_understory);


        /* reset the acc value to zero*/

        stratum[0].fe.acc_year.length = 0.0;
        stratum[0].fe.acc_year.length_overstory = 0.0;
        stratum[0].fe.acc_year.length_understory = 0.0;

		stratum[0].fe.acc_year.m_cwdc_to_atmos = 0.0;
		stratum[0].fe.acc_year.m_cwdn_to_atmos = 0.0;
		//stratum[0].fe.canopy_target_height, //different
		stratum[0].fe.acc_year.canopy_target_height_u_prop = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_mort = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_mort_consumed = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_mort_u_component = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_mort_o_component = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_c_consumed = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_c_remain = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_c_remain_adjusted = 0.0;
		stratum[0].fe.acc_year.canopy_target_prop_c_remain_adjusted_leafc = 0.0;
		//stratum[0].fe.canopy_subtarget_height, //different
		stratum[0].fe.acc_year.canopy_subtarget_height_u_prop = 0.0;
		stratum[0].fe.acc_year.canopy_subtarget_prop_mort = 0.0;
		stratum[0].fe.acc_year.canopy_subtarget_prop_mort_consumed = 0.0;
		stratum[0].fe.acc_year.canopy_subtarget_prop_c_consumed = 0.0;
		//stratum[0].fe.canopy_subtarget_c, //different

		stratum[0].fe.acc_year.litter_c_consumed = 0.0;

		stratum[0].fe.acc_year.understory_c_consumed = 0.0;
		stratum[0].fe.acc_year.understory_leafc_consumed = 0.0;
		stratum[0].fe.acc_year.understory_stemc_consumed = 0.0;
		stratum[0].fe.acc_year.understory_rootc_consumed = 0.0;

		stratum[0].fe.acc_year.overstory_c_consumed = 0.0;
		stratum[0].fe.acc_year.overstory_leafc_consumed = 0.0;
		stratum[0].fe.acc_year.overstory_stemc_consumed = 0.0;
		stratum[0].fe.acc_year.overstory_rootc_consumed = 0.0;

        stratum[0].fe.acc_year.overstory_c_mortality = 0.0;
		stratum[0].fe.acc_year.overstory_leafc_mortality = 0.0;
		stratum[0].fe.acc_year.overstory_stemc_mortality = 0.0;
		stratum[0].fe.acc_year.overstory_rootc_mortality = 0.0;



	return;
} /*end output_fire*/
