/*--------------------------------------------------------------*/
/* 																*/
/*					construct_base_station	 					*/
/*																*/
/*	construct_base_station.c - makes a base station object 		*/
/*																*/
/*	NAME														*/
/*	construct_base_station.c - makes base station object 		*/
/*																*/
/*	SYNOPSIS													*/
/*	construct_base_station( 									*/
/*							 base_station_file_name,			*/
/*							 start_date,						*/
/*							 duration);							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine makes one base station object.					*/
/*	A base station object holds temporal sequences of driving	*/
/*	variables that different simulation units require.			*/
/*	Base stations are technically located at a point in space	*/
/*	however it is possible for more than on simulation unit		*/
/*	to link to the base station.								*/
/*																*/
/*	The base station object holds header data and (by default)	*/
/*	yearly, monthly, daily, and hourly list of clim objects		*/
/*	which begin empty (NULL pointers).							*/
/*																*/
/*	Each time step has a predetermined set of critical and		*/
/*	non-critical sequences.  The critical sequences are			*/
/*	driving variables that MUST be present for the model to 	*/
/*	execute.  References to these sequences are typically 		*/
/*	hard coded into process algorithms.  Non-critical 			*/
/*	sequences are driving variables which are used if 			*/
/*	provided but are not essential to the simulation.  			*/
/*																*/
/*	File prefixes provided in the base station file identify	*/
/* 	critical sequences.  Hard coded routines will attempt to	*/
/*	open the file given by the file prefix followed by a "."	*/
/*	followed by a suffix identifying the sequence name.  		*/
/*	By convention, the parameter name in the "rhessys.h" file	*/
/*	corresponding to the sequence is used as the suffix.		*/
/*																*/
/*	The number of non-critical sequences for a specific time	*/
/*	step is read in.  Then each of the suffices for the 		*/
/*	non-critical sequences provided in the base station file	*/
/*	are read in one at a time.  The suffix is compared to a		*/
/*	list of valid non-critical sequences for the time step.		*/
/*	If it exists the non-critical sequence object will be		*/
/*	constructed ; if it does not exists a warning will be		*/
/*	reported.													*/
/*																*/
/*	The procedure for constructing critical and non-critical	*/
/*	sequences is repeated for all of the time steps.			*/
/*									*/
/*	Patch type base stations however are special and do not 	*/
/*	need to contain critical sequences but rather contain 		*/
/*	only dated sequences that are associated with 			*/
/*	temporal inputs such as irrigation and fertilizers		*/
/* 	Patch type base stations are noted by having NULL climate inputs */
/*																*/
/*	NOTE:  MISSING DATA IN ANY CLIM SEQUENCE SHOULD BE FLAGGED	*/
/*			AS -999.0 FOR FLOAT SEQUENCES AND -999 FOR INTEGER	*/
/*			SEQUENCES.  THE CODE WILL HANDLE MISSING DATA IN 	*/
/*			A DOCUMENTED FASHION AFTER ALL OF THE BASE STATIONS	*/
/*			HAVE BEEN CONSTRUCTED.  THIS WILL ALLOW FOR 		*/
/*			REPLACEMENT DATA FROM NEIGHBOURING BASE STATIONS.	*/
/*			IN ADDITION, SOME MISSING DATA MAY BE SYNTHESIZED	*/
/*			USING A CLIMATE POSTPROCESSOR (E.G. MTCLIM).		*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	This code only sets up the creation of the base station.	*/
/*	Construting the climate sequence objects is done by			*/
/*	calls to other code.										*/
/*																*/
/*	A base station file is laid out as follows:					*/
/*																*/
/*	<start of file>												*/
/*	ID															*/
/*	x location										meters		*/
/*	y location										meters		*/
/*	z location										meters		*/
/*	isohyet											mm			*/
/*	base_station_effective_lai						m^2/m^2		*/
/*	critical_annual_file_prefix									*/
/*	num_annual_file_non_critical_sequences (n)					*/
/*	name of annual non critical sequnce file 1					*/
/*	name of annual non critical sequnce file 2					*/
/*	name of annual non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of annual non critical sequnce file n					*/
/*	critical_monthly_file_prefix								*/
/*	num_monthly_file_non_critical_sequences (n)					*/
/*	name of monthly non critical sequnce file 1					*/
/*	name of monthly non critical sequnce file 2					*/
/*	name of monthly non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of mnothly non critical sequnce file n					*/
/*	critical_daily_sequence_file_prefix							*/
/*	num_daily_file_non_critical_sequences (n)					*/
/*	name of daily non critical sequnce file 1					*/
/*	name of daily non critical sequnce file 2					*/
/*	name of daily non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of daily non critical sequnce file n					*/
/*	critical_hourly_file_prefix									*/
/*	num_hourly_file_non_critical_sequences (n)					*/
/*	name of hourly non critical sequnce file 1					*/
/*	name of hourly non critical sequnce file 2					*/
/*	name of hourly non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of hourly non critical sequnce file n					*/
/*																*/	
/*	At the moment the code requires all of these entries for	*/
/*	all time steps.  The data file may have to have dummy		*/
/*	files for time steps for which there is no data.			*/
/*																*/
/*	The code which constructs clim objects ensures that they	*/
/*	extend from the start of the world to the end of the world	*/
/*	so there is no explicit date index for climate data in the	*/
/*	objects.  Each clim object consists of a set of pointers	*/
/*	to critical and non-critical climate sequences.  The 		*/
/*	critical sequences are always allocated while the non- 		*/
/*	critical sequences are only allocated if present.  In this	*/
/*	manner it is not neccessary to pre-allocate RAM for			*/
/*	parameters which may not be provided.   					*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*	July 28, 1997 C.Tague					*/
/*	- base station isohyet no longer used - pcp lapse rate	*/
/*	   for each zone is now used				*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

struct	base_station_object *construct_base_station(
													char	*base_station_filename,
													struct	date start_date,
													struct	date duration, 
													int  clim_repeat_flag)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	struct	yearly_clim_object	*construct_yearly_clim(
		FILE	*,
		char	*,
		struct date,
		long,int);
	
	struct	monthly_clim_object	*construct_monthly_clim(
		FILE	*,
		char	*,
		struct date,
		long,int);
	
	struct	daily_clim_object	*construct_daily_clim(
		FILE	*,
		char	*,
		struct date,
		long,int);
	
	struct	hourly_clim_object	*construct_hourly_clim(
		FILE	*,
		char	*,
		struct date,
		long);
	
	struct	dated_input_object	*construct_dated_input(
		FILE	*,
		char	*,
		struct date);

	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	char	clim_object_file_prefix[256];
	char  record[MAXSTR];
	struct	base_station_object	*base_station;
	
	/*--------------------------------------------------------------*/	
	/*	Allocate a structure for the base station object.			*/
	/*--------------------------------------------------------------*/
	base_station = (struct base_station_object *)
		alloc(1 * sizeof(struct base_station_object ),
		"base_station","construct_base_station");
	
	/*--------------------------------------------------------------*/
	/*	Try to open the base station file.							*/
	/*--------------------------------------------------------------*/
	if ( (base_station[0].base_station_file =
		fopen( base_station_filename, "r")) == NULL ){
		fprintf(stderr,
			"FATAL ERROR:in construct_base_stations unable to open base_station file %s\n",
			base_station_filename);
		exit(EXIT_FAILURE);
	} /*end if*/
	
	/*--------------------------------------------------------------*/
	/*	read the header of this base station file.					*/
	/*--------------------------------------------------------------*/
	fscanf(base_station[0].base_station_file, "%d",&(base_station[0].ID));
	read_record(base_station[0].base_station_file, record);
	printf("\n Reading Base station %d", base_station[0].ID);
    fscanf(base_station[0].base_station_file, "%lf",&(base_station[0].proj_x));
	read_record(base_station[0].base_station_file, record);
    fscanf(base_station[0].base_station_file, "%lf",&(base_station[0].proj_y));
	read_record(base_station[0].base_station_file, record);
	fscanf(base_station[0].base_station_file,	"%lf",&(base_station[0].z));
	read_record(base_station[0].base_station_file, record);
	fscanf(base_station[0].base_station_file, "%lf",
		&(base_station[0].effective_lai));
	read_record(base_station[0].base_station_file, record);
	fscanf(base_station[0].base_station_file, "%lf",
		&(base_station[0].screen_height));
	read_record(base_station[0].base_station_file, record);
	/*--------------------------------------------------------------*/
	/*	read in the name of the annual clim object prefix.			*/
	/*--------------------------------------------------------------*/
	fscanf(base_station[0].base_station_file, "%s",clim_object_file_prefix);
	read_record(base_station[0].base_station_file, record);
	/*--------------------------------------------------------------*/
	/*	create the annual climate object for this base station		*/
	/*--------------------------------------------------------------*/
	if (strcmp(clim_object_file_prefix,"NULL") != 0) {
	base_station[0].yearly_clim = construct_yearly_clim(
		base_station[0].base_station_file,
		clim_object_file_prefix,
		start_date,
		duration.year, clim_repeat_flag );
	}
	
	/*--------------------------------------------------------------*/
	/*	read in the name of the monthly clim object prefix.			*/
	/*--------------------------------------------------------------*/
	fscanf(base_station[0].base_station_file, "%s",clim_object_file_prefix);
	read_record(base_station[0].base_station_file, record);
	/*--------------------------------------------------------------*/
	/*	create the monthly climate object for this base station		*/
	/*--------------------------------------------------------------*/
	if (strcmp(clim_object_file_prefix,"NULL") != 0) {
	base_station[0].monthly_clim = construct_monthly_clim(
		base_station[0].base_station_file,
		clim_object_file_prefix,
		start_date,
		duration.month, clim_repeat_flag );
	}
	
	/*--------------------------------------------------------------*/
	/*	read in the name of the daily clim object prefix.			*/
	/*--------------------------------------------------------------*/
	fscanf(base_station[0].base_station_file, "%s",clim_object_file_prefix);
	read_record(base_station[0].base_station_file, record);
	/*--------------------------------------------------------------*/
	/*	create the daily climate object for this base station		*/
	/* 	only do this if not a patch type base station i.e		*/
	/*	prefix is not NULL					*/
	/*--------------------------------------------------------------*/
	if (strcmp(clim_object_file_prefix,"NULL") != 0) {
		base_station[0].daily_clim = construct_daily_clim(
			base_station[0].base_station_file,
			clim_object_file_prefix,
			start_date,
			duration.day, clim_repeat_flag);
	}
	/*--------------------------------------------------------------*/
	/*	read in the name of the hourly clim object prefix.			*/
	/*--------------------------------------------------------------*/
	fscanf(base_station[0].base_station_file, "%s",clim_object_file_prefix);
	printf("\n Reading from %d %s   ", base_station[0].ID, clim_object_file_prefix);
	read_record(base_station[0].base_station_file, record);
	/*--------------------------------------------------------------*/
	/*	create the hourly climate object for this base station		*/
	/*--------------------------------------------------------------*/
	if (strcmp(clim_object_file_prefix,"NULL") != 0) {
	base_station[0].hourly_clim = construct_hourly_clim(
		base_station[0].base_station_file,
		clim_object_file_prefix,
		start_date,
		duration.hour);
	}
	/*--------------------------------------------------------------*/
	/*	now check to see if there are additional sequences to be read */
	/*--------------------------------------------------------------*/
	if (fscanf(base_station[0].base_station_file, "%s",clim_object_file_prefix) != EOF) {
		printf("\n Now from %d %s   ", base_station[0].ID, clim_object_file_prefix);
		read_record(base_station[0].base_station_file, record);
		base_station[0].dated_input = construct_dated_input(
			base_station[0].base_station_file,
			clim_object_file_prefix,
			start_date);
			}

	/*--------------------------------------------------------------*/
	/*		close this base station file.							*/
	/*--------------------------------------------------------------*/
	fclose( base_station[0].base_station_file );
	return(base_station);
} /*end construct_base_stations*/
