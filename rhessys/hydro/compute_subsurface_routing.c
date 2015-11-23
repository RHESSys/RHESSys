/*--------------------------------------------------------------*/
/* 											*/
/*					compute_subsurface_routing			*/
/*											*/
/*	compute_subsurface_routing.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	compute_subsurface_routing.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	struct routing_list_object compute_subsurface_routing( 				*/
/*							struct command_line_object command */
/*							struct basin_object *basinn)	*/
/*				 			int,			 	*/
/*							struct date *current_date)	*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*											*/
/*											*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*	June 16, 98 C.Tague								*/
/*	limit drainage to maximum saturation deficit defined by soil depth		*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void compute_subsurface_routing(struct command_line_object *command_line,
		struct basin_object *basin, int n_timesteps, struct date current_date) {
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

	double	compute_N_leached(int verbose_flag,
				double total_nitrate,
				double Qout,
				double s1,
				double s2,
				double m,
				double gamma,
				double n_0,
				double p,
				double N_decay_rate,
				double z2_N,
				double z2_water,
				double N_absorption_rate,
				double *transmissivity);

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
	double scale;
	double theta, m, Ksat, Nout;
	double NO3_out, NH4_out, DON_out, DOC_out;
	double return_flow, excess;
	double water_balance, infiltration;
	double innundation_depth;
	double basin_outflow;
	double basin_rz_storage;
	double basin_unsat_storage;
	double basin_sat_deficit;
	double basin_return_flow;
	double basin_detention_store;
	double basin_area;
	double preday_basin_unsat_storage;
	double preday_basin_rz_storage;
	double preday_basin_sat_deficit;
	double preday_sat_deficit;
	double preday_basin_return_flow;
	double preday_basin_detention_store;
	double add_field_capacity, rz_drainage, unsat_drainage;
	double streamflow, Qout, Qin_total, Qstr_total;
	struct patch_object *patch;
	struct hillslope_object *hillslope;
	struct patch_object *neigh;
	/*--------------------------------------------------------------*/
	/*	initializations						*/
	/*--------------------------------------------------------------*/
	grow_flag = command_line[0].grow_flag;
	verbose_flag = command_line[0].verbose_flag;

	time_int = 1.0 / n_timesteps;
	basin_outflow = 0.0;
	basin_area = 0.0;
	basin_unsat_storage = 0.0;
	basin_rz_storage = 0.0;
	basin_sat_deficit = 0.0;
	basin_return_flow = 0.0;
	basin_detention_store = 0.0;
	preday_basin_rz_storage = 0.0;
	preday_basin_unsat_storage = 0.0;
	preday_basin_sat_deficit = 0.0;
	preday_basin_return_flow = 0.0;
	preday_basin_detention_store = 0.0;
	streamflow = 0.0;
	Qin_total = 0.0;
	Qstr_total = 0.0;
	d = 0;
	// Note: this assumes that the set of patches in the surface routing table is identical to
	//       the set of patches in the subsurface flow table
	for (i = 0; i < basin->route_list->num_patches; i++) {
		patch = basin->route_list->list[i];
		patch[0].streamflow = 0.0;
		patch[0].return_flow = 0.0;
		patch[0].base_flow = 0.0;
		patch[0].infiltration_excess = 0.0;
		preday_basin_rz_storage += patch[0].rz_storage * patch[0].area;
		preday_basin_unsat_storage += patch[0].unsat_storage * patch[0].area;
		preday_basin_sat_deficit += patch[0].sat_deficit * patch[0].area;
		preday_basin_return_flow += patch[0].return_flow * patch[0].area;
		preday_basin_detention_store += patch[0].detention_store
				* patch[0].area;
		basin_area += patch[0].area;
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

		}
	}

	/*--------------------------------------------------------------*/
	/*	calculate Qout for each patch and add appropriate	*/
	/*	proportion of subsurface outflow to each neighbour	*/
	/*--------------------------------------------------------------*/
	for (k = 0; k < n_timesteps; k++) {
		for (i = 0; i < basin->route_list->num_patches; i++) {
			patch = basin->route_list->list[i];
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
				update_drainage_road(patch, command_line, time_int,
						verbose_flag);
			} else if (patch[0].drainage_type == STREAM) {
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
			patch[0].sat_deficit += (patch[0].Qout - patch[0].Qin);

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
						MIN(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
				patch[0].S = patch[0].unsat_storage
						/ (patch[0].sat_deficit
								- patch[0].rootzone.potential_sat);
			} else {
				patch[0].rootzone.S =
						MIN((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)
								/ patch[0].rootzone.potential_sat, 1.0);
				patch[0].S =
						MIN(patch[0].rz_storage / patch[0].sat_deficit, 1.0);
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
			if (k == (n_timesteps - 1)) {

				if ((patch[0].sat_deficit
						- (patch[0].unsat_storage + patch[0].rz_storage))
						< -1.0 * ZERO) {
					excess = -1.0
							* (patch[0].sat_deficit - patch[0].unsat_storage
									- patch[0].rz_storage);
					patch[0].detention_store += excess;
					patch[0].sat_deficit = 0.0;
					patch[0].unsat_storage = 0.0;
					patch[0].rz_storage = 0.0;

					if (grow_flag > 0) {
						Nout =
								compute_N_leached(verbose_flag,
										patch[0].soil_cs.DOC, excess, 0.0, 0.0,
										patch[0].m,
										patch[0].innundation_list[d].gamma
												/ patch[0].area * time_int,
										patch[0].soil_defaults[0][0].porosity_0,
										patch[0].soil_defaults[0][0].porosity_decay,
										patch[0].soil_defaults[0][0].DOM_decay_rate,
										patch[0].soil_defaults[0][0].active_zone_z,
										patch[0].soil_defaults[0][0].soil_depth,
										patch[0].soil_defaults[0][0].DOC_adsorption_rate,
										patch[0].transmissivity_profile);
						patch[0].surface_DOC += Nout;
						patch[0].soil_cs.DOC -= Nout;
					}
					if (grow_flag > 0) {
						Nout =
								compute_N_leached(verbose_flag,
										patch[0].soil_ns.DON, excess, 0.0, 0.0,
										patch[0].m,
										patch[0].innundation_list[d].gamma
												/ patch[0].area * time_int,
										patch[0].soil_defaults[0][0].porosity_0,
										patch[0].soil_defaults[0][0].porosity_decay,
										patch[0].soil_defaults[0][0].DOM_decay_rate,
										patch[0].soil_defaults[0][0].active_zone_z,
										patch[0].soil_defaults[0][0].soil_depth,
										patch[0].soil_defaults[0][0].DON_adsorption_rate,
										patch[0].transmissivity_profile);
						patch[0].surface_DON += Nout;
						patch[0].soil_ns.DON -= Nout;
					}
					if (grow_flag > 0) {
						Nout =
								compute_N_leached(verbose_flag,
										patch[0].soil_ns.nitrate, excess, 0.0,
										0.0, patch[0].m,
										patch[0].innundation_list[d].gamma
												/ patch[0].area * time_int,
										patch[0].soil_defaults[0][0].porosity_0,
										patch[0].soil_defaults[0][0].porosity_decay,
										patch[0].soil_defaults[0][0].N_decay_rate,
										patch[0].soil_defaults[0][0].active_zone_z,
										patch[0].soil_defaults[0][0].soil_depth,
										patch[0].soil_defaults[0][0].NO3_adsorption_rate,
										patch[0].transmissivity_profile);
						patch[0].surface_NO3 += Nout;
						patch[0].soil_ns.nitrate -= Nout;
					}

					if (grow_flag > 0) {
						Nout =
								compute_N_leached(verbose_flag,
										patch[0].soil_ns.sminn, excess, 0.0,
										0.0, patch[0].m,
										patch[0].innundation_list[d].gamma
												/ patch[0].area * time_int,
										patch[0].soil_defaults[0][0].porosity_0,
										patch[0].soil_defaults[0][0].porosity_decay,
										patch[0].soil_defaults[0][0].N_decay_rate,
										patch[0].soil_defaults[0][0].active_zone_z,
										patch[0].soil_defaults[0][0].soil_depth,
										patch[0].soil_defaults[0][0].NH4_adsorption_rate,
										patch[0].transmissivity_profile);
						patch[0].surface_NH4 += Nout;
						patch[0].soil_ns.sminn -= Nout;
					}
				}

				/*--------------------------------------------------------------*/
				/*	final overland flow routing				*/
				/*--------------------------------------------------------------*/
				if (((excess = patch[0].detention_store
						- patch[0].soil_defaults[0][0].detention_store_size)
						> ZERO) && (patch[0].detention_store > ZERO)) {

					if (patch[0].drainage_type == STREAM) {
						if (grow_flag > 0) {
							patch[0].streamflow_DON += (excess
									/ patch[0].detention_store)
									* patch[0].surface_DON;
							patch[0].streamflow_DOC += (excess
									/ patch[0].detention_store)
									* patch[0].surface_DOC;
							patch[0].streamflow_NO3 += (excess
									/ patch[0].detention_store)
									* patch[0].surface_NO3;
							patch[0].streamflow_NH4 += (excess
									/ patch[0].detention_store)
									* patch[0].surface_NH4;
							patch[0].surface_DON -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_DON;
							patch[0].surface_DOC -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_DOC;
							patch[0].surface_NO3 -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_NO3;
							patch[0].surface_NH4 -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_NH4;
						}
						patch[0].return_flow += excess;
						patch[0].detention_store -= excess;
						patch[0].Qout_total += excess;
					} else {
						/*--------------------------------------------------------------*/
						/* determine which innundation depth to consider		*/
						/*--------------------------------------------------------------*/
						if (patch[0].num_innundation_depths > 0) {
							innundation_depth = patch[0].detention_store;
							d = 0;
							while ((innundation_depth
									> patch[0].innundation_list[d].critical_depth)
									&& (d < patch[0].num_innundation_depths - 1)) {
								d++;
							}
						} else {
							d = 0;
						}

						for (j = 0; j < patch->surface_innundation_list[d].num_neighbours; j++) {
							neigh = patch->surface_innundation_list[d].neighbours[j].patch;
							Qout = excess * patch->surface_innundation_list[d].neighbours[j].gamma;
							if (grow_flag > 0) {
								NO3_out = Qout / patch[0].detention_store
										* patch[0].surface_NO3;
								NH4_out = Qout / patch[0].detention_store
										* patch[0].surface_NH4;
								DON_out = Qout / patch[0].detention_store
										* patch[0].surface_DON;
								DOC_out = Qout / patch[0].detention_store
										* patch[0].surface_DOC;
								Nout = NO3_out + NH4_out + DON_out;
							}
							if (neigh[0].drainage_type == STREAM) {
								neigh[0].Qin_total += Qout * patch[0].area
										/ neigh[0].area;
								neigh[0].return_flow += Qout * patch[0].area
										/ neigh[0].area;
								if (grow_flag > 0) {
									neigh[0].streamflow_DOC += (DOC_out
											* patch[0].area / neigh[0].area);
									neigh[0].streamflow_DON += (DON_out
											* patch[0].area / neigh[0].area);
									neigh[0].streamflow_NO3 += (NO3_out
											* patch[0].area / neigh[0].area);
									neigh[0].streamflow_NH4 += (NH4_out
											* patch[0].area / neigh[0].area);
									neigh[0].surface_ns_leach += (Nout
											* patch[0].area / neigh[0].area);
								}
							} else {
								neigh[0].Qin_total += Qout * patch[0].area
										/ neigh[0].area;
								neigh[0].detention_store += Qout * patch[0].area
										/ neigh[0].area;
								if (grow_flag > 0) {
									neigh[0].surface_DOC += (DOC_out
											* patch[0].area / neigh[0].area);
									neigh[0].surface_DON += (DON_out
											* patch[0].area / neigh[0].area);
									neigh[0].surface_NO3 += (NO3_out
											* patch[0].area / neigh[0].area);
									neigh[0].surface_ns_leach -= (Nout
											* patch[0].area / neigh[0].area);
									neigh[0].surface_NH4 += (NH4_out
											* patch[0].area / neigh[0].area);

								}

							}
						}
						if (grow_flag > 0) {
							patch[0].surface_DOC -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_DOC;
							patch[0].surface_DON -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_DON;
							patch[0].surface_NO3 -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_NO3;
							patch[0].surface_NH4 -= (excess
									/ patch[0].detention_store)
									* patch[0].surface_NH4;
							patch[0].surface_ns_leach += (excess
									/ patch[0].detention_store)
									* patch[0].surface_NO3;
						}
						patch[0].detention_store -= excess;
						patch[0].Qout_total += excess;
					}
				}

				/*-------------------------------------------------------------------------*/
				/*Recompute current actual depth to water table				*/
				/*-------------------------------------------------------------------------*/
				patch[0].sat_deficit_z = compute_z_final(verbose_flag,
						patch[0].soil_defaults[0][0].porosity_0,
						patch[0].soil_defaults[0][0].porosity_decay,
						patch[0].soil_defaults[0][0].soil_depth, 0.0,
						-1.0 * patch[0].sat_deficit);

				/*--------------------------------------------------------------*/
				/* 	leave behind field capacity			*/
				/*	if sat deficit has been lowered			*/
				/*	this should be an interactive process, we will use 	*/
				/*	0th order approximation					*/
				/* 	we do not do this once sat def is below 0.9 soil depth	*/
				/*     we use 0.9 to prevent numerical instability		*/
				/*--------------------------------------------------------------*/
				if ((patch[0].sat_deficit_z > patch[0].preday_sat_deficit_z)
						&& (patch[0].sat_deficit_z
								< patch[0].soil_defaults[0][0].soil_depth * 0.9)) {
					add_field_capacity = compute_layer_field_capacity(
							command_line[0].verbose_flag,
							patch[0].soil_defaults[0][0].theta_psi_curve,
							patch[0].soil_defaults[0][0].psi_air_entry,
							patch[0].soil_defaults[0][0].pore_size_index,
							patch[0].soil_defaults[0][0].p3,
							patch[0].soil_defaults[0][0].p4,
							patch[0].soil_defaults[0][0].porosity_0,
							patch[0].soil_defaults[0][0].porosity_decay,
							patch[0].sat_deficit_z, patch[0].sat_deficit_z,
							patch[0].preday_sat_deficit_z);

					add_field_capacity = MAX(add_field_capacity, 0.0);
					patch[0].sat_deficit += add_field_capacity;

					if ((patch[0].sat_deficit_z > patch[0].rootzone.depth)
							&& (patch[0].preday_sat_deficit_z
									> patch[0].rootzone.depth))
						patch[0].unsat_storage += add_field_capacity;
					else
						patch[0].rz_storage += add_field_capacity;
				}

				if (patch[0].rootzone.depth > ZERO) {
					if ((patch[0].sat_deficit > ZERO)
							&& (patch[0].rz_storage == 0.0)) {
						add_field_capacity = compute_layer_field_capacity(
								command_line[0].verbose_flag,
								patch[0].soil_defaults[0][0].theta_psi_curve,
								patch[0].soil_defaults[0][0].psi_air_entry,
								patch[0].soil_defaults[0][0].pore_size_index,
								patch[0].soil_defaults[0][0].p3,
								patch[0].soil_defaults[0][0].p4,
								patch[0].soil_defaults[0][0].porosity_0,
								patch[0].soil_defaults[0][0].porosity_decay,
								patch[0].sat_deficit_z, patch[0].sat_deficit_z,
								0.0);
						add_field_capacity = MAX(add_field_capacity, 0.0);
						patch[0].sat_deficit += add_field_capacity;
						patch[0].rz_storage += add_field_capacity;
					}
				} else {
					if ((patch[0].sat_deficit > ZERO)
							&& (patch[0].unsat_storage == 0.0)) {
						add_field_capacity = compute_layer_field_capacity(
								command_line[0].verbose_flag,
								patch[0].soil_defaults[0][0].theta_psi_curve,
								patch[0].soil_defaults[0][0].psi_air_entry,
								patch[0].soil_defaults[0][0].pore_size_index,
								patch[0].soil_defaults[0][0].p3,
								patch[0].soil_defaults[0][0].p4,
								patch[0].soil_defaults[0][0].porosity_0,
								patch[0].soil_defaults[0][0].porosity_decay,
								patch[0].sat_deficit_z, patch[0].sat_deficit_z,
								0.0);
						add_field_capacity = MAX(add_field_capacity, 0.0);
						patch[0].sat_deficit += add_field_capacity;
						patch[0].unsat_storage += add_field_capacity;
					}
				}

				/*--------------------------------------------------------------*/
				/* try to infiltrate this water					*/
				/* use time_int as duration */
				/*--------------------------------------------------------------*/

				if (patch[0].detention_store > ZERO)
					if (patch[0].rootzone.depth > ZERO) {
						infiltration = compute_infiltration(verbose_flag,
								patch[0].sat_deficit_z, patch[0].rootzone.S,
								patch[0].Ksat_vertical,
								patch[0].soil_defaults[0][0].Ksat_0_v,
								patch[0].soil_defaults[0][0].mz_v,
								patch[0].soil_defaults[0][0].porosity_0,
								patch[0].soil_defaults[0][0].porosity_decay,
								(patch[0].detention_store), time_int,
								patch[0].soil_defaults[0][0].psi_air_entry);
					} else {
						infiltration = compute_infiltration(verbose_flag,
								patch[0].sat_deficit_z, patch[0].S,
								patch[0].Ksat_vertical,
								patch[0].soil_defaults[0][0].Ksat_0_v,
								patch[0].soil_defaults[0][0].mz_v,
								patch[0].soil_defaults[0][0].porosity_0,
								patch[0].soil_defaults[0][0].porosity_decay,
								(patch[0].detention_store), time_int,
								patch[0].soil_defaults[0][0].psi_air_entry);
					}
				else
					infiltration = 0.0;
				/*--------------------------------------------------------------*/
				/* added an surface N flux to surface N pool	and		*/
				/* allow infiltration of surface N				*/
				/*--------------------------------------------------------------*/
				if ((grow_flag > 0) && (infiltration > ZERO)) {
					patch[0].soil_ns.DON += ((infiltration
							/ patch[0].detention_store) * patch[0].surface_DON);
					patch[0].soil_cs.DOC += ((infiltration
							/ patch[0].detention_store) * patch[0].surface_DOC);
					patch[0].soil_ns.nitrate += ((infiltration
							/ patch[0].detention_store) * patch[0].surface_NO3);
					patch[0].surface_NO3 -= ((infiltration
							/ patch[0].detention_store) * patch[0].surface_NO3);
					patch[0].soil_ns.sminn += ((infiltration
							/ patch[0].detention_store) * patch[0].surface_NH4);
					patch[0].surface_NH4 -= ((infiltration
							/ patch[0].detention_store) * patch[0].surface_NH4);
					patch[0].surface_DOC -= ((infiltration
							/ patch[0].detention_store) * patch[0].surface_DOC);
					patch[0].surface_DON -= ((infiltration
							/ patch[0].detention_store) * patch[0].surface_DON);
				}

				/*--------------------------------------------------------------*/
				/*	Determine if the infifltration will fill up the unsat	*/
				/*	zone or not.						*/
				/*	We use the strict assumption that sat deficit is the	*/
				/*	amount of water needed to saturate the soil.		*/
				/*--------------------------------------------------------------*/

				if (infiltration
						> patch[0].sat_deficit - patch[0].unsat_storage
								- patch[0].rz_storage) {
					/*--------------------------------------------------------------*/
					/*		Yes the unsat zone will be filled so we may	*/
					/*		as well treat the unsat_storage and infiltration*/
					/*		as water added to the water table.		*/
					/*--------------------------------------------------------------*/
					patch[0].sat_deficit -= (infiltration
							+ patch[0].unsat_storage + patch[0].rz_storage);
					/*--------------------------------------------------------------*/
					/*		There is no unsat_storage left.			*/
					/*--------------------------------------------------------------*/
					patch[0].unsat_storage = 0.0;
					patch[0].rz_storage = 0.0;
					patch[0].field_capacity = 0.0;
					patch[0].rootzone.field_capacity = 0.0;
				} else if ((patch[0].sat_deficit
						> patch[0].rootzone.potential_sat)
						&& (infiltration
								> patch[0].rootzone.potential_sat
										- patch[0].rz_storage)) {
					/*------------------------------------------------------------------------------*/
					/*		Just add the infiltration to the rz_storage and unsat_storage	*/
					/*------------------------------------------------------------------------------*/
					patch[0].unsat_storage += infiltration
							- (patch[0].rootzone.potential_sat
									- patch[0].rz_storage);
					patch[0].rz_storage = patch[0].rootzone.potential_sat;
				}
				/* Only rootzone layer saturated - perched water table case */
				else if ((patch[0].sat_deficit > patch[0].rootzone.potential_sat)
						&& (infiltration
								<= patch[0].rootzone.potential_sat
										- patch[0].rz_storage)) {
					/*--------------------------------------------------------------*/
					/*		Just add the infiltration to the rz_storage	*/
					/*--------------------------------------------------------------*/
					patch[0].rz_storage += infiltration;
				}

				else if ((patch[0].sat_deficit
						<= patch[0].rootzone.potential_sat)
						&& (infiltration
								<= patch[0].sat_deficit - patch[0].rz_storage
										- patch[0].unsat_storage)) {
					patch[0].rz_storage += patch[0].unsat_storage;
					/* transfer left water in unsat storage to rootzone layer */
					patch[0].unsat_storage = 0;
					patch[0].rz_storage += infiltration;
					patch[0].field_capacity = 0;
				}

				if (patch[0].sat_deficit < 0.0) {
					patch[0].detention_store -= (patch[0].sat_deficit
							- patch[0].unsat_storage);
					patch[0].sat_deficit = 0.0;
					patch[0].unsat_storage = 0.0;
				}

				patch[0].detention_store -= infiltration;
				/*--------------------------------------------------------------*/
				/* recompute saturation deficit					*/
				/*--------------------------------------------------------------*/
				patch[0].sat_deficit_z = compute_z_final(verbose_flag,
						patch[0].soil_defaults[0][0].porosity_0,
						patch[0].soil_defaults[0][0].porosity_decay,
						patch[0].soil_defaults[0][0].soil_depth, 0.0,
						-1.0 * patch[0].sat_deficit);

				/*--------------------------------------------------------------*/
				/*	compute new field capacity				*/
				/*--------------------------------------------------------------*/
				if (patch[0].sat_deficit_z < patch[0].rootzone.depth) {
					patch[0].rootzone.field_capacity =
							compute_layer_field_capacity(
									command_line[0].verbose_flag,
									patch[0].soil_defaults[0][0].theta_psi_curve,
									patch[0].soil_defaults[0][0].psi_air_entry,
									patch[0].soil_defaults[0][0].pore_size_index,
									patch[0].soil_defaults[0][0].p3,
									patch[0].soil_defaults[0][0].p4,
									patch[0].soil_defaults[0][0].porosity_0,
									patch[0].soil_defaults[0][0].porosity_decay,
									patch[0].sat_deficit_z,
									patch[0].rootzone.depth, 0.0);

					patch[0].field_capacity = 0.0;
				} else {

					patch[0].rootzone.field_capacity =
							compute_layer_field_capacity(
									command_line[0].verbose_flag,
									patch[0].soil_defaults[0][0].theta_psi_curve,
									patch[0].soil_defaults[0][0].psi_air_entry,
									patch[0].soil_defaults[0][0].pore_size_index,
									patch[0].soil_defaults[0][0].p3,
									patch[0].soil_defaults[0][0].p4,
									patch[0].soil_defaults[0][0].porosity_0,
									patch[0].soil_defaults[0][0].porosity_decay,
									patch[0].sat_deficit_z,
									patch[0].rootzone.depth, 0.0);

					patch[0].field_capacity = compute_layer_field_capacity(
							command_line[0].verbose_flag,
							patch[0].soil_defaults[0][0].theta_psi_curve,
							patch[0].soil_defaults[0][0].psi_air_entry,
							patch[0].soil_defaults[0][0].pore_size_index,
							patch[0].soil_defaults[0][0].p3,
							patch[0].soil_defaults[0][0].p4,
							patch[0].soil_defaults[0][0].porosity_0,
							patch[0].soil_defaults[0][0].porosity_decay,
							patch[0].sat_deficit_z, patch[0].sat_deficit_z, 0)
							- patch[0].rootzone.field_capacity;
				}

				/*--------------------------------------------------------------*/
				/*      Recompute patch soil moisture storage                   */
				/*--------------------------------------------------------------*/
				if (patch[0].sat_deficit < ZERO) {
					patch[0].S = 1.0;
					patch[0].rootzone.S = 1.0;
					rz_drainage = 0.0;
					unsat_drainage = 0.0;
				} else if (patch[0].sat_deficit_z > patch[0].rootzone.depth) { /* Constant vertical profile of soil porosity */

					/*-------------------------------------------------------*/
					/*	soil drainage and storage update	     	 */
					/*-------------------------------------------------------*/
					patch[0].rootzone.S =
							MIN(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
					rz_drainage = compute_unsat_zone_drainage(
							command_line[0].verbose_flag,
							patch[0].soil_defaults[0][0].theta_psi_curve,
							patch[0].soil_defaults[0][0].pore_size_index,
							patch[0].rootzone.S,
							patch[0].soil_defaults[0][0].mz_v,
							patch[0].rootzone.depth,
							patch[0].soil_defaults[0][0].Ksat_0 / n_timesteps
									/ 2,
							patch[0].rz_storage
									- patch[0].rootzone.field_capacity);

					patch[0].rz_storage -= rz_drainage;
					patch[0].unsat_storage += rz_drainage;

					patch[0].S =
							MIN(patch[0].unsat_storage / (patch[0].sat_deficit - patch[0].rootzone.potential_sat), 1.0);
					unsat_drainage = compute_unsat_zone_drainage(
							command_line[0].verbose_flag,
							patch[0].soil_defaults[0][0].theta_psi_curve,
							patch[0].soil_defaults[0][0].pore_size_index,
							patch[0].S, patch[0].soil_defaults[0][0].mz_v,
							patch[0].sat_deficit_z,
							patch[0].soil_defaults[0][0].Ksat_0 / n_timesteps
									/ 2,
							patch[0].unsat_storage - patch[0].field_capacity);

					patch[0].unsat_storage -= unsat_drainage;
					patch[0].sat_deficit -= unsat_drainage;
				} else {
					patch[0].sat_deficit -= patch[0].unsat_storage; /* transfer left water in unsat storage to rootzone layer */
					patch[0].unsat_storage = 0.0;

					patch[0].S =
							MIN(patch[0].rz_storage / patch[0].sat_deficit, 1.0);
					rz_drainage = compute_unsat_zone_drainage(
							command_line[0].verbose_flag,
							patch[0].soil_defaults[0][0].theta_psi_curve,
							patch[0].soil_defaults[0][0].pore_size_index,
							patch[0].S, patch[0].soil_defaults[0][0].mz_v,
							patch[0].sat_deficit_z,
							patch[0].soil_defaults[0][0].Ksat_0 / n_timesteps
									/ 2,
							patch[0].rz_storage
									- patch[0].rootzone.field_capacity);

					unsat_drainage = 0.0;

					patch[0].rz_storage -= rz_drainage;
					patch[0].sat_deficit -= rz_drainage;
				}

				patch[0].unsat_drainage += unsat_drainage;
				patch[0].rz_drainage += rz_drainage;

				if (patch[0].sat_deficit > patch[0].rootzone.potential_sat)
					patch[0].rootzone.S =
							MIN(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
				else
					patch[0].rootzone.S =
							MIN((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)
									/ patch[0].rootzone.potential_sat, 1.0);

				/*-------------------c------------------------------------------------------*/
				/*	Recompute current actual depth to water table				*/
				/*-------------------------------------------------------------------------*/
				patch[0].sat_deficit_z = compute_z_final(verbose_flag,
						patch[0].soil_defaults[0][0].porosity_0,
						patch[0].soil_defaults[0][0].porosity_decay,
						patch[0].soil_defaults[0][0].soil_depth, 0.0,
						-1.0 * patch[0].sat_deficit);

				/*--------------------------------------------------------------*/
				/* final stream flow calculations				*/
				/*--------------------------------------------------------------*/

				if (patch[0].drainage_type == STREAM) {
					patch[0].streamflow += patch[0].return_flow
							+ patch[0].base_flow;
				}
				basin_outflow += (patch[0].streamflow) * patch[0].area;
				basin_unsat_storage += patch[0].unsat_storage * patch[0].area;
				basin_sat_deficit += patch[0].sat_deficit * patch[0].area;
				basin_rz_storage += patch[0].rz_storage * patch[0].area;
				basin_detention_store += patch[0].detention_store
						* patch[0].area;

				/*---------------------------------------------------------------------*/
				/*update accumulator variables                                            */
				/*-----------------------------------------------------------------------*/
				patch[0].acc_year_trans += (patch[0].transpiration_unsat_zone
						+ patch[0].transpiration_sat_zone);
				if ((command_line[0].output_flags.monthly == 1)
						&& (command_line[0].b != NULL )) {
					scale = patch[0].area / basin[0].area;
					basin[0].acc_month.streamflow += (patch[0].streamflow)
							* scale;
					basin[0].acc_month.et += (patch[0].transpiration_unsat_zone
							+ patch[0].evaporation_surf
							+ patch[0].exfiltration_unsat_zone
							+ patch[0].exfiltration_sat_zone
							+ patch[0].transpiration_sat_zone
							+ patch[0].evaporation) * scale;
					basin[0].acc_month.denitrif += patch[0].ndf.denitrif
							* scale;
					basin[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate
							* scale;
					basin[0].acc_month.mineralized +=
							patch[0].ndf.net_mineralized * scale;
					basin[0].acc_month.uptake += patch[0].ndf.sminn_to_npool
							* scale;
					basin[0].acc_month.DON_loss +=
							(patch[0].soil_ns.DON_Qout_total
									- patch[0].soil_ns.DON_Qin_total) * scale;
					basin[0].acc_month.DOC_loss +=
							(patch[0].soil_cs.DOC_Qout_total
									- patch[0].soil_cs.DOC_Qin_total) * scale;
					basin[0].acc_month.length += 1;
					basin[0].acc_month.stream_NO3 += patch[0].streamflow_NO3
							* scale;
					basin[0].acc_month.stream_NH4 += patch[0].streamflow_NH4
							* scale;
					basin[0].acc_month.stream_DON += patch[0].streamflow_DON
							* scale;
					basin[0].acc_month.stream_DOC += patch[0].streamflow_DOC
							* scale;
					basin[0].acc_month.psn += patch[0].net_plant_psn * scale;
					basin[0].acc_month.lai += patch[0].lai * scale;
					basin[0].acc_month.leach += (patch[0].soil_ns.leach
							+ patch[0].surface_ns_leach) * scale;
				}

				if ((command_line[0].output_flags.yearly == 1)
						&& (command_line[0].b != NULL )) {
					scale = patch[0].area / basin[0].area;
					basin[0].acc_year.length += 1;
					basin[0].acc_year.leach += (patch[0].soil_ns.leach
							+ patch[0].surface_ns_leach) * scale;
					basin[0].acc_year.stream_NH4 += patch[0].streamflow_NH4
							* scale;
					basin[0].acc_year.stream_NO3 += patch[0].streamflow_NO3
							* scale;
					basin[0].acc_year.denitrif += patch[0].ndf.denitrif * scale;
					basin[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate
							* scale;
					basin[0].acc_year.mineralized +=
							patch[0].ndf.net_mineralized * scale;
					basin[0].acc_year.uptake += patch[0].ndf.sminn_to_npool
							* scale;
					basin[0].acc_year.DON_loss +=
							(patch[0].soil_ns.DON_Qout_total
									- patch[0].soil_ns.DON_Qin_total) * scale;
					basin[0].acc_year.DOC_loss +=
							(patch[0].soil_cs.DOC_Qout_total
									- patch[0].soil_cs.DOC_Qin_total) * scale;
					basin[0].acc_year.stream_DON += patch[0].streamflow_DON
							* scale;
					basin[0].acc_year.stream_DOC += patch[0].streamflow_DOC
							* scale;
					basin[0].acc_year.psn += patch[0].net_plant_psn * scale;
					basin[0].acc_year.PET += (patch[0].PE + patch[0].PET)
							* scale;

					basin[0].acc_year.et += (patch[0].evaporation
							+ patch[0].evaporation_surf
							+ patch[0].exfiltration_unsat_zone
							+ patch[0].exfiltration_sat_zone
							+ patch[0].transpiration_unsat_zone
							+ patch[0].transpiration_sat_zone) * scale;
					basin[0].acc_year.streamflow += (patch[0].streamflow)
							* scale;
					basin[0].acc_year.lai += patch[0].lai * scale;
				}

				if ((command_line[0].output_flags.monthly == 1)
						&& (command_line[0].p != NULL )) {
					patch[0].acc_month.theta += patch[0].rootzone.S;
					patch[0].acc_month.sm_deficit +=
							MAX(0.0,
									(patch[0].sat_deficit-patch[0].rz_storage-patch[0].unsat_storage));
					patch[0].acc_month.et += (patch[0].transpiration_unsat_zone
							+ patch[0].evaporation_surf
							+ patch[0].exfiltration_unsat_zone
							+ patch[0].exfiltration_sat_zone
							+ +patch[0].transpiration_sat_zone
							+ patch[0].evaporation);
					patch[0].acc_month.denitrif += patch[0].ndf.denitrif;
					patch[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate;
					patch[0].acc_month.mineralized +=
							patch[0].ndf.net_mineralized;
					patch[0].acc_month.uptake += patch[0].ndf.sminn_to_npool;
					patch[0].acc_month.DON_loss +=
							(patch[0].soil_ns.DON_Qout_total
									- patch[0].soil_ns.DON_Qout_total);
					patch[0].acc_month.DOC_loss +=
							(patch[0].soil_cs.DOC_Qout_total
									- patch[0].soil_cs.DOC_Qout_total);
					patch[0].acc_month.psn += patch[0].net_plant_psn;
					patch[0].acc_month.snowpack =
							MAX(patch[0].snowpack.water_equivalent_depth, patch[0].acc_month.snowpack);
					patch[0].acc_month.lai =
							MAX(patch[0].acc_month.lai, patch[0].lai);
					patch[0].acc_month.leach += (patch[0].soil_ns.leach
							+ patch[0].surface_ns_leach);
					patch[0].acc_month.length += 1;

				}
				if ((command_line[0].output_flags.yearly == 1)
						&& (command_line[0].p != NULL )) {
					patch[0].acc_year.length += 1;
					if ((patch[0].sat_deficit - patch[0].unsat_storage)
							> command_line[0].thresholds[SATDEF])
						patch[0].acc_year.num_threshold += 1;
					patch[0].acc_year.theta += patch[0].rootzone.S;
					patch[0].acc_year.denitrif += patch[0].ndf.denitrif;
					patch[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate;
					patch[0].acc_year.mineralized +=
							patch[0].ndf.net_mineralized;
					patch[0].acc_year.uptake += patch[0].ndf.sminn_to_npool;
					patch[0].acc_year.leach += (patch[0].soil_ns.leach
							+ patch[0].surface_ns_leach);
					patch[0].acc_year.DON_loss +=
							(patch[0].soil_ns.DON_Qout_total
									- patch[0].soil_ns.DON_Qout_total);
					patch[0].acc_year.DOC_loss +=
							(patch[0].soil_cs.DOC_Qout_total
									- patch[0].soil_cs.DOC_Qout_total);
					patch[0].acc_year.streamflow += patch[0].streamflow;
					patch[0].acc_year.Qout_total += patch[0].Qout_total;
					patch[0].acc_year.Qin_total += patch[0].Qin_total;
					patch[0].acc_year.psn += patch[0].net_plant_psn;
					patch[0].acc_year.PET += (patch[0].PE + patch[0].PET);
					patch[0].acc_year.burn += patch[0].burn;
					patch[0].acc_year.potential_recharge +=
							patch[0].rain_throughfall;
					patch[0].acc_year.potential_recharge_wyd +=
							patch[0].rain_throughfall
									* round(patch[0].acc_year.length);
					patch[0].acc_year.recharge += patch[0].recharge;
					patch[0].acc_year.recharge_wyd += patch[0].recharge
							* round(patch[0].acc_year.length);

					if ((patch[0].snowpack.water_equivalent_depth == 0)
							&& (patch[0].acc_year.snowpack > 0)) {
						if (patch[0].acc_year.meltday
								< patch[0].acc_year.peaksweday)
							patch[0].acc_year.meltday = round(
									patch[0].acc_year.length);
					}

					if (patch[0].snowpack.water_equivalent_depth
							> patch[0].acc_year.snowpack) {
						patch[0].acc_year.peaksweday = round(
								patch[0].acc_year.length);
					}

					patch[0].acc_year.snowpack =
							MAX(patch[0].snowpack.water_equivalent_depth,
									patch[0].acc_year.snowpack);

					/* transpiration water stress computations */
					tmp = (patch[0].transpiration_unsat_zone
							+ patch[0].exfiltration_unsat_zone
							+ patch[0].exfiltration_sat_zone
							+ patch[0].evaporation_surf
							+ +patch[0].transpiration_sat_zone
							+ patch[0].evaporation);
					patch[0].acc_year.et += tmp;

					tmp = (patch[0].transpiration_unsat_zone
							+ patch[0].transpiration_sat_zone);
					patch[0].acc_year.trans += tmp;
					patch[0].acc_year.day7trans = (tmp / 14
							+ 13 / 14 * patch[0].acc_year.day7trans);
					patch[0].acc_year.day7pet = (patch[0].PET + patch[0].PE)
							/ 14 + 13 / 14 * patch[0].acc_year.day7pet;
					if (patch[0].acc_year.day7pet > patch[0].acc_year.maxpet) {
						patch[0].acc_year.maxpet = patch[0].acc_year.day7pet;
						patch[0].acc_year.rec_pet_wyd = 0;
						patch[0].acc_year.max_pet_wyd = patch[0].acc_year.wyd;
					}

					if ((patch[0].acc_year.day7trans
							> patch[0].acc_year.maxtrans)) {
						patch[0].acc_year.maxtrans =
								patch[0].acc_year.day7trans;
						patch[0].acc_year.rec_wyd = 0;
					}

					if ((patch[0].acc_year.rec_wyd == 0)
							&& (patch[0].acc_year.day7trans
									< patch[0].acc_year.maxtrans * 0.5)) {
						patch[0].acc_year.rec_wyd = patch[0].acc_year.wyd;
					}

					if ((patch[0].acc_year.rec_pet_wyd == 0)
							&& (patch[0].acc_year.day7pet
									< patch[0].acc_year.maxpet * 0.5)) {
						patch[0].acc_year.rec_pet_wyd = patch[0].acc_year.wyd;
					}

					tmp = (patch[0].transpiration_unsat_zone
							+ patch[0].exfiltration_unsat_zone
							+ patch[0].exfiltration_sat_zone
							+ patch[0].evaporation_surf
							+ +patch[0].transpiration_sat_zone
							+ patch[0].evaporation);

					if ((patch[0].PET + patch[0].PE - tmp)
							> patch[0].acc_year.sm_deficit)
						patch[0].acc_year.sm_deficit = (patch[0].PET
								+ patch[0].PE - tmp);
					patch[0].acc_year.lai =
							MAX(patch[0].acc_year.lai, patch[0].lai);

					tmp = patch[0].sat_deficit - patch[0].unsat_storage
							- patch[0].rz_storage;
					if (tmp <= 0)
						patch[0].acc_year.ndays_sat += 1;

					if (patch[0].rootzone.S > 0.7)
						patch[0].acc_year.ndays_sat70 += 1;

					tmp =
							MAX(0.0, (patch[0].rootzone.field_capacity/patch[0].rootzone.potential_sat -
											patch[0].wilting_point*patch[0].soil_defaults[0][0].porosity_0))
									/ 2.0
									+ patch[0].wilting_point
											* patch[0].soil_defaults[0][0].porosity_0;

					if ((patch[0].rootzone.S < tmp) && (current_date.month < 10)
							&& (patch[0].acc_year.midsm_wyd == 0)
							&& (patch[0].snowpack.water_equivalent_depth <= 0.0))
						patch[0].acc_year.midsm_wyd = patch[0].acc_year.wyd;

					patch[0].acc_year.wyd = patch[0].acc_year.wyd + 1;
				}

			}
			basin_return_flow += (patch[0].return_flow) * patch[0].area;

		} /* end i */

	} /* end k  */

	basin_outflow /= basin_area;
	preday_basin_rz_storage /= basin_area;
	preday_basin_unsat_storage /= basin_area;
	preday_basin_detention_store /= basin_area;
	preday_basin_sat_deficit /= basin_area;
	basin_rz_storage /= basin_area;
	basin_unsat_storage /= basin_area;
	basin_detention_store /= basin_area;
	basin_sat_deficit /= basin_area;
	water_balance = preday_basin_rz_storage + preday_basin_unsat_storage
			+ preday_basin_detention_store - preday_basin_sat_deficit
			- (basin_rz_storage + basin_unsat_storage + basin_detention_store
					- basin_sat_deficit) - basin_outflow;

	if (abs(water_balance) > ZERO)
		printf("\n Water Balance is %lf for %ld %ld %ld", water_balance,
				current_date.day, current_date.month, current_date.year);
	if ((command_line[0].output_flags.yearly == 1)
			&& (command_line[0].b != NULL )) {
		if (basin_outflow <= command_line[0].thresholds[STREAMFLOW])
			basin[0].acc_year.num_threshold += 1;
	}

	return;

} /*end compute_subsurface_routing.c*/

