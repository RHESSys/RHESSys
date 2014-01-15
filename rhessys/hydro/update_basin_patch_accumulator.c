/*--------------------------------------------------------------------------------------*/
/* 											*/
/*			update_basin_patch_accumulator					*/
/*											*/
/*	NAME										*/
/*	update_basin_patch_accumulator.c - update accumulator variables at the end of day	*/
/*					this process is taken from compute_subsurface_routing.c	*/
/*	SYNOPSIS									*/
/*	void update_basin_patch_accumulator( 						*/
/*					struct command_line_object *command_line,	*/
/*					struct basin_object *basin			*/
/*					struct date current_date)			*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*	this function is called in basin_daily_F at the end of each day, it was in  	*/
/*	the compute_subsurface_routing, 										*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*											*/	
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void update_basin_patch_accumulator(
			struct command_line_object 	*command_line,
			struct basin_object 		*basin,
			struct date		 	current_date)
{
	/*----------------------------------------------------------------------*/
	/* Local variables definition                                           */
	/*-----------------------------------------------------------------------*/
	double scale;
	double tmp;
	int i;
	struct patch_object *patch;
	/*----------------------------------------------------------------------*/
	/* initializations		                                           */
	/*----------------------------------------------------------------------*/	

	/*---------------------------------------------------------------------*/
	/*update accumulator variables                                            */
	/*-----------------------------------------------------------------------*/
	for (i=0; i<basin->route_list->num_patches;i++) {
		patch = basin->route_list->list[i];

		patch[0].acc_year_trans += (patch[0].transpiration_unsat_zone	+ patch[0].transpiration_sat_zone);
		if ((command_line[0].output_flags.monthly == 1)	&& (command_line[0].b != NULL )) {
			scale = patch[0].area / basin[0].area;
			basin[0].acc_month.streamflow += (patch[0].streamflow) * scale;
			basin[0].acc_month.et += (patch[0].transpiration_unsat_zone
					+ patch[0].evaporation_surf
					+ patch[0].exfiltration_unsat_zone
					+ patch[0].exfiltration_sat_zone
					+ patch[0].transpiration_sat_zone
					+ patch[0].evaporation) * scale;
			basin[0].acc_month.denitrif += patch[0].ndf.denitrif * scale;
			basin[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate * scale;
			basin[0].acc_month.mineralized +=patch[0].ndf.net_mineralized * scale;
			basin[0].acc_month.uptake += patch[0].ndf.sminn_to_npool * scale;
			basin[0].acc_month.DON_loss +=(patch[0].soil_ns.DON_Qout_total - patch[0].soil_ns.DON_Qin_total) * scale;
			basin[0].acc_month.DOC_loss +=(patch[0].soil_cs.DOC_Qout_total - patch[0].soil_cs.DOC_Qin_total) * scale;
			basin[0].acc_month.length += 1;
			basin[0].acc_month.stream_NO3 += patch[0].streamflow_NO3 * scale;
			basin[0].acc_month.stream_NH4 += patch[0].streamflow_NH4 * scale;
			basin[0].acc_month.stream_DON += patch[0].streamflow_DON * scale;
		basin[0].acc_year.stream_NO3 += patch[0].streamflow_NO3	* scale;
			basin[0].acc_year.denitrif += patch[0].ndf.denitrif * scale;
			basin[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate * scale;
			basin[0].acc_year.mineralized += patch[0].ndf.net_mineralized * scale;
			basin[0].acc_year.uptake += patch[0].ndf.sminn_to_npool	* scale;
			basin[0].acc_year.DON_loss +=	(patch[0].soil_ns.DON_Qout_total - patch[0].soil_ns.DON_Qin_total) * scale;
			basin[0].acc_year.DOC_loss +=	(patch[0].soil_cs.DOC_Qout_total - patch[0].soil_cs.DOC_Qin_total) * scale;
			basin[0].acc_year.stream_DON += patch[0].streamflow_DON	* scale;
			basin[0].acc_year.stream_DOC += patch[0].streamflow_DOC * scale;
			basin[0].acc_year.psn += patch[0].net_plant_psn * scale;
			basin[0].acc_year.PET += (patch[0].PE + patch[0].PET) * scale;
			basin[0].acc_year.et += (patch[0].evaporation
					+ patch[0].evaporation_surf
					+ patch[0].exfiltration_unsat_zone
					+ patch[0].exfiltration_sat_zone
					+ patch[0].transpiration_unsat_zone
					+ patch[0].transpiration_sat_zone) * scale;
			basin[0].acc_year.streamflow += (patch[0].streamflow) * scale;
			basin[0].acc_year.lai += patch[0].lai * scale;
		}

		if ((command_line[0].output_flags.monthly == 1) && (command_line[0].p != NULL )) {
			patch[0].acc_month.theta += patch[0].rootzone.S;
			patch[0].acc_month.sm_deficit +=max(0.0, (patch[0].sat_deficit-patch[0].rz_storage-patch[0].unsat_storage));
			patch[0].acc_month.et += (patch[0].transpiration_unsat_zone
					+ patch[0].evaporation_surf
					+ patch[0].exfiltration_unsat_zone
					+ patch[0].exfiltration_sat_zone
					+ +patch[0].transpiration_sat_zone
					+ patch[0].evaporation);
			patch[0].acc_month.denitrif += patch[0].ndf.denitrif;
			patch[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate;
			patch[0].acc_month.mineralized += patch[0].ndf.net_mineralized;
			patch[0].acc_month.uptake += patch[0].ndf.sminn_to_npool;
			patch[0].acc_month.DON_loss += (patch[0].soil_ns.DON_Qout_total - patch[0].soil_ns.DON_Qout_total);
			patch[0].acc_month.DOC_loss += (patch[0].soil_cs.DOC_Qout_total - patch[0].soil_cs.DOC_Qout_total);
			patch[0].acc_month.psn += patch[0].net_plant_psn;
			patch[0].acc_month.snowpack =max(patch[0].snowpack.water_equivalent_depth, patch[0].acc_month.snowpack);
			patch[0].acc_month.lai = max(patch[0].acc_month.lai, patch[0].lai);
			patch[0].acc_month.leach += (patch[0].soil_ns.leach + patch[0].surface_ns_leach);
			patch[0].acc_month.length += 1;
		}
		if ((command_line[0].output_flags.yearly == 1) && (command_line[0].p != NULL )) {
			patch[0].acc_year.length += 1;
			if ((patch[0].sat_deficit - patch[0].unsat_storage) > command_line[0].thresholds[SATDEF])
				patch[0].acc_year.num_threshold += 1;
				patch[0].acc_year.theta += patch[0].rootzone.S;
				patch[0].acc_year.denitrif += patch[0].ndf.denitrif;
				patch[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate;
				patch[0].acc_year.mineralized += patch[0].ndf.net_mineralized;
				patch[0].acc_year.uptake += patch[0].ndf.sminn_to_npool;
				patch[0].acc_year.leach += (patch[0].soil_ns.leach+ patch[0].surface_ns_leach);
				patch[0].acc_year.DON_loss += (patch[0].soil_ns.DON_Qout_total - patch[0].soil_ns.DON_Qout_total);
				patch[0].acc_year.DOC_loss += (patch[0].soil_cs.DOC_Qout_total - patch[0].soil_cs.DOC_Qout_total);
				patch[0].acc_year.streamflow += patch[0].streamflow;
				patch[0].acc_year.Qout_total += patch[0].Qout_total;
				patch[0].acc_year.Qin_total += patch[0].Qin_total;
				patch[0].acc_year.psn += patch[0].net_plant_psn;
				patch[0].acc_year.PET += (patch[0].PE + patch[0].PET);
				patch[0].acc_year.burn += patch[0].burn;
				patch[0].acc_year.potential_recharge +=	patch[0].rain_throughfall;
				patch[0].acc_year.potential_recharge_wyd += patch[0].rain_throughfall * round(patch[0].acc_year.length);
				patch[0].acc_year.recharge += patch[0].recharge;
				patch[0].acc_year.recharge_wyd += patch[0].recharge * round(patch[0].acc_year.length);
			if ((patch[0].snowpack.water_equivalent_depth == 0) && (patch[0].acc_year.snowpack > 0)) {
				if (patch[0].acc_year.meltday < patch[0].acc_year.peaksweday)
					patch[0].acc_year.meltday = round(patch[0].acc_year.length);
			}

			if (patch[0].snowpack.water_equivalent_depth > patch[0].acc_year.snowpack) {
				patch[0].acc_year.peaksweday = round(patch[0].acc_year.length);
			}

			patch[0].acc_year.snowpack =max(patch[0].snowpack.water_equivalent_depth,patch[0].acc_year.snowpack);

			/* transpiration water stress computations */
			tmp = (patch[0].transpiration_unsat_zone
					+ patch[0].exfiltration_unsat_zone
					+ patch[0].exfiltration_sat_zone
					+ patch[0].evaporation_surf
					+ +patch[0].transpiration_sat_zone
					+ patch[0].evaporation);
			patch[0].acc_year.et += tmp;
			tmp = (patch[0].transpiration_unsat_zone + patch[0].transpiration_sat_zone);
			patch[0].acc_year.trans += tmp;
			patch[0].acc_year.day7trans = (tmp / 14 + 13 / 14 * patch[0].acc_year.day7trans);
			patch[0].acc_year.day7pet = (patch[0].PET + patch[0].PE) / 14 + 13 / 14 * patch[0].acc_year.day7pet;
			if (patch[0].acc_year.day7pet > patch[0].acc_year.maxpet) {
				patch[0].acc_year.maxpet = patch[0].acc_year.day7pet;
				patch[0].acc_year.rec_pet_wyd = 0;
				patch[0].acc_year.max_pet_wyd = patch[0].acc_year.wyd;
			}

			if ((patch[0].acc_year.day7trans > patch[0].acc_year.maxtrans)) {
				patch[0].acc_year.maxtrans = patch[0].acc_year.day7trans;
				patch[0].acc_year.rec_wyd = 0;
			}

			if ((patch[0].acc_year.rec_wyd == 0) && (patch[0].acc_year.day7trans < patch[0].acc_year.maxtrans * 0.5)) {
				patch[0].acc_year.rec_wyd = patch[0].acc_year.wyd;
			}

			if ((patch[0].acc_year.rec_pet_wyd == 0) && (patch[0].acc_year.day7pet < patch[0].acc_year.maxpet * 0.5)) {
				patch[0].acc_year.rec_pet_wyd = patch[0].acc_year.wyd;
			}

			tmp = (patch[0].transpiration_unsat_zone
					+ patch[0].exfiltration_unsat_zone
					+ patch[0].exfiltration_sat_zone
					+ patch[0].evaporation_surf
					+ +patch[0].transpiration_sat_zone
					+ patch[0].evaporation);

			if ((patch[0].PET + patch[0].PE - tmp) > patch[0].acc_year.sm_deficit)
				patch[0].acc_year.sm_deficit = (patch[0].PET + patch[0].PE - tmp);
			patch[0].acc_year.lai = max(patch[0].acc_year.lai, patch[0].lai);

			tmp = patch[0].sat_deficit - patch[0].unsat_storage - patch[0].rz_storage;
			
			if (tmp <= 0)
				patch[0].acc_year.ndays_sat += 1;

			if (patch[0].rootzone.S > 0.7)
				patch[0].acc_year.ndays_sat70 += 1;

			tmp =	max(0.0, (patch[0].rootzone.field_capacity/patch[0].rootzone.potential_sat -
							patch[0].wilting_point*patch[0].soil_defaults[0][0].porosity_0))
							/ 2.0
							+ patch[0].wilting_point
									* patch[0].soil_defaults[0][0].porosity_0;

			if ((patch[0].rootzone.S < tmp) && (current_date.month < 10)
					&& (patch[0].acc_year.midsm_wyd == 0)
					&& (patch[0].snowpack.water_equivalent_depth <= 0.0))
				patch[0].acc_year.midsm_wyd = patch[0].acc_year.wyd;

			patch[0].acc_year.wyd = patch[0].acc_year.wyd + 1;
		} /* end if */		
	} /* end of i*/


	return;
} /* end of update_basin_patch_accumulator.c */
