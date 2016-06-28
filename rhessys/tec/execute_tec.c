/*--------------------------------------------------------------*/
/* 																*/
/*						execute_tec_file						*/
/*																*/
/*	execute_tec_file.c - executes the tec file with the world	*/
/*																*/
/*	NAME														*/
/*	execute_tec_file.c - executes the tec file with the world	*/
/*																*/
/*	SYNOPSIS													*/
/*	void *execute_tec(											*/
/*			struct	tec_object,									*/
/*			struct command_line_object,					 		*/
/*			struct	world_output_file_object,					*/
/*			struct	world_output_file_object,					*/
/*			struct world_object );								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	To encourage future parellelism, operations on groups of	*/
/*	objects are grouped together (e.g. all canopy processes		*/
/*	for a given patch are put together) in an event loop.		*/
/*																*/
/*	The event loop cycles through an a priori temporal and		*/
/*	spatial hierarchy.  At the moment the hierarchies are:		*/
/*																*/
/*	SPATIAL						TEMPORAL						*/
/*	world						yearly							*/
/*	basin						monthly							*/
/*	hillslope					daily							*/
/*	zone						hourly							*/
/*	patch														*/
/*	stratum 													*/
/*																*/
/*	These levels are somewhat arbitrary but changes should be	*/
/*	documented.  Refer to the rhessys.h file for documentation 	*/
/*	of the spatial object hierarchy.							*/
/*																*/
/*	The temporal levels are handeled as implicit events.		*/
/*	For simplicity no sub-daily events are handled at the   	*/
/*	moment.														*/
/*																*/
/*	Note: It is important to start at the beginning of a day	*/
/*			and end before the beginning of a day since there	*/
/*			are day start and day end simulation calls.  So, 	*/
/*			make sure that the start and end dates have hour =1	*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	There is only one tec event that is kept track of.  		*/
/*	This is the next event.  									*/
/*																*/
/*	This implementation of the event loop allows a comprimise 	*/
/*	between a clean spatial and temporal layout of processes	*/
/*	and the desire to reduce RAM allocation.					*/
/*																*/
/*	The event loop starts at the world start date and continues	*/
/*	up to , but not including , the world end date.  For the	*/
/*	purposes of this code, the first hour in a day is hour 1 and*/
/*	the last hour is hour 24.  									*/
/*																*/
/*	The event loop has an hourly time step in which is does the	*/
/*	following:													*/
/*																*/
/*	1.  Initialze the event queue with a null event and set the	*/
/*			date to the start date.								*/
/*	2.  check if the current date is less than the end date		*/
/*		(if not the the event loop ends.						*/
/*	3.  	Perform the next event in the event queue.			*/
/*	4.  	Read the next event from tec file into event queue.	*/
/*			(if there are no more events set the next event to	*/
/*			 a null event).										*/
/*	5.  	check if  current date is less than the date of the	*/
/*			next event (if not go to 13).						*/
/*	6.			if hour = 1 call world_daily_I 			        */
/*	7.			call world_hourly								*/
/*	8.  		if hour = 24 	call world_daily_F				*/
/*								increment the day by one		*/
/*	9.			if the month of the new date does not equal that*/
/*					of the last day's date increment the month 	*/
/*	10.			if the year of the new date does not equal that*/
/*					of the last day's date increment the year 	*/
/*	11. 		increment the hour								*/
/*	12. 		goto 5											*/ 
/*	13.		At the moment there is no event handling here.		*/
/*	14.		goto 2												*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	execute_tec(
					struct	tec_object *tecfile ,
					struct command_line_object *command_line,
					struct	world_output_file_object *outfile,
					struct	world_output_file_object *growth_outfile,
					struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local Function Declarations.								*/
	/*--------------------------------------------------------------*/
	int		cal_date_lt(struct date, struct date );
	
	long	julday( struct date );
	
	struct	date	caldat( long );
	
	struct	tec_entry	*construct_tec_entry( struct date, char * );
	
	void	world_daily_I(
		long,
		struct world_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void	world_hourly(
		struct world_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void	world_daily_F(
		long,
		struct world_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void	handle_event(
		struct	tec_entry	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_object	*);
	
	void	execute_yearly_growth_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);

	void	execute_yearly_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);
	
	void	execute_daily_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);
	
	void	execute_daily_growth_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);

	void	execute_monthly_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);
	
	void	execute_hourly_output_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date,
		struct	world_output_file_object *);

	void	execute_firespread_event(
		struct	world_object	*,
		struct	command_line_object	*,
		struct	date);
	
	void	execute_state_output_event(
		struct world_object *,
		struct date,
		struct date,
		struct command_line_object *);

	/*--------------------------------------------------------------*/
	/*	Local Variable Definition. 									*/
	/*--------------------------------------------------------------*/
	int check;
	long	day;
	long	hour;
	long	month;
	long	year;
	struct	date	current_date;
	struct	date	next_date;
	struct	tec_entry	*event;
	
	/*--------------------------------------------------------------*/
	/*	Initialize the indices into the base station clime sequences*/
	/*--------------------------------------------------------------*/
	year = 0;
	month = 0;
	day = 0;
	hour = 0;
	
	/*--------------------------------------------------------------*/
	/*	Initialize the tec event									*/
	/*--------------------------------------------------------------*/
	
	event =  construct_tec_entry(world[0].end_date,"none");
	
	/*--------------------------------------------------------------*/
	/*	Loop from the start of the world to the end of the world.	*/
	/*--------------------------------------------------------------*/
	current_date = world[0].start_date;
	next_date = current_date;
	while ( cal_date_lt(current_date,world[0].end_date)){
		/*--------------------------------------------------------------*/
		/*		Perform the tec event.									*/
		/*--------------------------------------------------------------*/
		handle_event(event,command_line,current_date,world);
		/*--------------------------------------------------------------*/
		/*		read the next tec file entry.							*/
		/*		if we are not at the end of the tec file.				*/
		/*--------------------------------------------------------------*/
		if ( !(feof(tecfile[0].tfile))){
			/*--------------------------------------------------------------*/
			/*			read in the next tec line.							*/
			/*--------------------------------------------------------------*/
			check = fscanf(tecfile[0].tfile,"%d %d %d %d %s\n",
				&(event[0].cal_date.year),
				&(event[0].cal_date.month),
				&(event[0].cal_date.day),
				&(event[0].cal_date.hour),
				event[0].command);
			/*--------------------------------------------------------------*/
			/*			report an error if for some reason we cant read it	*/
			/*--------------------------------------------------------------*/
			if ( !check ){
				fprintf(stderr,"\nERROR:  the tec file is corrupted.");
				fclose(tecfile[0].tfile);
				exit(EXIT_FAILURE);
			} /*end if*/
		} /*end if*/
		/*--------------------------------------------------------------*/
		/* 	if end of tec file next event is the end of the world		*/
		/*--------------------------------------------------------------*/
		else{
			event =  construct_tec_entry(world[0].end_date, "none");
		} /*end if-else*/
		/*--------------------------------------------------------------*/
		/*		If the next event's date exceeds the end_date then		*/
		/*		set the next event to nothing and at a time at the 		*/
		/*		end of the simulation.									*/
		/*--------------------------------------------------------------*/
		if ( cal_date_lt(event[0].cal_date,	world[0].end_date) == 0  ){
			event =  construct_tec_entry(world[0].end_date,	"none");
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		Do stuff until the next tec event.						*/
		/*--------------------------------------------------------------*/
		while ( cal_date_lt(current_date, event[0].cal_date)){
			/*--------------------------------------------------------------*/
			/*			Simulate the world for the start of this day e		*/
			/*--------------------------------------------------------------*/
            if ( current_date.hour == 1 ) printf("Current_date year = %d mon = %d day = %d\r",
                   current_date.year,current_date.month,current_date.day);
            //fflush(stdout);
			if ( current_date.hour == 1 ){
                world_daily_I(
					day,
					world,
					command_line,
					event,
                    current_date);
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*          Do hourly stuff for the day.                        */
			/*--------------------------------------------------------------*/
            world_hourly( world,
				command_line,
				event,
                current_date);
			
			/*--------------------------------------------------------------*/
			/*			Perform any requested hourly output					*/
			/*--------------------------------------------------------------*/
			if (command_line[0].output_flags.hourly == 1){
				execute_hourly_output_event(
							  world,
							  command_line,
							  current_date,
							  outfile);
			}

			if(command_line[0].output_flags.hourly_growth ==1 &&
					(command_line[0].grow_flag > 0) ){
				  execute_hourly_growth_output_event(
							      world, 
							      command_line, 
							      current_date, 
							      growth_outfile);
				  
				};
			/*--------------------------------------------------------------*/
			/*			Increment to the next hour.							*/
			/*--------------------------------------------------------------*/
			current_date.hour++;
			/*--------------------------------------------------------------*/
			/*			Check if this is a day end.							*/
			/*--------------------------------------------------------------*/
			if ( current_date.hour == 25 ){
				/*--------------------------------------------------------------*/
				/*			Simulate the world for the end of this day e		*/
				/*--------------------------------------------------------------*/
                world_daily_F(
					day,
					world,
					command_line,
					event,
                    current_date);
				/*--------------------------------------------------------------*/
				/*			Perform any requested daily output					*/
				/*--------------------------------------------------------------*/
				if ((command_line[0].output_flags.daily_growth == 1) &&
							(command_line[0].grow_flag > 0) ) {
						execute_daily_growth_output_event(
						world,
						command_line,
						current_date,
						growth_outfile);
				}
				if (command_line[0].output_flags.daily == 1) {
						execute_daily_output_event(
						world,
						command_line,
						current_date,
						outfile);
                               }
				/*--------------------------------------------------------------*/
        /*  Output world state in spinup mode if targets met            */
				/*--------------------------------------------------------------*/

				if((command_line[0].vegspinup_flag > 0) && (world[0].target_status > 0)) {
		      execute_state_output_event(world, current_date, world[0].end_date,command_line);
          printf("\nSpinup completed YEAR %d MONTH %d DAY %d \n", current_date.year,current_date.month,current_date.day);
          exit(0);
        } 

				/*--------------------------------------------------------------*/
				/*			Perform any requested yearly output					*/
				/*--------------------------------------------------------------*/
				if ((command_line[0].output_flags.yearly == 1) &&
					(command_line[0].output_yearly_date.month==current_date.month)&&
					(command_line[0].output_yearly_date.day == current_date.day))
							execute_yearly_output_event(
							world,
							command_line,
							current_date,
							outfile);

				if ((command_line[0].output_flags.yearly_growth == 1) &&
					(command_line[0].output_yearly_date.month==current_date.month)&&
					(command_line[0].output_yearly_date.day == current_date.day) &&
					(command_line[0].grow_flag > 0) )
					execute_yearly_growth_output_event(
					world,
					command_line,
					current_date,
					growth_outfile);
				/*--------------------------------------------------------------*/
				/*				Determine the new calendar date if we add 1 day.*/
				/*				Do this by first conversting the current cal	*/
				/* 				endar date into a julian day.  Then adding one	*/
				/*				to the julian day and the converting back to	*/
				/*				get tomorrows calendar date.					*/
				/*				We assume that it starts at hour 1.				*/
				/*--------------------------------------------------------------*/
				day = day + 1;
				next_date = caldat(julday(current_date)+1);
				current_date.day = next_date.day;
				current_date.hour = next_date.hour;
				if (command_line[0].verbose_flag > 0)
					fprintf(stderr,"\n\nYEAR %d MONTH %d DAY %d\n\n",
					current_date.year,current_date.month,current_date.day);
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*			Check if this is a month end.						*/
			/*--------------------------------------------------------------*/
			if ( next_date.month !=	current_date.month ){
				/*--------------------------------------------------------------*/
				/*				Do monthly stuff.								*/
				/*--------------------------------------------------------------*/

				/*--------------------------------------------------------------*/
				/* if fire spread is called - initiate fire spread routine 	*/
				/*--------------------------------------------------------------*/
				if (command_line[0].firespread_flag == 1) {
					execute_firespread_event(
						world,
						command_line,
						current_date);
				}	
				
				/*--------------------------------------------------------------*/
				/*			Perform any requested monthly output				*/
				/*--------------------------------------------------------------*/
				if (command_line[0].output_flags.monthly == 1)
						execute_monthly_output_event(
						world,
						command_line,
						current_date,
						outfile);
				/*--------------------------------------------------------------*/
				/*				increment month 								*/
				/*--------------------------------------------------------------*/
				month = month + 1;
				current_date.month = next_date.month;
			} /* end if */
			/*--------------------------------------------------------------*/
			/*			Check if this is a year end.						*/
			/*--------------------------------------------------------------*/
			if ( next_date.year != current_date.year ){
				/*--------------------------------------------------------------*/
				/*				Do yearly stuff.								*/
				/*--------------------------------------------------------------*/
				
				/*--------------------------------------------------------------*/
				/*				increment year  								*/
				/*-------------------------------------------------------------*/
                printf("\nYear %d\n", current_date.year);
				year = year + 1;
				current_date.year= next_date.year;
			}  /*end if*/
			} /*end while*/
		} /*end while*/
		return;
} /*end execute_tec.c*/
