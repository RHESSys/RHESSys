/*--------------------------------------------------------------*/
/* 																*/
/*					construct_dated_input	 					*/
/*																*/
/*	construct_dated_input.c - makes a hourly clim object		*/
/*																*/
/*	NAME														*/
/*	construct_dated_input.c - makes a hourly clim object		*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates a climate object.		  							*/
/*	Computes the number of entries based on start and end dates.*/
/*	Allocates an array of critical clim data for n entries.		*/
/*																*/
/*  A clim object is simply a list of doubleing point values     */
/*  however, by checking the index in the file containgin the   */
/*  list, the code insures that the list matches the number of  */
/*  hours in the simulation and extends from start to end hour  */
/*	where hour 1 is the first hour of the simulation and 		*/
/*	the last hour is computed by multiplying by the number of	*/
/*	julian days in the simulation.								*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*  A clim sequence file should be named with a prefix          */
/*  matching that in its base station file.  It will be         */
/*  ignored if its suffix is not in the list of parameters      */
/*  required at that time step.                                 */
/*                                                              */
/*  The clim_sequence is constructed by calling construct_      */
/*  clim_sequence.  Refer to that code for documentation of     */
/*  what a valid clim sequence is.                              */
/*                                                              */
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

struct	dated_input_object *construct_dated_input(
						  FILE	*base_station_file,
						  char	*file_prefix,
						  struct	date	start_date)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	struct clim_event_sequence construct_dated_clim_sequence(char *,
		struct date);
	void	*alloc(size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	i;
	int	num_non_critical_sequences;
	char  record[MAXSTR];
	char	sequence_name[256];
	char	file_name[256];
	struct dated_input_object	*dated_input;
	
	/*--------------------------------------------------------------*/
	/*	Allocate the hourly clim object.								*/
	/*--------------------------------------------------------------*/
	dated_input = (struct dated_input_object *)
		alloc(1*sizeof(struct dated_input_object),
		"dated_input","construct_dated_input" );
	
	/*--------------------------------------------------------------*/
	/*	Attempt to open the hourly clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Initialize non - critical sequences.			*/
	/*--------------------------------------------------------------*/
	dated_input[0].fertilizer_NO3.inx = -999;
	dated_input[0].fertilizer_NH4.inx = -999;
	dated_input[0].irrigation.inx = -999;
	dated_input[0].snow_melt_input.inx = -999;
	dated_input[0].biomass_removal_percent.inx = -999;
	dated_input[0].pspread.inx = -999;
	dated_input[0].PH.inx = -999;
	dated_input[0].grazing_Closs.inx = -999;
	
	/*--------------------------------------------------------------*/
	/*	Read the still open base station file for the number of		*/
	/*	non-critical parameters.									*/
	/*--------------------------------------------------------------*/
	fscanf(base_station_file,"%d", &num_non_critical_sequences);
	read_record(base_station_file, record);

	/*--------------------------------------------------------------*/
	/*	Loop through all of the non-critical sequences and attempt	*/
	/*	to construct them.											*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<num_non_critical_sequences ; i++ ){
		/*--------------------------------------------------------------*/
		/*		read in the non-critical_sequence name.					*/
		/*--------------------------------------------------------------*/
		fscanf(base_station_file,"%s",sequence_name);
		read_record(base_station_file, record);
		/*--------------------------------------------------------------*/
		/*		test the sequence name and create it if it is valid.	*/
		/*		otherwise report a warning.								*/
		/*--------------------------------------------------------------*/
		if ( strcmp(sequence_name,"fertilizer_NO3" ) == 0) {
			strcpy(file_name, file_prefix);
			dated_input[0].fertilizer_NO3 = construct_dated_clim_sequence(
				(char *)strcat(file_name,".fertilizer_NO3"),
				start_date);
		}
		else if ( strcmp(sequence_name,"fertilizer_NH4" ) == 0){
			strcpy(file_name, file_prefix);
			dated_input[0].fertilizer_NH4 = construct_dated_clim_sequence(
				(char *)strcat(file_name,".fertilizer_NH4"),
				start_date);
		}
		else if ( strcmp(sequence_name,"irrigation" ) == 0){
			strcpy(file_name, file_prefix);
			dated_input[0].irrigation = construct_dated_clim_sequence(
				(char *)strcat(file_name,".irrigation"),
				start_date);
		}
		else if ( strcmp(sequence_name,"snow_melt_input" ) == 0){
			strcpy(file_name, file_prefix);
			dated_input[0].snow_melt_input = construct_dated_clim_sequence(
				(char *)strcat(file_name,".snow_melt_input"),
				start_date);
		}
		else if ( strcmp(sequence_name,"biomass_removal_percent" ) == 0){
			strcpy(file_name, file_prefix);
			dated_input[0].biomass_removal_percent = construct_dated_clim_sequence(
				(char *)strcat(file_name,".biomass_removal_percent"),
				start_date);
		}
		else if ( strcmp(sequence_name,"pspread" ) == 0){
			strcpy(file_name, file_prefix);
			dated_input[0].pspread = construct_dated_clim_sequence(
				(char *)strcat(file_name,".pspread"),
				start_date);
		}
		else if ( strcmp(sequence_name,"PH" ) == 0) {
			strcpy(file_name, file_prefix);
			dated_input[0].PH = construct_dated_clim_sequence(
				(char *)strcat(file_name,".PH"),
				start_date);
		}
		else if ( strcmp(sequence_name,"grazing_Closs" ) == 0) {
			strcpy(file_name, file_prefix);
			dated_input[0].grazing_Closs = construct_dated_clim_sequence(
				(char *)strcat(file_name,".grazing_Closs"),
				start_date);
		}
		else  fprintf(stderr,"WARNING-clim sequence %s not found.\n",
			sequence_name);
	} /*end for*/

	return(dated_input);
} /*end construct_dated_input*/
