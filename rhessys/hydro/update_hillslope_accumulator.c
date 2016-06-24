/*--------------------------------------------------------------------------------------*/
/* 											*/
/*			update_basin_patch_accumulator					*/
/*											*/
/*	NAME										*/
/*	update_hillslope_accumulator.c - update accumulator variables at the end of day	*/
/*											*/
/*	SYNOPSIS									*/
/*	void update_hillslope_accumulator( 						*/
/*					struct command_line_object *command_line,	*/
/*					struct basin_object *basin			*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*	this function is called in basin_daily_F at the end of each day,		*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
void update_hillslope_accumulator(
			struct command_line_object *command_line,
			struct basin_object *basin){
	/*--------------------------------------------------------------------------------------*/
	/* Local variables definitin								*/
	/*--------------------------------------------------------------------------------------*/
    //160420LML double scale;
    //160420LML int h,z,p;
    //160420LML struct hillslope_object *hillslope;
    //160420LML struct patch_object * patch;

	/*--------------------------------------------------------------------------------------*/
	/* update hillslope accumulator								*/
	/*--------------------------------------------------------------------------------------*/
    #pragma omp parallel for
    for (int h = 0 ; h < basin[0].num_hillslopes; h++ ){
        struct hillslope_object *hillslope = basin[0].hillslopes[h];
		hillslope[0].acc_month.length += 1;
        for (int z = 0; z < hillslope[0].num_zones; z++) {
            for (int p=0; p < hillslope[0].zones[z][0].num_patches; p++) {
                struct patch_object *patch = hillslope[0].zones[z][0].patches[p];

                double scale = patch[0].area / hillslope[0].area;

				if((command_line[0].output_flags.monthly == 1)&&(command_line[0].h != NULL)){
					hillslope[0].acc_month.snowpack += (patch[0].snowpack.water_equivalent_depth) * scale;
					hillslope[0].acc_month.streamflow += (patch[0].streamflow) * scale;
					hillslope[0].acc_month.et += (patch[0].transpiration_unsat_zone
						+ patch[0].evaporation_surf + 
						patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone +
						patch[0].transpiration_sat_zone + patch[0].evaporation) * scale;
					hillslope[0].acc_month.denitrif += patch[0].ndf.denitrif * scale;
					hillslope[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate * scale;
					hillslope[0].acc_month.mineralized += patch[0].ndf.net_mineralized * scale;
					hillslope[0].acc_month.uptake += patch[0].ndf.sminn_to_npool * scale;
					hillslope[0].acc_month.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
					hillslope[0].acc_month.DON_loss+= patch[0].ndf.total_DON_loss * scale;
					hillslope[0].acc_month.stream_NO3 += patch[0].streamflow_NO3 * scale;
					hillslope[0].acc_month.stream_NH4 += patch[0].streamflow_NH4 * scale;
					hillslope[0].acc_month.psn += patch[0].net_plant_psn * scale;
					hillslope[0].acc_month.lai += patch[0].lai * scale;
				}
				if((command_line[0].output_flags.yearly == 1)&&(command_line[0].h != NULL)){
					hillslope[0].acc_year.length += 1;
					hillslope[0].acc_year.stream_NO3 += patch[0].streamflow_NH4 * scale;
					hillslope[0].acc_year.stream_NH4 += patch[0].streamflow_NO3 * scale;
					hillslope[0].acc_year.denitrif += patch[0].ndf.denitrif * scale;
					hillslope[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate * scale;
					hillslope[0].acc_year.mineralized += patch[0].ndf.net_mineralized * scale;
					hillslope[0].acc_year.uptake += patch[0].ndf.sminn_to_npool * scale;
					hillslope[0].acc_year.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
					hillslope[0].acc_year.DON_loss += patch[0].ndf.total_DON_loss * scale;
					hillslope[0].acc_year.psn += patch[0].net_plant_psn * scale;
					hillslope[0].acc_year.et += (patch[0].evaporation 
						+ patch[0].evaporation_surf + 
						patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone +
						patch[0].transpiration_unsat_zone + patch[0].transpiration_sat_zone) * scale;
					hillslope[0].acc_year.streamflow += (patch[0].streamflow) * scale;
					hillslope[0].acc_year.lai += patch[0].lai * scale;
				}
			} /* end of patch p  */
		} /* end of zones z */
	} /* end of hillslope  */	
	return;
} /* end of file update_hillslope_accumulator.c */
