/*--------------------------------------------------------------*/
/* 																*/
/*					construct_fire_defaults				*/
/*																*/
/*	construct_fire_defaults.c - makes fire default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_fire_defaults.c - makes fire default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct fire_default *construct_fire_defaults(     */
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
struct fire_default *construct_fire_defaults(
											   int	num_default_files,
											   char	**default_files,
											   struct command_line_object *command_line)
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
	struct	fire_default	*default_object_list;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct fire_default *)
		alloc(num_default_files *
		sizeof(struct fire_default),"default_object_list",
		"construct_fire_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct firee defaults,unable to open defaults file %s.\n",default_files[i]);
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
		default_object_list[i].veg_fuel_weighting = 0.2;
		/*--------------------------------------------------------------*/
		/*		read the optional parameter specification in  default */
		/*--------------------------------------------------------------*/
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			newrecord = strchr(record,'v');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"veg_fuel_weighting") == 0) {	
				default_object_list[i].veg_fuel_weighting = ftmp;
				printf("\n Using %lf for %s for fire default ID %d",
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
} /*end construct_fire_defaults*/
