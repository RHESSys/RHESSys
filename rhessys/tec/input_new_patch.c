/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_patch								*/
/*																*/
/*	input_new_patch.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	input_new_patch.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void  input_new_patch( 						*/
/*					FILE	*world_file,						*/
/*					struct	command_line_object	*command_line,	*/
/*					struct	default_object)						*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory and reads in parameters from the hillslope	*/
/*	file to create a patch object.  Invokes construction		*/
/*	of canopy_stratum objects.									*/
/*																*/
/*	Refer to cnostruct_basin.c for a specification of the		*/
/*	hillslopes file.											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We assume that the FILE pointers to the 					*/
/*	hillslope file are positioned properly.						*/
/*	 															*/
/*	We assume that the basin and hillslope files have correct	*/
/*	syntax.														*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "phys_constants.h"
#define ONE 1.0
#include "params.h"

 void  input_new_patch(
									 struct	command_line_object	*command_line,
									 FILE	*world_file,
									 int     num_world_base_stations,
									 struct  base_station_object **world_base_stations,
									 struct	default_object	*defaults,
									 struct  basin_object *basin,
									 struct	 patch_object *patch)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	double	compute_z_final( 	int,
		double,
		double,
		double,
		double,
		double);
	void	update_litter_interception_capacity (double,
		double,
		struct litter_c_object *,
		struct litter_object *);

	void	*alloc(	size_t, char *, char *);
	param	*readtag_worldfile(int *,
				  FILE *,
				  char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i, dtmp;
	char		record[MAXSTR];
	double	        ltmp;
	int		paramCnt=0;
	param		*paramPtr=NULL;

	/*--------------------------------------------------------------*/
	/*	Read in the next patch record for this hillslope.			*/
	/*--------------------------------------------------------------*/
	paramPtr = readtag_worldfile(&paramCnt,world_file,"Patch");

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",patch[0].x,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].x = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",patch[0].y,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].y = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",patch[0].z,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].z = ltmp;

	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"soil_parm_ID","%d",patch[0].soil_parm_ID,1);	
	if (abs(dtmp - NULLVAL) >= ONE)  patch[0].soil_parm_ID = dtmp;
	dtmp  = getIntWorldfile(&paramCnt,&paramPtr,"landuse_parm_ID","%d",patch[0].landuse_parm_ID,1);
	if (abs(dtmp - NULLVAL) >= ONE)  patch[0].landuse_parm_ID = dtmp;
	if (command_line[0].beetlespread_flag == 1)
	{
		dtmp = getIntWorldfile(&paramCnt, &paramPtr, "beetle_parm_ID", "%d", 1, 1);
		if (fabs(ltmp - NULLVAL) >= ONE)
			patch[0].beetle_parm_ID = dtmp;
	}

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"area","%lf",patch[0].area,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].area = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"slope","%lf",patch[0].slope,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].slope = ltmp * DtoR;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"lna","%lf",patch[0].lna,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].lna = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"Ksat_vertical","%lf",patch[0].Ksat_vertical,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].Ksat_vertical = ltmp;
	ltmp= getDoubleWorldfile(&paramCnt,&paramPtr,"mpar","%lf",patch[0].mpar,1);
      	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].mpar = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"std","%lf",patch[0].std,1);
	if (command_line[0].stdev_flag == 1) {
		if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].std = ltmp;
	}

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"rz_storage","%lf",patch[0].rz_storage,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].rz_storage = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"unsat_storage","%lf",patch[0].unsat_storage,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].unsat_storage = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"sat_deficit","%lf",patch[0].sat_deficit,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].sat_deficit = ltmp;

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snowpack.water_equivalent_depth","%lf",
						    patch[0].snowpack.water_equivalent_depth,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		 if ((patch[0].snowpack.water_equivalent_depth < ZERO) && (ltmp > ZERO)) {
			patch[0].snowpack.energy_deficit = basin[0].snowpack.energy_deficit;
			patch[0].snowpack.T = basin[0].snowpack.T;
			patch[0].snowpack.surface_age = basin[0].snowpack.surface_age;
			}

		patch[0].precip_with_assim += (ltmp - patch[0].snowpack.water_equivalent_depth);
		patch[0].snowpack.water_equivalent_depth = ltmp;
	}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snowpack.water_depth","%lf",patch[0].snowpack.water_depth,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.water_depth = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snowpack.T","%lf",patch[0].snowpack.T,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.T = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snowpack.surface_age","%lf",patch[0].snowpack.surface_age,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.surface_age = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snowpack.energy_deficit","%lf",patch[0].snowpack.energy_deficit,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.energy_deficit = ltmp;

	if (command_line[0].snow_scale_flag == 1) {
		ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"snow_redist_scale","%lf",patch[0].snow_redist_scale,1);
		if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snow_redist_scale = ltmp;
		}



	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter.cover_fraction","%lf",patch[0].litter.cover_fraction,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.cover_fraction = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter.rain_stored","%lf",patch[0].litter.rain_stored,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.rain_stored = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter_cs.litr1c","%lf",patch[0].litter_cs.litr1c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_cs.litr1c = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter_ns.litr1n","%lf",patch[0].litter_ns.litr1n,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_ns.litr1n = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter_cs.litr2c","%lf",patch[0].litter_cs.litr2c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr2c = ltmp;
		patch[0].litter_ns.litr2n = patch[0].litter_cs.litr2c / CEL_CN;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter_cs.litr3c","%lf",patch[0].litter_cs.litr3c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr3c = ltmp;
		patch[0].litter_ns.litr3n = patch[0].litter_cs.litr3c / CEL_CN;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"litter_cs.litr4c","%lf",patch[0].litter_cs.litr4c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr4c = ltmp;
		patch[0].litter_ns.litr4n = patch[0].litter_cs.litr4c / LIG_CN;
		}

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_cs.soil1c","%lf",patch[0].soil_cs.soil1c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil1c = ltmp;
		patch[0].soil_ns.soil1n = patch[0].soil_cs.soil1c / SOIL1_CN;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_ns.sminn","%lf",patch[0].soil_ns.sminn,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.sminn = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_ns.nitrate","%lf",patch[0].soil_ns.nitrate,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.nitrate = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_cs.soil2c","%lf",patch[0].soil_cs.soil2c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil2c = ltmp;
		patch[0].soil_ns.soil2n = patch[0].soil_cs.soil2c / SOIL2_CN;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_cs.soil3c","%lf",patch[0].soil_cs.soil3c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil3c = ltmp;
		patch[0].soil_ns.soil3n = patch[0].soil_cs.soil3c / SOIL3_CN;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"soil_cs.soil4c","%lf",patch[0].soil_cs.soil4c,1);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil4c = ltmp;
		patch[0].soil_ns.soil4n = patch[0].soil_cs.soil4c / SOIL4_CN;
		}

	/*--------------------------------------------------------------*/
	/*	initialize litter capacity				*/
	/*--------------------------------------------------------------*/
	update_litter_interception_capacity(
		patch[0].litter.moist_coef,
		patch[0].litter.density,
		&(patch[0].litter_cs),
		&(patch[0].litter));

	/*--------------------------------------------------------------*/
	/*	initialize sinks					*/
	/*--------------------------------------------------------------*/

	patch[0].litter_cs.litr1c_hr_snk = 0.0;
	patch[0].litter_cs.litr2c_hr_snk = 0.0;
	patch[0].litter_cs.litr4c_hr_snk = 0.0;

	patch[0].soil_cs.soil1c_hr_snk = 0.0;
	patch[0].soil_cs.soil2c_hr_snk = 0.0;
	patch[0].soil_cs.soil4c_hr_snk = 0.0;

	patch[0].soil_ns.nfix_src = 0.0;
	patch[0].soil_ns.ndep_src = 0.0;
	patch[0].soil_ns.nleached_snk = 0.0;
	patch[0].soil_ns.nvolatilized_snk = 0.0;


	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this patch								*/
	/*--------------------------------------------------------------*/
	if (patch[0].soil_parm_ID > 0) {
		i = 0;
		while (defaults[0].soil[i].ID != patch[0].soil_parm_ID) {
			i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_soil_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in input_new_patch, soil default ID %d not found for patch %d\n" ,
				patch[0].soil_parm_ID, patch[0].ID);
			exit(EXIT_FAILURE);
			}
		} /* end-while */

	patch[0].soil_defaults[0] = &defaults[0].soil[i];
	}

	if (patch[0].landuse_parm_ID > 0) {
		i = 0;
		while (defaults[0].landuse[i].ID != patch[0].landuse_parm_ID) {
			i++;
			/*--------------------------------------------------------------*/
			/*  Report an error if no match was found.  Otherwise assign    */
			/*  the default to point to this patch.						    */
			/*--------------------------------------------------------------*/
			if ( i>= defaults[0].num_landuse_default_files ){
				fprintf(stderr,
					"\nFATAL ERROR: in input_new_patch, landuse default ID %d not found for patch %d\n" ,
					patch[0].landuse_parm_ID, patch[0].ID);
				exit(EXIT_FAILURE);
			}
		} /* end-while */
		patch[0].landuse_defaults[0] = &defaults[0].landuse[i];
	}
	/*--------------------------------------------------------------*/
	/* FOR now substitute worldfile m (if > 0) in defaults			*/
	/*--------------------------------------------------------------*/
	if (patch[0].mpar > ZERO) {
		patch[0].original_m = patch[0].mpar;
		patch[0].soil_defaults[0][0].m = patch[0].mpar * command_line[0].sen[M];
		patch[0].soil_defaults[0][0].m_z = patch[0].mpar * command_line[0].sen[M] /
				patch[0].soil_defaults[0][0].porosity_0;
	}


	/*--------------------------------------------------------------*/
	/* compute a biological soil depth based on the minimum of soil depth */
	/* and m, K parameters defining conductivity < 0.1% original value */
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults[0][0].effective_soil_depth = patch[0].soil_defaults[0][0].soil_depth;

	/* patch[0].soil_defaults[0][0].effective_soil_depth = min(patch[0].soil_defaults[0][0].soil_depth,
				6.9*patch[0].soil_defaults[0][0].m_z);
	*/
	/*--------------------------------------------------------------*/
	/* detention store size can vary with both soil and landuse		*/
	/*	use the maximum of the two									*/
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults[0][0].detention_store_size =
				max(patch[0].landuse_defaults[0][0].detention_store_size,
				patch[0].soil_defaults[0][0].detention_store_size);
	/*--------------------------------------------------------------*/
	/*	Read in the number of  patch base stations 					*/
	/*--------------------------------------------------------------*/
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"patch_n_basestations","%d",patch[0].num_base_stations,0);	
	if (dtmp > 0)  {
		patch[0].num_base_stations = dtmp;
		/*--------------------------------------------------------------*/
		/*    Allocate a list of base stations for this patch.			*/
		/*--------------------------------------------------------------*/
		patch[0].base_stations = (struct base_station_object **)
			alloc(patch[0].num_base_stations *
			sizeof(struct base_station_object *),
			"base_stations","input_new_patch" );
		/*--------------------------------------------------------------*/
		/*      Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<patch[0].num_base_stations; i++){
			fscanf(world_file,"%d",&(base_stationID));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*	Point to the appropriate base station in the base       	*/
			/*              station list for this world.					*/
			/*																*/
			/*--------------------------------------------------------------*/
			patch[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);
		} /*end for*/
	}

	/*--------------------------------------------------------------*/
	/*	compute actual depth to water tablke			*/
	/*--------------------------------------------------------------*/
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0,
		-1*patch[0].sat_deficit);

	if(paramPtr!=NULL){
	  free(paramPtr);
	}
	return;
} /*end input_new_patch.c*/

