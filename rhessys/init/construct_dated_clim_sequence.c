/*--------------------------------------------------------------*/
/* 																*/
/*					construct_dated_clim_sequence	 					*/
/*																*/
/*	construct_dated_clim_sequence - reads in sequence of climate data	*/ 
/*																*/
/*	NAME														*/
/*	construct_dated_clim_sequence - reads in sequence of climate data	*/ 
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

struct clim_event_sequence construct_dated_clim_sequence(
														 char *file, struct date start_date)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	long julday(struct date );
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	long	i;
	int	inx;
	long	start_date_julian;
	double	value;
	int	duration;
	struct	clim_event_sequence events;
	FILE	*sequence_file;
	struct	date	cur_date;
	
	/*--------------------------------------------------------------*/
	/*      Compute julian date of start 				  */
	/*      date of world.						  */
	/*--------------------------------------------------------------*/
	start_date_julian = julday(start_date);
	
	/*--------------------------------------------------------------*/
	/*	Try to open the file containing the clim sequence.			*/
	/*--------------------------------------------------------------*/
	if ( (sequence_file = fopen(file, "r") ) == NULL ){
		fprintf(stderr,
			"\nFATAL ERROR: in construct_dated_clim_sequence\nunable to open sequence file %s\n",
			file);
		exit(EXIT_FAILURE);
	} /*end if*/
	
	printf("\nReading dated sequence %s\n", file);

	fscanf(sequence_file,"%d",&duration);
	/*--------------------------------------------------------------*/
	/*	Allocate the clim sequence.									*/
	/*--------------------------------------------------------------*/
	events.seq = (struct dated_sequence *) alloc((duration+1) *
		sizeof(struct dated_sequence),
		"sequence","construct_dated_clim_sequence");
	events.inx = 0;
	
	/*--------------------------------------------------------------*/
	/*	Read in the climate sequence data.							*/
	/*--------------------------------------------------------------*/
	inx = 0;
	for ( i=0 ; i<duration ; i++ ){
		if(fscanf(sequence_file,"%ld %ld %ld %ld %lf",
			&cur_date.year,
			&cur_date.month,
			&cur_date.day,
			&cur_date.hour,
			&value) == EOF){
			fprintf(stderr,"FATAL ERROR: in construct_dated_clim_sequence\n");
			exit(EXIT_FAILURE);
		}
		else{
			if (julday(cur_date) >= start_date_julian){
				events.seq[inx].edate.year = cur_date.year;
				events.seq[inx].edate.month = cur_date.month;
				events.seq[inx].edate.day = cur_date.day;
				events.seq[inx].edate.hour = cur_date.hour;
				events.seq[inx].value = value;
				inx += 1;
			}
		}
	}
	events.seq[inx].edate.year = 0;
	//test
	for (i=0; i<inx; i++){
		printf("%d %d %d %d %f\n",events.seq[i].edate.year,events.seq[i].edate.month,events.seq[i].edate.day,events.seq[i].edate.hour,events.seq[i].value);
	}
	return(events);
} /*end construct_dated_clim_sequence*/
