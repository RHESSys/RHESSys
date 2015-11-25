/*--------------------------------------------------------------*/
/* 																*/
/*					output_patch						*/
/*																*/
/*	output_patch - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_patch - outputs current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_patch(										*/
/*					struct	patch_object	*patch,				*/
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
#include <iostream>
#include <time.h>
#include <errno.h>

//#include <cassandra.h>
#include <zmq.h>

#include "rhessys.h"
#include "zmqutil.h"

#include "patch.pb.h"

using namespace std;

//void _write_to_patchdb(struct world_output_file_object *world_output_files,
//		char* patchid, char* var, cass_double_t value) {
//	CassError rc = CASS_OK;
//	CassStatement* statement = NULL;
//
//	statement = cass_prepared_bind(world_output_files->var_by_date_patch_stmt);
//	cass_statement_bind_string(statement, 0, var);
//	cass_statement_bind_string(statement, 1, patchid);
//	cass_statement_bind_double(statement, 2, value);
//	cass_batch_add_statement(world_output_files->patchdb_batch, statement);
//	cass_statement_free(statement);
//
//	statement = cass_prepared_bind(world_output_files->patch_by_var_date_stmt);
//	cass_statement_bind_string(statement, 0, patchid);
//	cass_statement_bind_string(statement, 1, var);
//	cass_statement_bind_double(statement, 2, value);
//	cass_batch_add_statement(world_output_files->patchdb_batch, statement);
//	cass_statement_free(statement);
//}

