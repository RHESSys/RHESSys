/*--------------------------------------------------------------*/
/* 																*/
/*					construct_yearly_clim	 					*/
/*																*/
/*	construct_yearly_clim.c - makes a yearly clim object		*/
/*																*/
/*	NAME														*/
/*	construct_yearly_clim.c - makes a yearly clim object		*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates a climate object.		  							*/
/*	Computes the number of entries based on duration.			*/
/*	Allocates an array of critical clim data for n entries.		*/
/*																*/
/*	A clim object is simply a list of doubleing point values		*/
/*	however, by checking the index in the file containgin the   */
/*	list, the code insures that the list matches the number of	*/
/*	years in the simulation and extends from start to end year.	*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	A clim sequence file should be named with a prefix 			*/
/*	matching that in its base station file.  It will be			*/
/*	ignored if its suffix is not in the list of parameters		*/
/*	required at that time step.									*/
/*																*/
/* 	The clim_sequence is constructed by calling construct_		*/
/*	clim_sequence.  Refer to that code for documentation of		*/
/*	what a valid clim sequence is.								*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

struct	yearly_clim_object *construct_yearly_clim(
												  FILE	*base_station_file,
												  char	*file_prefix,
												  struct	date	start_date,
												  long	duration)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	double	*construct_clim_sequence( char *, struct date, long);
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	i;
	int	num_non_critical_sequences;
	char  record[MAXSTR];
	char	sequence_name[256];
	char	file_name[256];
	struct yearly_clim_object	*yearly_clim;
	
	/*--------------------------------------------------------------*/
	/*	Allocate the yearly clim object.								*/
	/*--------------------------------------------------------------*/
	yearly_clim = (struct yearly_clim_object *)
		alloc(1*sizeof(struct yearly_clim_object),
		"yearly_clim","construct_yearly_clim");
	
	/*--------------------------------------------------------------*/
	/*	Attempt to open the yearly clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*																*/
	/*	At the moment there are no critical clim parameters.		*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Initialize non-critical sequences			*/
	/*--------------------------------------------------------------*/
	yearly_clim[0].temp = NULL;
	
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
		if ( strcmp(sequence_name,"temp" ) == 0){
			strcpy(file_name, file_prefix);
			yearly_clim[0].temp = construct_clim_sequence(
				(char *)strcat(file_name,".temp"),
				start_date,
				duration);
		}
		else{
			fprintf(stderr,
				"WARNING -  clim sequence %s not found.\n",sequence_name);
		} /*end if-else*/
	} /*end for*/
	return(yearly_clim);
} /*end construct_yearly_clim*/
