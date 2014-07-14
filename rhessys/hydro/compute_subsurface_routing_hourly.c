/*--------------------------------------------------------------*/
/* 											*/
/*					compute_subsurface_routing_hourly		*/
/*											*/
/*	compute_subsurface_routing_hourly.c - do subsurface computation during the end of each hour	*/
/*											*/
/*	NAME										*/
/*	compute_subsurface_routing_hourly.c - do subsurface computation during the end of each hour	*/
/*											*/
/*	SYNOPSIS									*/
/*	void compute_subsurface_routing_hourly( 								*/
/*						struct command_line_object command_line, */
/*							struct basin_object *basin)	*/
/*				 			int,			 	*/
/*							struct date *current_date)	*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*	this function is called in basin_hourly_test at the end of each hour during	*/
/* 	hourly calculation, it is doing the compute_subsurface_routing for each hour	*/
/*											*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*											*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void compute_subsurface_routing_hourly(
		struct command_line_object *command_line,
		struct basin_object *basin, 
		int n_timesteps, 
		struct date current_date) {
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/

	void update_drainage_stream(struct patch_object *,
			struct command_line_object *, double, int);

	void update_drainage_road(struct patch_object *,
			struct command_line_object *, double, int);

	void update_drainage_land(struct patch_object *,
			struct command_line_object *, double, int);

	double compute_infiltration(int, double, double, double, double, double,
			double, double, double, double, double);

	double compute_z_final(int, double, double, double, double, double);

	double compute_N_leached(int, double, double, double, double, double,
			double, double, double, double, double, double, double);

	double compute_layer_field_capacity(int, int, double, double, double,
			double, double, double, double, double, double);

	double compute_unsat_zone_drainage(int, int, double, double, double, double,
			double, double);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int i, d;
	int j, k;
	int grow_flag, verbose_flag;
	double time_int, tmp;
	double theta, m, Ksat, Nout;
	double NO3_out, NH4_out, DON_out, DOC_out;
	double return_flow, excess;
	double water_balance, infiltration;
	double innundation_depth;
	double add_field_capacity, rz_drainage, unsat_drainage;
	double streamflow, Qout, Qin_total, Qstr_total;
	struct patch_object *patch;
	struct hillslope_object *hillslope;
	struct patch_object *neigh;
	struct litter_object *litter;
	/*--------------------------------------------------------------*/
	/*	initializations						*/
	/*--------------------------------------------------------------*/
	
		grow_flag = command_line[0].grow_flag;
		verbose_flag = command_line[0].verbose_flag;

		time_int = 1.0 / n_timesteps;

	if (current_date.hour==1)
	{
		basin[0].basin_outflow = 0.0;
		basin[0].basin_area = 0.0;
		basin[0].basin_unsat_storage = 0.0;
		basin[0].basin_rz_storage = 0.0;
		basin[0].basin_sat_deficit = 0.0;
		basin[0].basin_return_flow = 0.0;
		basin[0].basin_detention_store = 0.0;
		basin[0].preday_basin_rz_storage = 0.0;
		basin[0].preday_basin_unsat_storage = 0.0;
		basin[0].preday_basin_sat_deficit = 0.0;
		basin[0].preday_basin_return_flow = 0.0;
		basin[0].preday_basin_detention_store = 0.0;
		streamflow = 0.0;
		Qin_total = 0.0;
		Qstr_total = 0.0;
		d = 0;
		// Note: this assumes that the set of patches in the surface routing table is identical to
		//       the set of patches in the subsurface flow table
		for (i = 0; i < basin->route_list->num_patches; i++) {
			patch = basin->route_list->list[i];
			/*//the following code is for testing only
			if (i==2000){
			printf("sat_deficit = %f, soil_dep = %f, n_0 = %f, p = %f\n",
				patch[0].sat_deficit,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay);
			}*/
			patch[0].streamflow = 0.0;
			patch[0].return_flow = 0.0;
			patch[0].base_flow = 0.0;
			patch[0].infiltration_excess = 0.0;
			basin[0].preday_basin_rz_storage += patch[0].rz_storage * patch[0].area;
			basin[0].preday_basin_unsat_storage += patch[0].unsat_storage * patch[0].area;
			basin[0].preday_basin_sat_deficit += patch[0].sat_deficit * patch[0].area;
			basin[0].preday_basin_return_flow += patch[0].return_flow * patch[0].area;
			basin[0].preday_basin_detention_store += patch[0].detention_store
					* patch[0].area;
			basin[0].basin_area += patch[0].area;
			patch[0].Qin_total = 0.0;
			patch[0].Qout_total = 0.0;
			patch[0].Qin = 0.0;
			patch[0].Qout = 0.0;
			patch[0].surface_Qin = 0.0;
			patch[0].surface_Qout = 0.0;

			patch[0].preday_sat_deficit = patch[0].sat_deficit;

			patch[0].preday_sat_deficit_z = compute_z_final(verbose_flag,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].soil_depth, 0.0,
					-1.0 * patch[0].sat_deficit);

			patch[0].interim_sat = patch[0].sat_deficit - patch[0].unsat_storage;
			if ((patch[0].sat_deficit - patch[0].unsat_storage) < ZERO)
				patch[0].S = 1.0;
			else
				patch[0].S = patch[0].unsat_storage / patch[0].sat_deficit;

			if (grow_flag > 0) {
				patch[0].soil_ns.NO3_Qin = 0.0;
				patch[0].soil_ns.NO3_Qout = 0.0;
				patch[0].soil_ns.NH4_Qin = 0.0;
				patch[0].soil_ns.NH4_Qout = 0.0;
				patch[0].soil_ns.NO3_Qin_total = 0.0;
				patch[0].soil_ns.NO3_Qout_total = 0.0;
				patch[0].soil_ns.NH4_Qin_total = 0.0;
				patch[0].soil_ns.NH4_Qout_total = 0.0;
				patch[0].streamflow_DON = 0.0;
				patch[0].streamflow_DOC = 0.0;
				patch[0].streamflow_NO3 = 0.0;
				patch[0].streamflow_NH4 = 0.0;
				patch[0].soil_ns.DON_Qin_total = 0.0;
				patch[0].soil_ns.DON_Qout_total = 0.0;
				patch[0].soil_cs.DOC_Qin_total = 0.0;
				patch[0].soil_cs.DOC_Qout_total = 0.0;
				patch[0].surface_DON_Qin_total = 0.0;
				patch[0].surface_DON_Qout_total = 0.0;
				patch[0].surface_DOC_Qin_total = 0.0;
				patch[0].surface_DOC_Qout_total = 0.0;
				patch[0].soil_ns.leach = 0.0;
				patch[0].surface_ns_leach = 0.0;
				patch[0].soil_ns.DON_Qout = 0.0;
				patch[0].soil_ns.DON_Qin = 0.0;
				patch[0].soil_cs.DOC_Qout = 0.0;
				patch[0].soil_cs.DOC_Qin = 0.0;
				patch[0].surface_DON_Qout = 0.0;
				patch[0].surface_DON_Qin = 0.0;
				patch[0].surface_DOC_Qout = 0.0;
				patch[0].surface_DOC_Qin = 0.0;
			
				patch[0].streamNO3_from_surface	= 0.0;
				patch[0].streamNO3_from_sub = 0.0;
			}
		}
	}
	/*--------------------------------------------------------------*/
	/*	calculate Qout for each patch and add appropriate	*/
	/*	proportion of subsurface outflow to each neighbour	*/
	/*--------------------------------------------------------------*/
		for (i = 0; i < basin->route_list->num_patches; i++) {
			patch = basin->route_list->list[i];
		      	patch[0].hourly_subsur2stream_flow = 0;
			patch[0].hourly_sur2stream_flow = 0;
			patch[0].hourly_stream_flow = 0;
			patch[0].hourly[0].streamflow_NO3 = 0;
			patch[0].hourly[0].streamflow_NO3_from_sub = 0;
			patch[0].hourly[0].streamflow_NO3_from_surface = 0;
		}

		for (i = 0; i < basin->route_list->num_patches; i++) {
			patch = basin->route_list->list[i];
			litter=&(patch[0].litter);
			/*--------------------------------------------------------------*/
			/*	for roads, saturated throughflow beneath road cut	*/
			/*	is routed to downslope patches; saturated throughflow	*/
			/*	above the cut and overland flow is routed to the stream	*/
			/*								*/
			/*	for streams, no routing - all exported from basin	*/
			/*								*/
			/*	regular land patches - route to downslope neighbours    */
			/*--------------------------------------------------------------*/
			if ((patch[0].drainage_type == ROAD)
					&& (command_line[0].road_flag == 1)) {
			  // in this part, the patch[0].hourly_sur2stream_flow will be updated
				update_drainage_road(patch, command_line, time_int,
						verbose_flag);
			} else if (patch[0].drainage_type == STREAM) {
			  // in this part, the patch[0].hourly_subsur2stream_flow and hourly_sur2stream_flow will be updated
				update_drainage_stream(patch, command_line, time_int,
						verbose_flag);
			} else {
				update_drainage_land(patch, command_line, time_int,
						verbose_flag);
			}

			

		} /* end i */

		/*--------------------------------------------------------------*/
		/*	update soil moisture and nitrogen stores		*/
		/*	check water balance					*/
		/*--------------------------------------------------------------*/
		for (i = 0; i < basin->route_list->num_patches; i++) {
			patch = basin->route_list->list[i];

			/*--------------------------------------------------------------*/
			/*	update subsurface 				*/
			/*-------------------------------------------------------------------------*/

			/*-------------------------------------------------------------------------*/
			/*	Recompute current actual depth to water table				*/
			/*-------------------------------------------------------------------------*/
			patch[0].sat_deficit += (patch[0].Qout - patch[0].Qin); // this part need to put into some where else

			patch[0].sat_deficit_z = compute_z_final(verbose_flag,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].soil_depth, 0.0,
					-1.0 * patch[0].sat_deficit);

			if (grow_flag > 0) {
				patch[0].soil_ns.nitrate += (patch[0].soil_ns.NO3_Qin
						- patch[0].soil_ns.NO3_Qout);
				patch[0].soil_ns.sminn += (patch[0].soil_ns.NH4_Qin
						- patch[0].soil_ns.NH4_Qout);
				patch[0].soil_cs.DOC += (patch[0].soil_cs.DOC_Qin
						- patch[0].soil_cs.DOC_Qout);
				patch[0].soil_ns.DON += (patch[0].soil_ns.DON_Qin
						- patch[0].soil_ns.DON_Qout);
			}

			/*--------------------------------------------------------------*/
			/*      Recompute 	soil moisture storage                   */
			/*--------------------------------------------------------------*/

			if (patch[0].sat_deficit > patch[0].rootzone.potential_sat) {
				patch[0].rootzone.S =
						min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
				patch[0].S = patch[0].unsat_storage
						/ (patch[0].sat_deficit
								- patch[0].rootzone.potential_sat);
			} else {
				patch[0].rootzone.S =
						min((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)
								/ patch[0].rootzone.potential_sat, 1.0);
				patch[0].S =
						min(patch[0].rz_storage / patch[0].sat_deficit, 1.0);
			}

			/*--------------------------------------------------------------*/
			/*	reset iterative  patch fluxes to zero			*/
			/*--------------------------------------------------------------*/
			patch[0].soil_ns.leach += (patch[0].soil_ns.DON_Qout
					+ patch[0].soil_ns.NH4_Qout + patch[0].soil_ns.NO3_Qout
					- patch[0].soil_ns.NH4_Qin - patch[0].soil_ns.NO3_Qin
					- patch[0].soil_ns.DON_Qin);
			patch[0].surface_ns_leach += ((patch[0].surface_NO3_Qout
					- patch[0].surface_NO3_Qin)
					+ (patch[0].surface_NH4_Qout - patch[0].surface_NH4_Qin)
					+ (patch[0].surface_DON_Qout - patch[0].surface_DON_Qin));
			patch[0].Qin_total += patch[0].Qin + patch[0].surface_Qin;
			patch[0].Qout_total += patch[0].Qout + patch[0].surface_Qout;

			patch[0].surface_Qin = 0.0;
			patch[0].surface_Qout = 0.0;
			patch[0].Qin = 0.0;
			patch[0].Qout = 0.0;
			if (grow_flag > 0) {
				patch[0].soil_cs.DOC_Qin_total += patch[0].soil_cs.DOC_Qin;
				patch[0].soil_cs.DOC_Qout_total += patch[0].soil_cs.DOC_Qout;
				patch[0].soil_ns.NH4_Qin_total += patch[0].soil_ns.NH4_Qin;
				patch[0].soil_ns.NH4_Qout_total += patch[0].soil_ns.NH4_Qout;
				patch[0].soil_ns.NO3_Qin_total += patch[0].soil_ns.NO3_Qin;
				patch[0].soil_ns.NO3_Qout_total += patch[0].soil_ns.NO3_Qout;
				patch[0].soil_ns.DON_Qin_total += patch[0].soil_ns.DON_Qin;
				patch[0].soil_ns.DON_Qout_total += patch[0].soil_ns.DON_Qout;
				patch[0].surface_DON_Qin_total += patch[0].surface_DON_Qin;
				patch[0].surface_DON_Qout_total += patch[0].surface_DON_Qout;
				patch[0].surface_DOC_Qin_total += patch[0].surface_DOC_Qin;
				patch[0].surface_DOC_Qout_total += patch[0].surface_DOC_Qout;

				patch[0].soil_ns.NH4_Qin = 0.0;
				patch[0].soil_ns.NH4_Qout = 0.0;
				patch[0].soil_ns.NO3_Qin = 0.0;
				patch[0].soil_ns.NO3_Qout = 0.0;
				patch[0].soil_ns.DON_Qout = 0.0;
				patch[0].soil_ns.DON_Qin = 0.0;
				patch[0].soil_cs.DOC_Qout = 0.0;
				patch[0].soil_cs.DOC_Qin = 0.0;
				patch[0].surface_NH4_Qout = 0.0;
				patch[0].surface_NH4_Qin = 0.0;
				patch[0].surface_NO3_Qout = 0.0;
				patch[0].surface_NO3_Qin = 0.0;
				patch[0].surface_DON_Qout = 0.0;
				patch[0].surface_DON_Qin = 0.0;
				patch[0].surface_DOC_Qout = 0.0;
				patch[0].surface_DOC_Qin = 0.0;

			}
			/*--------------------------------------------------------------*/
			/*	finalize streamflow and saturation deficits		*/
			/*								*/
			/*	move any saturation excess into detention store		*/
			/*	(i.e this needs to be routed on the next time step)	*/
			/* 	some streamflow may have already been accumulated from 	*/
			/* 	redirected streamflow					*/
			/*	water balance calculations				*/
			/* only on last iteration					*/
			/* **** note that streamflow is updated sequentially		*/
			/*	i.e not at the end; it is similar to Qout, in		*/
			/*	that it accumulates flux in from patches		*/
			/*	(roads) that direct water to the stream			*/
			/*--------------------------------------------------------------*/


			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/			
			/* in the new version of rhessys, we delete the finalize streamflow and saturation deficits */
			/* because it is already counted in the update_drainage and patch_hourly.c		    */
			/* double counting should be deleted.*/

			/* ******************************** */
			/* accumulate the daily returnflow and baseflow calculated from update_drainage*/
			/* The N calculation has been completed in update_drainage_***.c routing*/
			if (current_date.hour == n_timesteps){
				    if (patch[0].drainage_type == STREAM) {
					patch[0].streamflow += patch[0].return_flow
							+ patch[0].base_flow;
				    }
			}
			/* ******************************** this is done by each hourly*/
			patch[0].hourly_stream_flow += patch[0].hourly_subsur2stream_flow
		      				+ patch[0].hourly_sur2stream_flow;


			basin[0].basin_return_flow += (patch[0].return_flow) * patch[0].area;
			// it is not the sum of hourly return_flow, the patch[0].return_flow is already the sum of hourly return_flow from
			// hour 0 to current_date.hour 
	

		} /* end i */


	basin[0].basin_outflow /= basin[0].basin_area;
	basin[0].preday_basin_rz_storage /= basin[0].basin_area;
	basin[0].preday_basin_unsat_storage /= basin[0].basin_area;
	basin[0].preday_basin_detention_store /= basin[0].basin_area;
	basin[0].preday_basin_sat_deficit /= basin[0].basin_area;
	basin[0].basin_rz_storage /= basin[0].basin_area;
	basin[0].basin_unsat_storage /= basin[0].basin_area;
	basin[0].basin_detention_store /= basin[0].basin_area;
	basin[0].basin_sat_deficit /= basin[0].basin_area;
	water_balance = basin[0].preday_basin_rz_storage + basin[0].preday_basin_unsat_storage
			+ basin[0].preday_basin_detention_store - basin[0].preday_basin_sat_deficit
			- (basin[0].basin_rz_storage + basin[0].basin_unsat_storage + basin[0].basin_detention_store
					- basin[0].basin_sat_deficit) - basin[0].basin_outflow;
/*
	if (abs(water_balance) > ZERO)
		printf("\n Water Balance is %lf for %ld %ld %ld", water_balance, current_date.day, current_date.month, current_date.year);
*/	
	if ((command_line[0].output_flags.yearly == 1)
			&& (command_line[0].b != NULL )) {
		if (basin[0].basin_outflow <= command_line[0].thresholds[STREAMFLOW])
			basin[0].acc_year.num_threshold += 1;
	}

	return;

} /*end compute_subsurface_routing_hourly.c*/


