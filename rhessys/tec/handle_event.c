/*--------------------------------------------------------------*/
/* 																*/
/*						handle_event							*/
/*																*/
/*	handle_event - handles a tec event.							*/
/*																*/
/*	NAME														*/
/*	handle_event - handles a tec event.							*/
/*																*/
/*	SYNOPSIS													*/
/*	(void) handle_event(										*/
/*					struct	tec_entry	*event,					*/
/*					struct command_line_object *command_line,	*/
/*					struct	date	current_date,				*/
/*					struct world_object *world)					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine adjusts state variables based on the current	*/
/*	tec event.													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	This rotine is called by execute_tec.c just before the 		*/
/*	next tec entry is read.  This routine is called continually	*/
/*	with successive tec entries until the next tec entry has a	*/
/*	date later than the current tec entry.  In this manner 		*/
/*	multiple events can be executed before another simulation	*/
/*	time step.													*/
/*																*/
/*	March 6, 97 - C.Tague										*/
/*	-added a state output event 								*/
/* 	 current date added to handle_event parameter list			*/
/*																*/
/*	Sept, 98 - C.Tague	*/
/*	added comma delimited output event */
/*																*/
/*	June, 2014 - X Chen	*/
/*	added hourly growth output event*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

void	handle_event(
					 struct	tec_entry	*event,
					 struct command_line_object *command_line,
					 struct	date	current_date,
					 struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local Function Declarations.								*/
	/*--------------------------------------------------------------*/
	void	execute_redefine_strata_event(
		struct world_object *,
		struct command_line_object *,
		struct	date);
	void	execute_redefine_world_event(
		struct world_object *,
		struct command_line_object *,
		struct	date);
	void	execute_redefine_world_mult_event(
		struct world_object *,
		struct command_line_object *,
		struct	date);
	void	execute_redefine_world_thin_event(
		struct world_object *,
		struct command_line_object *,
		struct	date,
		int);
	void	execute_road_construction_event(
		struct world_object *,
		struct command_line_object *,
		struct	date);
	void	execute_state_output_event(
		struct world_object *,
		struct date,
		struct date,
		struct command_line_object *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Below is a list of events which are caught if present.		*/
	/*	OTHERWISE a fatal error ensues.								*/
	/*--------------------------------------------------------------*/

	if ( !strcmp(event[0].command,"none") ){
		/* nothing here */
	}
	else if ( !strcmp(event[0].command,"print_yearly_on") ){
		command_line[0].output_flags.yearly= 1;
		command_line[0].output_yearly_date.month = current_date.month;
		command_line[0].output_yearly_date.day = current_date.day;
	}
	else if ( !strcmp(event[0].command,"print_yearly_off") ){
		command_line[0].output_flags.yearly= 0;
	}
	else if ( !strcmp(event[0].command,"print_yearly_growth_on")){
		command_line[0].output_flags.yearly_growth = 1;
		command_line[0].output_yearly_date.month = current_date.month;
		command_line[0].output_yearly_date.day = current_date.day;
	}
	else if (  !strcmp(event[0].command,"print_yearly_growth_off")){
		command_line[0].output_flags.yearly_growth = 0;
	}
	else if ( !strcmp(event[0].command,"print_monthly_on") ){
		command_line[0].output_flags.monthly= 1;
	}
	else if ( !strcmp(event[0].command,"print_monthly_off") ){
		command_line[0].output_flags.monthly= 0;
	}
	else if ( !strcmp(event[0].command,"print_daily_on") ){
		command_line[0].output_flags.daily= 1;
	}
	else if ( !strcmp(event[0].command,"print_daily_off") ){
		command_line[0].output_flags.daily= 0;
	}
	else if ( !strcmp(event[0].command,"print_daily_growth_on")){
		command_line[0].output_flags.daily_growth = 1;
	}
	else if ( !strcmp(event[0].command,"print_daily_csv_growth_on")){
		command_line[0].output_flags.daily_growth = 1;
	}
	else if ( !strcmp(event[0].command,"print_daily_csv_on") ){
		command_line[0].output_flags.daily = 1;
	}
	else if ( !strcmp(event[0].command,"print_yearly_csv_on") ){
		command_line[0].output_flags.yearly = 1;
		command_line[0].output_yearly_date.month = current_date.month;
		command_line[0].output_yearly_date.day = current_date.day;
	}
	else if ( !strcmp(event[0].command,"print_monthly_csv_on") ){
		command_line[0].output_flags.monthly = 1;
	}
	else if ( !strcmp(event[0].command,"print_daily_growth_off")){
		command_line[0].output_flags.daily_growth = 0;
	}
	else if ( !strcmp(event[0].command,"print_daily_csv_growth_off")){
		command_line[0].output_flags.daily_growth = 0;
	}
	else if ( !strcmp(event[0].command,"print_daily_csv_off") ){
		command_line[0].output_flags.daily = 0;
	}
	else if ( !strcmp(event[0].command,"print_yearly_csv_off") ){
		command_line[0].output_flags.yearly = 0;
	}
	else if ( !strcmp(event[0].command,"print_monthly_csv_off") ){
		command_line[0].output_flags.monthly = 0;
	}
	else if ( !strcmp(event[0].command,"print_hourly_on") ){
		command_line[0].output_flags.hourly= 1;
	}
	else if ( !strcmp(event[0].command,"print_hourly_off") ){
		command_line[0].output_flags.hourly= 0;
	}
	else if ( !strcmp(event[0].command,"print_hourly_growth_on")){
		command_line[0].output_flags.hourly_growth = 1;
	}
	else if ( !strcmp(event[0].command,"print_hourly_growth_off")){
		command_line[0].output_flags.hourly_growth = 0;
	}
	else if ( !strcmp(event[0].command,"output_current_state") ){
		execute_state_output_event(world, current_date,
			world[0].end_date,command_line);
	}
	else if ( !strcmp(event[0].command,"redefine_strata") ){
		execute_redefine_strata_event(world, command_line, current_date);
	}
	else if ( !strcmp(event[0].command,"redefine_world") ){
		execute_redefine_world_event(world, command_line, current_date);
	}
	else if ( !strcmp(event[0].command,"redefine_world_multiplier") ){
		execute_redefine_world_mult_event(world, command_line, current_date);
	}	
	else if ( !strcmp(event[0].command,"redefine_world_thin_remain") ){
		execute_redefine_world_thin_event(world, command_line, current_date, 1);
	}		
	else if ( !strcmp(event[0].command,"redefine_world_thin_harvest") ){
		execute_redefine_world_thin_event(world, command_line, current_date, 2);
	}
	else if ( !strcmp(event[0].command,"redefine_world_thin_snags") ){
		execute_redefine_world_thin_event(world, command_line, current_date, 3);
	}			
	else if ( !strcmp(event[0].command,"roads_on") ){
		command_line[0].road_flag = 1;
		execute_road_construction_event(world, command_line, current_date);
	}
	else if ( !strcmp(event[0].command,"roads_off") ){
		command_line[0].road_flag = 0;
	}
	else{
		fprintf(stderr,"FATAL ERROR: in handle event - event %s not recognized.\n",
			event[0].command);
		exit(EXIT_FAILURE);
	}
} /*end handle_event*/

