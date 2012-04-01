/*--------------------------------------------------------------*/
/* 																*/
/*					construct_soil_defaults						*/
/*																*/
/*	construct_soil_defaults.c - makes patch default				*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_soil_defaults.c - makes patch default				*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct soil_default *construct_soil_defaults(             */
/*								num_default_files,				*/
/*								  default_files,				*/
/*								  grow_flag,					*/
/*								  default_object_list )			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*	July 28, 1997	C.Tague					*/
/*	removed capillary rise soil variables i.e rooting depth */
/*	pore size index and suction				*/
/*								*/
/*	Sep 15 97 RAF						*/
/*	added cap rise variables back as we now do a cap rise	*/
/*	routine but removed wilting point which was never used.	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct soil_default *construct_soil_defaults(
											   int	num_default_files,
											   char	**default_files,
											   struct command_line_object *command_line) 
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t,
		char	*,
		char	*);
	
	double compute_delta_water(int, double, double,	double, double, double);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	double 		ftmp,soil;
	FILE	*default_file;
	char		*newrecord;
	char	record[MAXSTR];
	struct 	soil_default *default_object_list;
	
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct soil_default *)
		alloc(num_default_files *
		sizeof(struct soil_default),"default_object_list",
		"construct_soil_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,"FATAL ERROR:in construct_soil_defaults",
				"unable to open defaults file %d.\n",i);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);
		fscanf(default_file,"%d",&(default_object_list[i].theta_psi_curve));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].Ksat_0));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].m));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].porosity_0));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].porosity_decay));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].p3));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].pore_size_index));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].psi_air_entry));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].psi_max));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].soil_depth));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].m_z));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].detention_store_size));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].deltaz));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].active_zone_z));
		read_record(default_file, record);

		if (abs(default_object_list[i].active_zone_z - default_object_list[i].soil_depth) > 0.5) {
			printf("\n note that soil depth used for biogeochem cycling (active zone z)");
 			printf("\nis more than 0.5 meter different from hydrologic soil depth");
 			printf("\n for soil default file %s", default_files[i] );
			}

		fscanf(default_file,"%lf",
			&(default_object_list[i].maximum_snow_energy_deficit));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].snow_water_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].snow_light_ext_coef));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].snow_melt_Tcoef));
		read_record(default_file, record);
/*
			 default_object_list[i].snow_melt_Tcoef *= command_line[0].tmp_value; 
*/
		fscanf(default_file,"%lf",&(default_object_list[i].max_heat_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].min_heat_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].albedo));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].mobile_N_proportion));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].N_decay_rate));
		read_record(default_file, record);
		/*
		if (command_line[0].tmp_value > ZERO)
			default_object_list[i].N_decay_rate *= command_line[0].tmp_value;
		*/
		fscanf(default_file,"%lf",&(default_object_list[i].soil_type.sand));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].soil_type.silt));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].soil_type.clay));
		read_record(default_file, record);
		soil =  default_object_list[i].soil_type.sand
			+ default_object_list[i].soil_type.silt
			+ default_object_list[i].soil_type.clay;
		if  (abs(soil - 1.0) > ZERO) {
			fprintf(stderr,
				"FATAL ERROR:in construct_soil_defaults\n proportion sand, silt, clay = %f\n\n", soil);
			printf("\n %d -  %f %f %f %f \n",
				default_object_list[i].ID,
				default_object_list[i].N_decay_rate,
				default_object_list[i].soil_type.sand,
				default_object_list[i].soil_type.silt,
				default_object_list[i].soil_type.clay);
		} /*end if*/

		default_object_list[i].sat_to_gw_coeff = 0.0;

		if (command_line[0].gw_flag > 0) {
			fscanf(default_file,"%lf",&(default_object_list[i].sat_to_gw_coeff));
			read_record(default_file, record);
			default_object_list[i].sat_to_gw_coeff *= command_line[0].sat_to_gw_coeff_mult;
			}

		/*--------------------------------------------------------------*/
		/*	vertical soil m and K are initized using soil default	*/
		/*	but sensitivity analysis -s is not applied to them	*/
		/*      use -sv to change these parameters			*/
		/*--------------------------------------------------------------*/
			default_object_list[i].m_v = default_object_list[i].m;
			default_object_list[i].mz_v = default_object_list[i].m_z;
			default_object_list[i].Ksat_0_v = default_object_list[i].Ksat_0;


		/*--------------------------------------------------------------*/
		/* sensitivity adjustment of vertical drainage  soil paramters	*/
		/*--------------------------------------------------------------*/
		if (command_line[0].vsen_flag > 0) {
				default_object_list[i].m_v *= command_line[0].vsen[M];
				default_object_list[i].mz_v *= command_line[0].vsen[M];
				default_object_list[i].Ksat_0_v *= command_line[0].vsen[K];
		}

		/*--------------------------------------------------------------*/
		/* sensitivity adjustment of soil drainage paramters		*/
		/*--------------------------------------------------------------*/
		if (command_line[0].sen_flag > 0) {
				default_object_list[i].m *= command_line[0].sen[M];
				default_object_list[i].m_z *= command_line[0].sen[M];
				default_object_list[i].Ksat_0 *= command_line[0].sen[K];
				default_object_list[i].soil_depth *= command_line[0].sen[SOIL_DEPTH];
		}

		/*--------------------------------------------------------------*/
		/*      calculate water_equivalent depth of soil                */
		/*--------------------------------------------------------------*/
		default_object_list[i].soil_water_cap = compute_delta_water(
			0, default_object_list[i].porosity_0,
			default_object_list[i].porosity_decay,
			default_object_list[i].soil_depth,
			default_object_list[i].soil_depth,
			0.0);

		/*--------------------------------------------------------------*/
		/* initialization of optional default file parms		*/
		/*--------------------------------------------------------------*/
			default_object_list[i].theta_mean_std_p1 = 0.0;
			default_object_list[i].theta_mean_std_p2 = 0.0;
			default_object_list[i].gl_c = 0.0062;
			default_object_list[i].gsurf_slope = 0.01;
			default_object_list[i].gsurf_intercept = 0.001;
			default_object_list[i].p4 = -1.5;
			default_object_list[i].DOM_decay_rate = 0.05;
			default_object_list[i].mobile_DON_proportion = 1.0;
			default_object_list[i].mobile_DOC_proportion = 1.0;
			default_object_list[i].DON_production_rate = 0.03;


		/*--------------------------------------------------------------*/
		/* non-critical parameter changes				*/
		/*--------------------------------------------------------------*/
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			newrecord = strchr(record,'g');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"gl_c") == 0) {	
				default_object_list[i].gl_c = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"gsurf_slope") == 0) {	
				default_object_list[i].gsurf_slope = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"gsurf_intercept") == 0) {	
				default_object_list[i].gsurf_intercept = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'t');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"theta_mean_std_p1") == 0) {	
				default_object_list[i].theta_mean_std_p1 = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"theta_mean_std_p2") == 0) {	
				default_object_list[i].theta_mean_std_p2 = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'D');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"DON_production_rate") == 0) {	
				default_object_list[i].DOM_decay_rate = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'D');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"DOM_decay_rate") == 0) {	
				default_object_list[i].DOM_decay_rate = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'m');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"mobile_DOC_proportion") == 0) {	
				default_object_list[i].mobile_DON_proportion = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"mobile_DON_proportion") == 0) {	
				default_object_list[i].mobile_DON_proportion = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'p');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"p4") == 0) {	
				default_object_list[i].p4 = ftmp;
				printf("\n Using %lf for %s for soil default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
		}

		/*--------------------------------------------------------------*/
		/* sensitivity adjustment of vertical drainage  soil paramters	*/
		/* an  scale Pore size index and psi air entry or other parameters	*/
		/* that control moisture retention (if curve 3 is used)		*/
		/*--------------------------------------------------------------*/
		if (command_line[0].vsen_alt_flag > 0) {
			if (default_object_list[i].theta_psi_curve != 3)  {
				default_object_list[i].psi_air_entry *= command_line[0].vsen_alt[PA];
				default_object_list[i].pore_size_index *= command_line[0].vsen_alt[PO];
				if (default_object_list[0].pore_size_index >= 1.0) {
					printf("\n Sensitivity analysis giving Pore Size Index > 1.0, not allowed, setting to 1.0\n");
					default_object_list[i].pore_size_index = 0.999;
					}
			}
			else {
				default_object_list[i].p3 *= command_line[0].vsen_alt[PA];
				default_object_list[i].p4 *= command_line[0].vsen_alt[PO];
			}

		}



	
		
		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_soil_defaults*/
