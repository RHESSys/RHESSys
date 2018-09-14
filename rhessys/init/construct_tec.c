/*--------------------------------------------------------------*/
/* 																*/
/*						construct_tec_file						*/
/*																*/
/*	construct_tec_file.c - creates a tec file and checks it  OK	*/
/*																*/
/*	NAME														*/
/*	construct_tec_file.c - creates a tec file and checks it  OK	*/
/*																*/
/*	SYNOPSIS													*/
/*	struct tec_object *construct_tec_file(						*/
/*								struct command_line_object 		*/
/*			*command_line, struct world_object *world);			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	The code creates a tec file object (i.e. a file pointer		*/
/*	to a valid tec file).										*/
/*																*/
/*	A tec (temporal event control) file is an ascii				*/
/*	file which can be used with rhessys to govern temporal		*/
/*	response variables' output.  The use of						*/
/*	a tec file with rhessys is optional and is specified with	*/
/*	the '-t' option.											*/
/*																*/
/*	A tec file has the following format:						*/
/*																*/
/*      <start of file>     									*/ 
/*		date1	command1										*/
/*		date2	command2										*/
/*		date3	command3										*/
/*          .                                                	*/ 
/*          .                                                  	*/ 
/*          .                                                 	*/ 
/*		daten	commandn										*/
/*      <end of file>                                           */ 
/*                                                             	*/ 
/*      The additional constraints placed upon this file are:  	*/    
/*                                                            	*/ 
/*		1.  The dates must lie within the startday and endday.	*/
/*      2.  The dates must be listed in non-decreasing orderfrom*/ 
/*          the first to nth entry pair.                       	*/ 
/*		3.  The dates must be listed as							*/
/*				year month_of_year day_of_month hour_of_day		*/
/*			and must be a valid date.							*/
/*                                                             	*/ 
/*	This routine now enforces the 3 conditions above or returns	*/
/*	a fatal error.												*/
/*																*/
/*		TEC FILE COMMANDS										*/
/*																*/
/*		This is a list of tec file commands which the 			*/
/*		simulation will act upon.  Other commands should		*/
/*		result in an error.										*/
/*																*/
/*		print_yearly_on - start printing of yearly output data	*/
/*		print_monthly_on - start printing of monthly output data*/
/*		print_daily_growth_on - start printing of daily output data	*/
/*		print_daily_on - start printing of daily output data	*/
/*		print_hourly_on - start printing of hourly output data	*/
/*		print_yearly_off - stop printing of yearly output data	*/
/*		print_monthly_off - stop printing of monthly output data*/
/*		print_daily_off - stop printing of daily output data	*/
/*		print_daily_growth_off - stop printing of daily output data	*/
/*		print_hourly_off - stop printing of hourly output data	*/
/*		redefine_strata - redefine strata based on a file	*/
/*		road_on		 - start processing roads		*/
/*		road_off	 - stop processing roads		*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*	This routine used to just validate the tec file but I 		*/
/*	changed it to return a FILE pointer to the opened tec		*/
/*	file.  As such , it now makes sense to pull the validation	*/
/*	code out of this creation routine and place it in a  		*/
/*	subroutine.													*/
/*																*/
/*	At present the tec file commands only start and stop        */
/*	printing.  It may be useful to extend this to a list		*/
/*	of selected output state variables.							*/
/*	This however would require substantial checking in the		*/
/*	code here and elsewhere to catch the required output		*/
/*	variables and print them out (and also warn/error if it		*/
/*	is not possible to print out a specified state variable).	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

struct tec_object	*construct_tec(
								   struct command_line_object *command_line,
								   struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local Function Definition									*/
	/*--------------------------------------------------------------*/
	int cal_date_lt(struct date, struct date);
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local Variable Definition. 									*/
	/*--------------------------------------------------------------*/
	char	command[256];
	int		check;
	struct	date	current_date;
	struct	date	old_date;
	struct	tec_object 	*tecfile;
	/*--------------------------------------------------------------*/
	/*	Allocate a tec file object  								*/
	/*--------------------------------------------------------------*/
	tecfile = (struct tec_object *) alloc( 1 *
		sizeof(struct tec_object),"tecfile","construct_tec" );
	/*--------------------------------------------------------------*/
	/*	Attempt to open the tec file.								*/
	/*--------------------------------------------------------------*/
	if ((tecfile[0].tfile = fopen(command_line[0].tec_filename, "r")) == NULL){
		fprintf(stderr,"\nERROR:  cannot open tec file %s",
			command_line[0].tec_filename);
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Read a line of the tec file if it exists.					*/
	/*--------------------------------------------------------------*/
	check = fscanf(tecfile[0].tfile,"%d %d %d %d %s\n",
		&(current_date.year),
		&(current_date.month),
		&(current_date.day),
		&(current_date.hour),
		command);
	/*--------------------------------------------------------------*/
	/*	Assume that the earliest date possible is the startdate.	*/
	/*--------------------------------------------------------------*/
	old_date = world[0].start_date;
	/*--------------------------------------------------------------*/
	/*	Read every line until end of file or an error.				*/
	/*--------------------------------------------------------------*/
	while( !feof(tecfile[0].tfile ) ){
		/*--------------------------------------------------------------*/
		/*		Report a fatal error if line is incorrect.				*/
		/*--------------------------------------------------------------*/
		if ( !check ){
			fprintf(stderr,"\nERROR:  the tec file is corrupted.\n");
			fclose(tecfile[0].tfile);
			exit(EXIT_FAILURE);
		} /*end if*/
		
		/*--------------------------------------------------------------*/
		/*		make sure that the date read in is not before the last	*/
		/*		date.													*/
		/*--------------------------------------------------------------*/
		if ( cal_date_lt(current_date,old_date)){
			fprintf(stderr,
					"FATAL ERROR: in construct_tec: dates not in sequence: %d/%d/%d %d; %d/%d/%d %d\n",
					current_date.year, current_date.month, current_date.day, current_date.hour,
					old_date.year, old_date.month, old_date.day, old_date.hour);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		make sure that the control flag makes sense.			*/
		/*--------------------------------------------------------------*/
		if ( (strcmp(command,"print_yearly_on") != 0) &&
			(strcmp(command,"print_monthly_on") != 0) &&
			(strcmp(command,"print_daily_on") != 0) &&
			(strcmp(command,"print_daily_growth_on") != 0) &&
			(strcmp(command,"print_yearly_growth_on") != 0) &&
			(strcmp(command,"print_daily_csv_growth_on") != 0) &&
			(strcmp(command,"print_daily_csv_on") != 0) &&
			(strcmp(command,"print_monthly_csv_on") != 0) &&
			(strcmp(command,"print_yearly_csv_on") != 0) &&
			(strcmp(command,"print_hourly_on") != 0) &&
			(strcmp(command,"print_hourly_growth_on") != 0) &&
			(strcmp(command,"print_monthly_off") != 0) &&
			(strcmp(command,"print_daily_off") != 0) &&
			(strcmp(command,"print_daily_growth_off") != 0) &&
			(strcmp(command,"print_yearly_growth_off") != 0) &&
			(strcmp(command,"print_daily_csv_growth_off") != 0) &&
			(strcmp(command,"print_daily_csv_off") != 0) &&
			(strcmp(command,"print_monthly_csv_off") != 0) &&
			(strcmp(command,"print_yearly_csv_off") != 0) &&
			(strcmp(command,"print_hourly_off") != 0) &&
			(strcmp(command,"print_hourly_growth_off") != 0) &&
			(strcmp(command,"print_yearly_off") != 0) &&
			(strcmp(command,"redefine_strata") != 0) &&
			(strcmp(command,"redefine_world") != 0) &&
			(strcmp(command,"redefine_world_multiplier") != 0) &&
			(strcmp(command,"redefine_world_thin_remain") != 0) &&
			(strcmp(command,"redefine_world_thin_harvest") != 0) &&		
			(strcmp(command,"redefine_world_thin_snags") != 0) &&			
			(strcmp(command,"roads_on") != 0) &&
			(strcmp(command,"roads_off") != 0) &&
			(strcmp(command,"output_current_state") != 0)  ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_tec bad command %s for date %d %d %d %d\n ",
				command, current_date.year,
				current_date.month, current_date.day,
				current_date.hour);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*	Read a line of the tec file if it exists.					*/
		/*--------------------------------------------------------------*/
		check = fscanf(tecfile[0].tfile,"%d %d %d %d %s\n",
			&(current_date.year),
			&(current_date.month),
			&(current_date.day),
			&(current_date.hour),
			command);
	} /*end while*/
	/*--------------------------------------------------------------*/
	/*	Move the file pointer to the start of the tecfile and return*/
	/*--------------------------------------------------------------*/
	fseek( tecfile[0].tfile, 0L, 0L);
	return(tecfile);
} /*end construct_tecfile.c*/
