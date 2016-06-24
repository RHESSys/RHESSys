/*--------------------------------------------------------------*/
/* 																*/
/*					construct_output_fileset					*/
/*																*/
/*	construct_output_fileset - creates output files objects for */
/*					one spatial level and all temporal levels.	*/
/*																*/
/*	NAME														*/
/*	construct_output_fileset - creates output files objects for */
/*					one spatial level and all temporal levels.	*/
/*																*/
/*	SYNOPSIS													*/
/*	struct	output_file_object	*construct_output_fileset(		*/
/*								char	*root)					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Creates four file pointers for yearly, monthly, daily and	*/
/*	hourly output for all simulation objects at a single level	*/
/*	in the spatial hierarchy.									*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We intend that the use of onbe output file per time step	*/
/*	is needed to restrict the output variables for frequent		*/
/*	timesteps.  We also feel that, one output fileset per       */
/*	spatial level reduces the total number of files open at one	*/
/*	time.  It may be useful for optimization to remove this last*/
/*	restriction if a cluster computing envrionment is used.		*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"


struct	output_files_object	*construct_output_fileset(
													  char	*root)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	char	filename[256];
	struct	output_files_object	*fileset;
	/*--------------------------------------------------------------*/
	/*	Construct the basin output files.							*/
	/*--------------------------------------------------------------*/
	fileset = (struct output_files_object *)
		alloc(1 * sizeof(struct output_files_object),
		"fileset","construct_output_fileset");
	strcpy(filename, root);
	strcat(filename,".yearly");
	if ( (fileset[0].yearly = fopen(filename , "w")) == NULL ){
		fprintf(stderr,"FATAL ERROR: in construct_output_fileset.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	strcpy(filename, root);
	strcat(filename,".monthly");
	if ( (fileset[0].monthly = fopen(filename , "w"))== NULL ){
		fprintf(stderr,"FATAL ERROR: in construct_output_fileset.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	strcpy(filename, root);
	strcat(filename,".daily");
	if ( (fileset[0].daily = fopen(filename , "w"))	== NULL ){
		fprintf(stderr,"FATAL ERROR: in construct_output_file.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	strcpy(filename, root);
	strcat(filename,".hourly");
	if ( (fileset[0].hourly = fopen(filename , "w")) == NULL ) {
		fprintf(stderr,"FATAL ERROR: in construct_output_file.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	return(fileset);
} /*end construct_output_fileset*/
