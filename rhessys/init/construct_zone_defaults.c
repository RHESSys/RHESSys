/*--------------------------------------------------------------*/
/* 																*/
/*					construct_zone_defaults						*/
/*																*/
/*	construct_zone_defaults.c - makes zone default				*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_zone_defaults.c - makes zone default				*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct zone_default *construct_zone_defaults(               */
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
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

struct zone_default *construct_zone_defaults(
											 int	num_default_files,
											 char	**default_files,
											 int 	grow_flag)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	FILE	*default_file;
	char	record[MAXSTR];
	struct	zone_default	*default_object_list ;
	char	*newrecord;
	double	ftmp;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct zone_default *)
		alloc(num_default_files * sizeof(struct zone_default),
		"default_object_list","construct_zone_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct_zone_defaults unable to open defaults file %d.\n",i);
			exit(0);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].atm_trans_lapse_rate));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].dewpoint_lapse_rate));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].max_effective_lai));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].lapse_rate));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].pptmin));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].sea_level_clear_sky_trans));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].temcf));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].trans_coeff1));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].trans_coeff2));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].wind));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].max_snow_temp));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].min_rain_temp));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].ndep_NO3));
		read_record(default_file, record);
		/*--------------------------------------------------------------*/
		/*	convert from annual to daily				*/
		/*	(old)							*/
		/*	currently a concentration				*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ndep_NO3 /= 365.0; 
		default_object_list[i].lapse_rate_tmin = default_object_list[i].lapse_rate; 
		default_object_list[i].lapse_rate_tmax = default_object_list[i].lapse_rate; 
		default_object_list[i].wet_lapse_rate = 0.0049; 
		default_object_list[i].lapse_rate_precip_default = -999.0; 


		/*--------------------------------------------------------------*/
		/* read any optional (tagged) defaults here			*/
		/*--------------------------------------------------------------*/
		/*	CO2 is initialized/default to 322 ppm			*/
		/*--------------------------------------------------------------*/
		default_object_list[i].atm_CO2 = 322.0; 
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			newrecord = strchr(record,'l');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"lapse_rate_tmax") == 0) {	
				default_object_list[i].lapse_rate_tmax = ftmp;
				printf("\n Using %lf for %s for zone default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'l');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"lapse_rate_tmin") == 0) {	
				default_object_list[i].lapse_rate_tmin = ftmp;
				printf("\n Using %lf for %s for zone default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'l');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"lapse_rate_precip_default") == 0) {	
				default_object_list[i].lapse_rate_precip_default = ftmp;
				printf("\n Using %lf for %s for zone default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'w');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"wet_lapse_rate") == 0) {	
				default_object_list[i].wet_lapse_rate = ftmp;
				printf("\n Using %lf for %s for zone default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'a');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"atm_CO2") == 0) {	
				default_object_list[i].atm_CO2 = ftmp;
				printf("\n Using %lf for %s for zone default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
		}
		/*--------------------------------------------------------------*/
		/*      if grow flag allocate a grow default structure for the  */
		/*      ith object.                                             */
		/*--------------------------------------------------------------*/
		if ( grow_flag ){
			default_object_list[i].grow_defaults = (struct
				zone_grow_default *)
				alloc(1 * sizeof(struct zone_grow_default),
				"grow_defaults","construct_zone_defaults");
			/*--------------------------------------------------------------*/
			/*          NOTE: PLACE ANY GROW READING HERE.                  */
			/*--------------------------------------------------------------*/
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_zone_defaults*/
