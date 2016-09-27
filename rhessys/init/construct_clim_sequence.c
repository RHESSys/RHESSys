/*--------------------------------------------------------------*/
/* 																*/
/*					construct_clim_sequence	 					*/
/*																*/
/*	construct_clim_sequence - reads in sequence of climate data	*/ 
/*																*/
/*	NAME														*/
/*	construct_clim_sequence - reads in sequence of climate data	*/ 
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	WORKS ONLY FOR DAILY SEQUENCES AT THE MOMENT				*/
/*	Allocates an array of doubles to hold the clim sequence.		*/
/*	Attempts to open the file containing the clim sequence.		*/
/*	Searches for the first occurrence of the start date in		*/
/*		the clim sequence file.									*/
/*	Reads clim data into the allocated array as long as the		*/
/*		EOF is not present.										*/
/*	Returns the clim sequence array.							*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	A clim sequence file has the following format:				*/
/*																*/
/*	<start of file>												*/
/*	first_calendar_date											*/		
/*	value1														*/
/*	value2												*/
/*	value3												*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	valuen												*/
/*																*/
/*	valuei is a double (we hope doubles are not needed) corr-	*/
/*	esponding to the clim value for datei.						*/
/*																*/
/*	Ideally, the datei's increment by 1 and include the range	*/
/*	from start_date to end_date inclusive.  However, any		*/
/*	records BEFORE the first occurence of a record with 		*/
/*	datei=start_date OR AFTER the next subsequent occurrent of	*/
/*	the end_date will be skipped.  								*/
/*																*/
/*	Note that we assume that entries corresponding to 			*/
/*	dates increasing by one time step.  						*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

double *construct_clim_sequence(char *file, struct date start_date,
								long duration, int clim_repeat_flag)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	*alloc(size_t, char *, char *);
	long	julday(struct date);
	struct  date caldat(long);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	target_fnd;
	long	i,j;
	long	first_date_julian;
	long	start_date_julian;
	long	target_date_julian;
	long	offset;
	double	value;
	double	*sequence;
	FILE	*sequence_file;
	struct	date	first_date;
	struct	date	target_date, curr_date;


	/*--------------------------------------------------------------*/
	/*	Allocate the clim sequence.									*/
	/*--------------------------------------------------------------*/
	sequence = (double *) alloc(duration*sizeof(double),
		"sequence","construct_clim_sequence");
	/*--------------------------------------------------------------*/
	/*	Try to open the file containing the clim sequence.			*/
	/*--------------------------------------------------------------*/
	if ( (sequence_file = fopen(file, "r") ) == NULL ){
		fprintf(stderr,
			"\nFATAL ERROR: in construct_clim_sequence\nunable to open sequence file %s\n", file);
		fprintf(stderr, "\nFile name %s", file);
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Read in start date in clim file (calendar date to hour res)	*/
	/*--------------------------------------------------------------*/
	fscanf(sequence_file,"%ld %ld %ld %ld",&first_date.year,
		&first_date.month,&first_date.day,&first_date.hour);
	/*--------------------------------------------------------------*/
	/*	Compute julian date of first date in sequence and 	start	*/
	/*	date of world.												*/
	/*--------------------------------------------------------------*/
	first_date_julian = julday(first_date);
	start_date_julian = julday(start_date);
	/*--------------------------------------------------------------*/
	/*	Compute offset to reach start date.							*/
	/*--------------------------------------------------------------*/
	offset = (start_date_julian - first_date_julian);

	/*--------------------------------------------------------------*/
	/* 	If the start date is before the first date error.			*/
	/*--------------------------------------------------------------*/
	if ( offset < 0 ){
		fprintf(stderr,
			"FATAL ERROR: start date before first date of a clim sequence.\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	Scan forwards in the sequence until the start date.			*/
	/*--------------------------------------------------------------*/
	for ( i = 0 ; i<offset ; i++ ){
		if ( fscanf(sequence_file,"%lf",&value) == EOF  ) {
			fprintf(stderr,"FATAL ERROR: in construct_clim_sequence\n - start date beyond eof"); 
			exit(EXIT_FAILURE);
		}
	}
	/*--------------------------------------------------------------*/
	/*	Read in the climate sequence data.							*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<duration ; i++ ){
		if ( fscanf(sequence_file,"%lf",&value) == EOF  ) {
			if (clim_repeat_flag == 0) {
			fprintf(stderr,"FATAL ERROR: in construct_clim_sequence\n");
			fprintf(stderr,"\n end date beyond end of clim sequence\n");
			exit(EXIT_FAILURE);
			}
			else {
			target_date_julian = first_date_julian + offset + i;
			target_date = caldat(target_date_julian);
			target_fnd = 0;
			j = 0;
			while ((target_fnd == 0) && (j < i)) {	
				curr_date = caldat(first_date_julian + offset + j);
				if ((curr_date.month == target_date.month) 
					&& (curr_date.day == target_date.day)) target_fnd=1;
				j = j+1;
			}
			if (j < i)
				*(sequence+i) = *(sequence+j);
			else {
				fprintf(stderr,"FATAL ERROR: in construct_clim_sequence\n");
				fprintf(stderr,"\n not enough data in base climate to repeat\n");
				exit(EXIT_FAILURE);
				}
			}
		}
		else{
			*(sequence+i) = value;
		}
	}
	fclose(sequence_file);
	return(sequence);
} /*end construct_clim_sequence*/
