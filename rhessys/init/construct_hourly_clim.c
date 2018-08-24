/*--------------------------------------------------------------*/
/* 																*/
/*					construct_hourly_clim	 					*/
/*																*/
/*	construct_hourly_clim.c - makes a hourly clim object		*/
/*																*/
/*	NAME														*/
/*	construct_hourly_clim.c - makes a hourly clim object		*/
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
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

struct	hourly_clim_object *construct_hourly_clim(
												  FILE	*base_station_file,
												  char	*file_prefix,
												  struct	date	start_date,
												  long	duration)
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
	char    record[MAXSTR];
	char	sequence_name[256];
	char	file_name[256];
	struct hourly_clim_object	*hourly_clim;
	
	/*--------------------------------------------------------------*/
	/*	Allocate the hourly clim object.								*/
	/*--------------------------------------------------------------*/
	hourly_clim = (struct hourly_clim_object *)
		alloc(1*sizeof(struct hourly_clim_object),
		"hourly_clim","construct_hourly_clim" );
	
	/*--------------------------------------------------------------*/
	/*	Attempt to open the hourly clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Initialize non - critical sequences.			*/
	/*--------------------------------------------------------------*/
	hourly_clim[0].rain.inx = -999;
	hourly_clim[0].rain_duration.inx = -999;
	
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
		if ( strcmp(sequence_name,"rain" ) == 0) {
			strcpy(file_name, file_prefix);
			hourly_clim[0].rain = construct_dated_clim_sequence(
				(char *)strcat(file_name,".rain"),
				start_date);
		}
		else if ( strcmp(sequence_name,"rain_duration" ) == 0){
			strcpy(file_name, file_prefix);
			hourly_clim[0].rain_duration = construct_dated_clim_sequence(
				(char *)strcat(file_name,".rain_duration"),
				start_date);
		}
		else  {fprintf(stderr,"WARNING-clim sequence %s not found.\n", sequence_name);
			exit(EXIT_FAILURE);
			}
	} /*end for*/
	return(hourly_clim);
} /*end construct_hourly_clim*/
