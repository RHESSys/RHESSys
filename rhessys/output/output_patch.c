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
#include <time.h>

#include <cassandra.h>

#include "rhessys.h"
#include "functions.h"


void _write_to_patchdb(struct world_output_file_object *world_output_files,
		char* patchid, char* var, cass_double_t value) {
	CassError rc = CASS_OK;
	CassStatement* statement = NULL;

	statement = cass_prepared_bind(world_output_files->var_by_date_patch_stmt);
	cass_statement_bind_string(statement, 0, var);
	cass_statement_bind_string(statement, 1, patchid);
	cass_statement_bind_double(statement, 2, value);
	cass_batch_add_statement(world_output_files->patchdb_batch, statement);
	cass_statement_free(statement);

	statement = cass_prepared_bind(world_output_files->patch_by_var_date_stmt);
	cass_statement_bind_string(statement, 0, patchid);
	cass_statement_bind_string(statement, 1, var);
	cass_statement_bind_double(statement, 2, value);
	cass_batch_add_statement(world_output_files->patchdb_batch, statement);
	cass_statement_free(statement);
}

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
		char patchid[64];
		snprintf(patchid, 64, "%d:%d:%d:%d", basinID, hillID,
				 zoneID, patch->ID);

		_write_to_patchdb(world_output_files, patchid,
				"rain_thr", (cass_double_t)patch[0].rain_throughfall*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"detention_store", (cass_double_t)patch[0].detention_store*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"sat_def_z", (cass_double_t)patch[0].sat_deficit_z*1000);
		_write_to_patchdb(world_output_files, patchid,
				"sat_def", (cass_double_t)patch[0].sat_deficit*1000);
		_write_to_patchdb(world_output_files, patchid,
				"rz_storage", (cass_double_t)patch[0].rz_storage*1000);
		_write_to_patchdb(world_output_files, patchid,
				"potential_rz_store", (cass_double_t)patch[0].rootzone.potential_sat*1000);
		_write_to_patchdb(world_output_files, patchid,
				"rz_field_capacity", (cass_double_t)patch[0].rootzone.field_capacity*1000);
		_write_to_patchdb(world_output_files, patchid,
				"rz_wilting_point", (cass_double_t)patch[0].wilting_point*1000);
		_write_to_patchdb(world_output_files, patchid,
				"unsat_stor", (cass_double_t)patch[0].unsat_storage*1000);
		_write_to_patchdb(world_output_files, patchid,
				"rz_drainage", (cass_double_t)patch[0].rz_drainage*1000);
		_write_to_patchdb(world_output_files, patchid,
				"unsat_drain", (cass_double_t)patch[0].unsat_drainage*1000);
		_write_to_patchdb(world_output_files, patchid,
				"sublimation", (cass_double_t)(patch[0].snowpack.sublimation + asub)*1000);
		_write_to_patchdb(world_output_files, patchid,
				"return", (cass_double_t)patch[0].return_flow*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"evap", (cass_double_t)patch[0].evaporation*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"evap_surface", (cass_double_t)patch[0].evaporation_surf*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"soil_evap", (cass_double_t)patch[0].exfiltration_sat_zone*1000.0 + patch[0].exfiltration_unsat_zone * 1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"snow", (cass_double_t)patch[0].snowpack.water_equivalent_depth*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"snow_melt", (cass_double_t)patch[0].snow_melt*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"trans_sat", (cass_double_t)patch[0].transpiration_sat_zone*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"trans_unsat", (cass_double_t)patch[0].transpiration_unsat_zone*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"Qin", (cass_double_t)patch[0].Qin_total * 1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"Qout", (cass_double_t)patch[0].Qout_total * 1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"psn", (cass_double_t)apsn * 1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"root_zone.S", (cass_double_t)patch[0].rootzone.S);
		_write_to_patchdb(world_output_files, patchid,
				"root.depth", (cass_double_t)patch[0].rootzone.depth*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"litter.rain_stor", (cass_double_t)patch[0].litter.rain_stored*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"litter.S", (cass_double_t)litterS);
		_write_to_patchdb(world_output_files, patchid,
				"area", (cass_double_t)patch[0].area);
		_write_to_patchdb(world_output_files, patchid,
				"pet", (cass_double_t)patch[0].PET*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"lai", (cass_double_t)alai);
		_write_to_patchdb(world_output_files, patchid,
				"baseflow", (cass_double_t)patch[0].base_flow*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"streamflow", (cass_double_t)patch[0].streamflow*1000.0);
		_write_to_patchdb(world_output_files, patchid,
				"pcp", (cass_double_t)1000.0*(zone[0].rain+zone[0].snow));
		_write_to_patchdb(world_output_files, patchid,
				"recharge", (cass_double_t)patch[0].recharge);
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
