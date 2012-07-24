/*--------------------------------------------------------------*/
/* 																*/
/*					construct_surface_energy_defaults				*/
/*																*/
/*	construct_surface_energy_defaults.c - makes surface_energy default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_surface_energy_defaults.c - makes surface_energy default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct surface_energy_default *construct_surface_energy_defaults(     */
/*								  num_default_files,			*/
/*								  default_files,				*/
/*								  grow_flag,					*/
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
#include <stdlib.h>
#include "rhessys.h"
struct surface_energy_default *construct_surface_energy_defaults(
											   int	num_default_files,
											   char	**default_files,
											   int grow_flag)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc( size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int 	i;
	double  ftmp, soil;
	FILE	*default_file;
	char	*newrecord;
	char	record[MAXSTR];
	struct	surface_energy_default	*default_object_list;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct surface_energy_default *)
		alloc(num_default_files *
		sizeof(struct surface_energy_default),"default_object_list",
		"construct_surface_energy_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct surface_energye defaults,unable to open defaults file %s.\n",default_files[i]);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);
		/*--------------------------------------------------------------*/
		/*		assign parameters in  default */
		/*--------------------------------------------------------------*/
		default_object_list[i].N_thermal_nodes = 10;
		default_object_list[i].exp_dist = 0;
		default_object_list[i].damping_depth = 4;
		default_object_list[i].iteration_threshold = 0.01;

		/*--------------------------------------------------------------*/
		/*		read the optional parameter specification in  default */
		/*--------------------------------------------------------------*/
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			newrecord = strchr(record,'N');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"N_thermal_nodes") == 0) {	
				default_object_list[i].N_thermal_nodes = ftmp;
				printf("\n Using %lf for %s for surface_energy default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'e');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"exp_dist") == 0) {	
				default_object_list[i].exp_dist = ftmp;
				printf("\n Using %lf for %s for surface_energy default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'d');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"damping_depth") == 0) {	
				default_object_list[i].damping_depth = ftmp;
				printf("\n Using %lf for %s for surface_energy default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'i');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"iteration_threshold") == 0) {	
				default_object_list[i].iteration_threshold = ftmp;
				printf("\n Using %lf for %s for surface_energy default ID %d",
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
} /*end construct_surface_energy_defaults*/
