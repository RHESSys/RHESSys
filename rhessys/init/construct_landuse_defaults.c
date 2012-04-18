/*--------------------------------------------------------------*/
/* 																*/
/*					construct_landuse_defaults						*/
/*																*/
/*	construct_landuse_defaults.c - makes patch default				*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_landuse_defaults.c - makes patch default				*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct patch_default *construct_landuse_defaults(             */
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
/*	removed capillary rise landuse variables i.e rooting depth */
/*	pore size index and suction				*/
/*								*/
/*	Sep 15 97 RAF						*/
/*	added cap rise variables back as we now do a cap rise	*/
/*	routine but removed wilting point which was never used.	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct landuse_default *construct_landuse_defaults(
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
	
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	double 		landuse, ftmp;
	FILE	*default_file;
	char	record[MAXSTR];
	char	*newrecord;
	struct 	landuse_default *default_object_list;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct landuse_default *)
		alloc(num_default_files *
		sizeof(struct landuse_default),"default_object_list",
		"construct_landuse_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,"FATAL ERROR:in construct_landuse_defaults",
				"unable to open defaults file %d.\n",i);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/


			fscanf(default_file,"%d",&(default_object_list[i].ID));
			read_record(default_file, record);
			fscanf(default_file,"%lf",&(default_object_list[i].irrigation));
			read_record(default_file, record);
			default_object_list[i].irrigation /= 365;
			fscanf(default_file,"%lf",&(default_object_list[i].fertilizer_NO3));
			read_record(default_file, record);
			default_object_list[i].fertilizer_NO3 /= 365;
			fscanf(default_file,"%lf",&(default_object_list[i].fertilizer_NH4));
			read_record(default_file, record);
			default_object_list[i].fertilizer_NH4 /= 365;
			fscanf(default_file,"%lf",&(default_object_list[i].septic_NO3_load));
			read_record(default_file, record);
			default_object_list[i].septic_NO3_load /= 365;
			fscanf(default_file,"%lf",&(default_object_list[i].septic_water_load));
			read_record(default_file, record);
			default_object_list[i].septic_water_load /= 365;
			fscanf(default_file,"%lf",&(default_object_list[i].detention_store_size));
			read_record(default_file, record);
			default_object_list[i].detention_store_size /= 365;

			default_object_list[i].percent_impervious = 0.0;
			default_object_list[i].PH = 7.0;
			while (!feof(default_file)) {
				fscanf(default_file,"%lf", &(ftmp));
				read_record(default_file, record);
				newrecord = strchr(record,'l');
				if (newrecord != NULL) {
				if (strcasecmp(newrecord,"PH") == 0) {	
					default_object_list[i].PH = ftmp;
					printf("\n Using %lf for %s for landuse default ID %d",
						ftmp, newrecord, default_object_list[i].ID);
					}
				}
				if (newrecord != NULL) {
				if (strcasecmp(newrecord,"landuse.percent_impervious") == 0) {	
					default_object_list[i].percent_impervious = ftmp;
					printf("\n Using %lf for %s for landuse default ID %d",
						ftmp, newrecord, default_object_list[i].ID);
					}
				}
			}

		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_landuse_defaults*/