void	output_patch(struct  command_line_object * command_line,
					 struct world_output_file_object *world_output_files,
					 int basinID, int hillID, int zoneID,
					 struct	patch_object	*patch,
					 struct	zone_object	*zone,
					 struct	date	current_date,
					 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int check, c, layer;
	double alai, asub, apsn, litterS;

	if (patch[0].litter.rain_capacity > ZERO)
		litterS = patch[0].litter.rain_stored / patch[0].litter.rain_capacity;
	else
		litterS = 1.0;

	apsn = 0.0;
	asub = 0.0;
	alai = 0.0;
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn ;
			asub += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation;
			alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai;



		}
	}

	if (command_line[0].patchdb_flag) {
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		// Serialize patch data to Protocol Buffers message
		// Wrap in PatchDB mesg
		rhessys::PatchDBMesg m;
		m.set_type(m.OUTPUT_PATCH);

		rhessys::OutputPatch *p = m.mutable_outputpatch();
		p->set_day(current_date.day);
		p->set_month(current_date.month);
		p->set_year(current_date.year);
		p->set_basin_id(basinID);
		p->set_hill_id(hillID);
		p->set_zone_id(zoneID);
		p->set_patch_id(patch[0].ID);
		p->set_rain_throughfall(patch[0].rain_throughfall*1000.0);
		p->set_detention_store(patch[0].detention_store*1000.0);
		p->set_sat_deficit_z(patch[0].sat_deficit_z*1000);
		p->set_sat_deficit(patch[0].sat_deficit*1000);
		p->set_rz_storage(patch[0].rz_storage*1000);
		p->set_potential_sat(patch[0].rootzone.potential_sat*1000);
		p->set_field_capacity(patch[0].rootzone.field_capacity*1000);
		p->set_wilting_point(patch[0].wilting_point*1000);
		p->set_unsat_storage(patch[0].unsat_storage*1000);
		p->set_rz_drainage(patch[0].rz_drainage*1000);
		p->set_unsat_drainage(patch[0].unsat_drainage*1000);
		p->set_sublimation((patch[0].snowpack.sublimation + asub)*1000);
		p->set_return_flow(patch[0].return_flow*1000.0);
		p->set_evaporation(patch[0].evaporation*1000.0);
		p->set_evaporation_surf(patch[0].evaporation_surf*1000.0);
		p->set_soil_evap(patch[0].exfiltration_sat_zone*1000.0 + patch[0].exfiltration_unsat_zone * 1000.0);
		p->set_snow(patch[0].snowpack.water_equivalent_depth*1000.0);
		p->set_snow_melt(patch[0].snow_melt*1000.0);
		p->set_trans_sat(patch[0].transpiration_sat_zone*1000.0);
		p->set_trans_unsat(patch[0].transpiration_unsat_zone*1000.0);
		p->set_q_in(patch[0].Qin_total * 1000.0);
		p->set_q_out(patch[0].Qout_total * 1000.0);
		p->set_psn(apsn * 1000.0);
		p->set_rootzone_s(patch[0].rootzone.S);
		p->set_rootzone_depth(patch[0].rootzone.depth*1000.0);
		p->set_litter_rain_stored(patch[0].litter.rain_stored*1000.0);
		p->set_litter_s(litterS);
		p->set_area(patch[0].area);
		p->set_pet(patch[0].PET*1000.0);
		p->set_lai(alai);
		p->set_baseflow(patch[0].base_flow*1000.0);
		p->set_streamflow(patch[0].streamflow*1000.0);
		p->set_precip(1000.0*(zone[0].rain+zone[0].snow));
		p->set_recharge(patch[0].recharge);

		zmq_msg_t msg;
		int rc = PbToZmq(&m, &msg);
		if (rc == -1) {
			printf("Unable to create zeromq message");
			exit(EXIT_FAILURE);
		}

		rc = zmq_sendmsg(world_output_files->patchdbmq_requester,
				&msg, 0);

		if (rc == -1) {
			if (errno == EFSM) {
				printf("output_patch: zeromq: operation cannot be performed on this socket at the moment due to the socket not being in the appropriate state.\n");
			} else {
				printf("output_patch: zeromq returned, output_patch:173: %d, exiting...\n", errno);
			}
			exit(EXIT_FAILURE);
		}

		//printf("RHESSys.output_patch(): get response...");

		char response[2];
		rc = zmq_recv(world_output_files->patchdbmq_requester, response, ZMQ_NOBLOCK);
		if (rc == -1) {
			if (errno == EFSM) {
				printf("output_patch: zeromq: operation cannot be performed on this socket at the moment due to the socket not being in the appropriate state.\n");
			} else {
				printf("output_patch: zeromq returned, output_patch:185: %d, exiting...\n", errno);
			}
			exit(EXIT_FAILURE);
		}
		//printf("done\n");

		//response[1] = 0;
		if (response[0] != 'A') {
			printf("output_patch: expected patchdbmq server to return %s, but received: %s, exiting...\n",
					"A", response);
			exit(EXIT_FAILURE);
		}

	} else {
		// Output to file
		check = fprintf(outfile,"%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
			current_date.day,
			current_date.month,
			current_date.year,
			basinID,
			hillID,
			zoneID,
			patch[0].ID,
			patch[0].rain_throughfall*1000.0,
			patch[0].detention_store*1000.0,
			patch[0].sat_deficit_z*1000,
			patch[0].sat_deficit*1000,
			patch[0].rz_storage*1000,
			patch[0].rootzone.potential_sat*1000,
			patch[0].rootzone.field_capacity*1000,
			patch[0].wilting_point*1000,
			patch[0].unsat_storage*1000,
			patch[0].rz_drainage*1000,
			patch[0].unsat_drainage*1000,
			(patch[0].snowpack.sublimation + asub)*1000,
			patch[0].return_flow*1000.0,
			patch[0].evaporation*1000.0,
			patch[0].evaporation_surf*1000.0,
			patch[0].exfiltration_sat_zone*1000.0 + patch[0].exfiltration_unsat_zone * 1000.0,
			patch[0].snowpack.water_equivalent_depth*1000.0,
			patch[0].snow_melt*1000.0,
			(patch[0].transpiration_sat_zone*1000.0), (patch[0].transpiration_unsat_zone)*1000.0,
			patch[0].Qin_total * 1000.0,
			patch[0].Qout_total * 1000.0,
			apsn * 1000.0,
			patch[0].rootzone.S,
			patch[0].rootzone.depth*1000.0,
			patch[0].litter.rain_stored*1000.0,
			litterS,
			patch[0].area, (patch[0].PET)*1000.0, alai,
			patch[0].base_flow*1000.0,
			patch[0].streamflow*1000.0, 1000.0*(zone[0].rain+zone[0].snow), patch[0].recharge);
	
		if (check <= 0) {
			fprintf(stdout, "\nWARNING: output error has occured in output_patch");
		}
	}
	return;
} /*end output_patch*/
