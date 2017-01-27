/*--------------------------------------------------------------*/
/* 																*/
/*					construct_daily_clim	 					*/
/*																*/
/*	construct_daily_clim.c - makes a daily clim object			*/
/*																*/
/*	NAME														*/
/*	construct_daily_clim.c - makes a daily clim object			*/
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
/*  dayss in the simulation and extends from start to end 		*/
/*	julian day.													*/
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
/*  Feb 28/95 - Richard Fernandes				*/
/*	Took out warnings that optional clim squences were not	*/
/*	found (this should error fatally instead).		*/
/*	It will now do so in the contruct_clim_sequence code	*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

struct	daily_clim_object *construct_daily_clim(
												FILE	*base_station_file,
												char	*file_prefix,
												struct	date	start_date,
												long	duration,
												int	clim_repeat_flag)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	double	*construct_clim_sequence( char *, struct date, long, int);
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	int	i;
	int	num_non_critical_sequences;
	char  record[MAXSTR];
	char	sequence_name[256];
	struct daily_clim_object	*daily_clim;
	char	file_name[256];
	/*--------------------------------------------------------------*/
	/*	Attempt to open the daily clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*--------------------------------------------------------------*/
	/*	Allocate the daily clim object.								*/
	/*--------------------------------------------------------------*/
	daily_clim = (struct daily_clim_object *)
		alloc(1*sizeof(struct daily_clim_object),"daily_clim",
		"construct_daily_clim" );
	
	/*--------------------------------------------------------------*/
	/*	Attempt to open the daily clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*--------------------------------------------------------------*/
	strcpy(file_name, file_prefix);
	daily_clim[0].tmin = construct_clim_sequence(
		(char *)strcat(file_name,".tmin"),
		start_date,
		duration, clim_repeat_flag);
	strcpy(file_name, file_prefix);
	daily_clim[0].tmax = construct_clim_sequence(
		(char *)strcat(file_name,".tmax"),
		start_date,
		duration, clim_repeat_flag);
	strcpy(file_name, file_prefix);
	daily_clim[0].rain = construct_clim_sequence(
		(char *)strcat(file_name,".rain"),
		start_date,
		duration, clim_repeat_flag);
	/*--------------------------------------------------------------*/
	/*	initialize the rest of the clim sequences as null	*/
	/*--------------------------------------------------------------*/
	daily_clim[0].atm_trans = NULL;
	daily_clim[0].CO2 = NULL;
	daily_clim[0].cloud_fraction = NULL;
	daily_clim[0].cloud_opacity = NULL;
	daily_clim[0].dayl = NULL;
	daily_clim[0].Delta_T = NULL;
	daily_clim[0].dewpoint = NULL;
	daily_clim[0].base_station_effective_lai = NULL;
	daily_clim[0].Kdown_diffuse = NULL;
	daily_clim[0].Kdown_direct = NULL;
	daily_clim[0].LAI_scalar = NULL;
	daily_clim[0].Ldown = NULL;
	daily_clim[0].PAR_diffuse = NULL;
	daily_clim[0].PAR_direct = NULL;
	daily_clim[0].daytime_rain_duration = NULL;
	daily_clim[0].relative_humidity = NULL;
	daily_clim[0].snow = NULL;
	daily_clim[0].tdewpoint = NULL;
	daily_clim[0].tday = NULL;
	daily_clim[0].tnight = NULL;
	daily_clim[0].tnightmax = NULL;
	daily_clim[0].tavg = NULL;
	daily_clim[0].tsoil = NULL;
	daily_clim[0].vpd = NULL;
	daily_clim[0].wind = NULL;
	daily_clim[0].wind_direction = NULL;
	daily_clim[0].ndep_NO3 = NULL;
	daily_clim[0].ndep_NH4 = NULL;
	daily_clim[0].lapse_rate_tmax = NULL;
	daily_clim[0].lapse_rate_tmin = NULL;
	daily_clim[0].lapse_rate_precip = NULL;
	
	/*--------------------------------------------------------------*/
	/*	Read the still open base station file for the number of		*/
	/*	non-critical parameters.									*/
	/*--------------------------------------------------------------*/
	fscanf(base_station_file,"%d", &num_non_critical_sequences);
	read_record(base_station_file, record);

	printf("\n Non critical sequences %d", num_non_critical_sequences); 
	/*--------------------------------------------------------------*/
	/*	Loop through all of the non-critical sequences and attempt	*/
	/*	to construct them.											*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<num_non_critical_sequences ; i++ ){
		/*--------------------------------------------------------------*/
		/*		read in the non-critical_sequence name.					*/
		/*--------------------------------------------------------------*/
		fscanf(base_station_file,"%s",sequence_name);
		printf(" \n %s", sequence_name);
		read_record(base_station_file, record);
		/*--------------------------------------------------------------*/
		/*		test the sequence name and create it if it is valid.	*/
		/*		otherwise report a warning.								*/
		/*--------------------------------------------------------------*/
		if ( strcmp(sequence_name,"dayl") == 0 ){
			strcpy(file_name, file_prefix);
			printf("\n Reading day length sequence ");
			daily_clim[0].dayl = construct_clim_sequence(
				(char *)strcat(file_name,".dayl"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"daytime_rain_duration") == 0 ){
			strcpy(file_name, file_prefix);
			printf("\n Reading rain duration sequence");
			daily_clim[0].daytime_rain_duration = construct_clim_sequence(
				(char *)strcat(file_name,".daytime_rain_duration"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"LAI_scalar") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].LAI_scalar = construct_clim_sequence(
				(char *)strcat(file_name,".LAI_scalar"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"Ldown") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].Ldown = construct_clim_sequence(
				(char *)strcat(file_name,".Ldown"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"Kdown_diffuse") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].Kdown_diffuse = construct_clim_sequence(
				(char *)strcat(file_name,".Kdown_diffuse"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"Kdown_direct") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].Kdown_direct = construct_clim_sequence(
				(char *)strcat(file_name,".Kdown_direct"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"PAR_diffuse") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].PAR_diffuse = construct_clim_sequence(
				(char *)strcat(file_name,".PAR_diffuse"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"PAR_direct") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].PAR_direct = construct_clim_sequence(
				(char *)strcat(file_name,".PAR_direct"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"relative_humidity") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].relative_humidity = construct_clim_sequence(
				(char *)strcat(file_name,".relative_humidity"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"tday") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].tday = construct_clim_sequence(
				(char *)strcat(file_name,".tday"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"tnightmax") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].tnightmax = construct_clim_sequence(
				(char *)strcat(file_name,".tnightmax"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"tsoil") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].tsoil = construct_clim_sequence(
				(char *)strcat(file_name,".tsoil"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"CO2") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].CO2 = construct_clim_sequence(
				(char *)strcat(file_name,".CO2"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"vpd") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].vpd = construct_clim_sequence(
				(char *)strcat(file_name,".vpd"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"tavg") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].tavg = construct_clim_sequence(
				(char *)strcat(file_name,".tavg"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"snow") == 0 ) {
			strcpy(file_name, file_prefix);
			daily_clim[0].snow = construct_clim_sequence(
				(char *)strcat(file_name,".snow"),
				start_date,
				duration, clim_repeat_flag);
		}

		else if ( strcmp(sequence_name,"wind") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].wind = construct_clim_sequence(
				(char *)strcat(file_name,".wind"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"wind_direction") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].wind_direction = construct_clim_sequence(
				(char *)strcat(file_name,".wind_direction"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"ndep_NH4") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].ndep_NH4 = construct_clim_sequence(
				(char *)strcat(file_name,".ndep_NH4"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"ndep_NO3") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].ndep_NO3 = construct_clim_sequence(
				(char *)strcat(file_name,".ndep_NO3"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"lapse_rate_tmax") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].lapse_rate_tmax = construct_clim_sequence(
				(char *)strcat(file_name,".lapse_rate_tmax"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"lapse_rate_tmin") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].lapse_rate_tmin = construct_clim_sequence(
				(char *)strcat(file_name,".lapse_rate_tmin"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"lapse_rate_precip") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].lapse_rate_precip = construct_clim_sequence(
				(char *)strcat(file_name,".lapse_rate_precip"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"tdewpoint") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].tdewpoint = construct_clim_sequence(
				(char *)strcat(file_name,".tdewpoint"),
				start_date,
				duration, clim_repeat_flag);
		}
		else if ( strcmp(sequence_name,"atm_trans") == 0 ){
			strcpy(file_name, file_prefix);
			daily_clim[0].atm_trans = construct_clim_sequence(
				(char *)strcat(file_name,".atm_trans"),
				start_date,
				duration, clim_repeat_flag);
		}
		
		
		else  fprintf(stderr,
			"WARNING -  clim sequence %s not found.\n",sequence_name);
	} /*end for*/
	return(daily_clim);
} /*end construct_daily_clim*/
