/*--------------------------------------------------------------*/
/* 																*/
/*					construct_hillslope_defaults				*/
/*																*/
/*	construct_hillslope_defaults.c - makes hillslope default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_hillslope_defaults.c - makes hillslope default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct hillslope_default *construct_hillslope_defaults(     */
/*								  num_default_files,			*/
/*								  default_files,				*/
/*								command_line_object) 	*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 4/97 - C. Tague										*/
/*	added soil parameters for baseflow routine					*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*								*/
/*	Sep 15 97 - RAF						*/
/*	Removed soil parameters due to new baseflow routine	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct hillslope_default *construct_hillslope_defaults(
							   int	num_default_files,
							   char	**default_files,
		  				           struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int 	i;
	FILE	*default_file;
	char	record[MAXSTR];
	char	*newrecord;
	double	ftmp;
	struct	hillslope_default	*default_object_list;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct hillslope_default *)
		alloc(num_default_files * sizeof(struct hillslope_default),
		"default_object_list", "construct_hillslope_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct hillslope defaults,unable to open defaults file %d.\n",i);
			exit(EXIT_FAILURE);
		} /*end if*/
		
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);

		/*--------------------------------------------------------------*/
		/* 	read in optional paramters				*/
		/*--------------------------------------------------------------*/
		default_object_list[i].gw_loss_coeff = 1.0;
		default_object_list[i].n_routing_timesteps = 24;
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			newrecord = strchr(record,'g');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"gw_loss_coeff") == 0) {	
				default_object_list[i].gw_loss_coeff = ftmp;
				printf("\n Using %lf for %s for hillslope default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
			newrecord = strchr(record,'n');
			if (newrecord != NULL)  {
			if (strcasecmp(newrecord,"n_routing_timesteps") == 0) {	
				default_object_list[i].n_routing_timesteps = (int) ftmp;
				printf("\n Using %lf for %s for hillslope default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			}
		}


		if (default_object_list[i].n_routing_timesteps < 1)
			default_object_list[i].n_routing_timesteps = 1;
		
		if (command_line[0].gw_flag > 0) {
			default_object_list[i].gw_loss_coeff *= command_line[0].gw_loss_coeff_mult;
			}

		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_hillslope_defaults*/
