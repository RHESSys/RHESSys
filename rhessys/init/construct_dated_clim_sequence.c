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
	int	j;
	int	inx;
	int	start_flag;
	long	start_date_julian;
	double	value;
	int	num_records;
	int	num_days;
	int	num_hours;
	struct	clim_event_sequence events;
	FILE	*sequence_file;
	struct	date	cur_date;
	struct	date	tmp_date;
	
	/*--------------------------------------------------------------*/
	/*	Initialize							*/
	/*--------------------------------------------------------------*/
        num_days = 0;
	start_flag = 0;

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
	
	/*--------------------------------------------------------------*/
	/*	First, calculate how many day it has in the record			*/
	/*--------------------------------------------------------------*/
	tmp_date.year = start_date.year;
	tmp_date.month = start_date.month;
	tmp_date.day  = start_date.day;
	tmp_date.hour = start_date.hour;

	fscanf(sequence_file,"%d",&num_records);
	printf("\nThere are %d days in the dated climate file %s\n", num_records, file);
	for ( i=0 ; i<num_records ; i++ ){
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
			if (julday(cur_date) == julday(start_date)){
			    num_days=1;
			}
		  	if (julday(cur_date) > julday(tmp_date)){
			  num_days = num_days + 1;
			  tmp_date.year = cur_date.year;
			  tmp_date.month = cur_date.month;
			  tmp_date.day  = cur_date.day;
			  tmp_date.hour = cur_date.hour;
			}
		}
	}



	num_hours = 24 * num_days;
	/*--------------------------------------------------------------*/
	/*	Second, seek back to the begin of the file and start read data			*/
	/*--------------------------------------------------------------*/
	fseek(sequence_file,0,SEEK_SET);
	
	fscanf(sequence_file,"%d",&num_records);
	
	printf("\nRead dated climate input file  %s\n", file);	
	/*--------------------------------------------------------------*/
	/*	Allocate the clim sequence.									*/
	/*--------------------------------------------------------------*/
	events.seq = (struct dated_sequence *) alloc((num_hours+1) *
		sizeof(struct dated_sequence),
		"sequence","construct_dated_clim_sequence");
	events.inx = 0;
	events.seq[0].edate.year = 1999;
	/*--------------------------------------------------------------*/
	/*	Read in the climate sequence data.							*/
	/*--------------------------------------------------------------*/
	inx = 0;
	tmp_date.year = start_date.year;
	tmp_date.month = start_date.month;
	tmp_date.day  = start_date.day;
	tmp_date.hour = start_date.hour;
	for ( i=0 ; i<num_records ; i++ ){
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
			  if(julday(cur_date)>julday(tmp_date)){ 
			    /* start a new day */
			    /* make the value of the rest hours in the previous day to 0 */
			    if (tmp_date.hour<=24 && start_flag != 0){ // not the first day after start_date
				for(j=tmp_date.hour+1;j<=24;j++){
				  events.seq[inx].edate.year = tmp_date.year;
				  events.seq[inx].edate.month = tmp_date.month;
				  events.seq[inx].edate.day = tmp_date.day;
				  events.seq[inx].edate.hour = j;
				  events.seq[inx].value = 0.0;
				  inx += 1;
				}
				
			    }
			    /* make the value of the hours earlier than cur day to 0 */
			    if (cur_date.hour>=1){
				for(j=1;j<cur_date.hour;j++){
				  events.seq[inx].edate.year = cur_date.year;
				  events.seq[inx].edate.month = cur_date.month;
				  events.seq[inx].edate.day = cur_date.day;
				  events.seq[inx].edate.hour = j;
				  events.seq[inx].value = 0.0;
				  inx += 1;
				}
				start_flag=1;
			    }
				
				events.seq[inx].edate.year = cur_date.year;
				events.seq[inx].edate.month = cur_date.month;
				events.seq[inx].edate.day = cur_date.day;
				events.seq[inx].edate.hour = cur_date.hour;
				events.seq[inx].value = value;
				inx += 1;
			    }
			
			else{/* if there are multiple records within one day*/
			    if(start_flag==0){ //if it is the first record after start_date
			      	for(j=1;j<cur_date.hour;j++){
				  events.seq[inx].edate.year = cur_date.year;
				  events.seq[inx].edate.month = cur_date.month;
				  events.seq[inx].edate.day = cur_date.day;
				  events.seq[inx].edate.hour = j;
				  events.seq[inx].value = 0.0;
				  inx += 1;
				}
				events.seq[inx].edate.year = cur_date.year;
				events.seq[inx].edate.month = cur_date.month;
				events.seq[inx].edate.day = cur_date.day;
				events.seq[inx].edate.hour = cur_date.hour;
				events.seq[inx].value = value;
				inx += 1;
				start_flag = 1;

			    }
			   else{ for(j=tmp_date.hour+1;j<cur_date.hour;j++){
				events.seq[inx].edate.year = tmp_date.year;
				events.seq[inx].edate.month = tmp_date.month;
				events.seq[inx].edate.day = tmp_date.day;
				events.seq[inx].edate.hour = j;
				events.seq[inx].value = 0.0;
				inx += 1;
			    }
				events.seq[inx].edate.year = cur_date.year;
				events.seq[inx].edate.month = cur_date.month;
				events.seq[inx].edate.day = cur_date.day;
				events.seq[inx].edate.hour = cur_date.hour;
				events.seq[inx].value = value;
				inx += 1;
			   }


			}
				tmp_date.year = cur_date.year;
				tmp_date.month= cur_date.month;
				tmp_date.day  = cur_date.day;
				tmp_date.hour = cur_date.hour;	
			    if(i == num_records-1){
				for(j=tmp_date.hour+1;j<=24;j++){
				  events.seq[inx].edate.year = tmp_date.year;
				  events.seq[inx].edate.month = tmp_date.month;
				  events.seq[inx].edate.day = tmp_date.day;
				  events.seq[inx].edate.hour = j;
				  events.seq[inx].value = 0.0;
				  inx += 1;
				}
				
			    }

		      }
				
		}
	}
	events.seq[inx].edate.year = 0;




	return(events);
} /*end construct_dated_clim_sequence*/
