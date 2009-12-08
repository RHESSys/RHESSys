/*--------------------------------------------------------------*/
/* 																*/
/*					construct_ascii_grid	 					*/
/*																*/
/*	construct_ascii_grid.c - makes a base station object 		*/
/*																*/
/*	NAME														*/
/*	construct_ascii_grid.c - makes base station object 			*/
/*																*/
/*	SYNOPSIS													*/
/*	construct_ascii_grid( 										*/
/*							 base_station_file_name,			*/
/*							 start_date,						*/
/*							 duration							*/
/*							 column_index);						*/
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
/*	only dated sequences that are associated with 				*/
/*	temporal inputs such as irrigation and fertilizers			*/
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
/*	The file format used is base off of the format used in      */
/*  construct_base_station.c, however has been modified to      */
/*  support gridded data.  These changes are added as a new     */
/*  function so that we do not lock ourselvs into the           */
/*  non-gridded format going forward.							*/
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
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

void construct_ascii_grid( char	  *base_station_filename,
      					   struct date start_date,
					       struct date duration,
						   struct base_station_object** base_stations)
{
	// Local functions for file parsing
	struct	yearly_clim_object*	ascii_yearly_clim(
		char*, struct date, long, int);
	struct	monthly_clim_object* ascii_monthly_clim(
		char*, struct date, long, int);
	struct	daily_clim_object* ascii_daily_clim(
		char*, struct date, long, int);
	struct	hourly_clim_object*	ascii_hourly_clim(
		char*, struct date, long, int);

	// Open the base station base file
	FILE* base_station_file;
	if ( (base_station_file = fopen(base_station_filename, "r")) == NULL ) {
		fprintf(stderr,
			"FATAL ERROR:in construct_ascii_grid unable to open base_station file %s\n",
			base_station_filename);
		exit(0);
	}

	// Reread the grid base file to find out how many cells there are
	int 	num_base_stations;
	fscanf(base_station_file, "%d", &num_base_stations);

	// Read in the header from the grid base file
	char	buffer[MAXSTR];
	char	first[MAXSTR];
	char	second[MAXSTR];
	char	yearly_climate_prefix[MAXSTR];
	char	monthly_climate_prefix[MAXSTR];
	char	daily_climate_prefix[MAXSTR];
	char	hourly_climate_prefix[MAXSTR];

	int i;
	for ( i = 0; i < num_base_stations; ++i) {
		base_stations[i] = (struct base_station_object*)
							malloc(1 * sizeof(struct base_station_object) );

		(*base_stations[i]).base_station_file = base_station_file;

		while (fgets(buffer, sizeof(buffer), (*base_stations[i]).base_station_file) != NULL) {
			if (buffer != NULL) {
				sscanf(buffer, "%s %s", first, second);

				if (strcmp(second, "grid_cells") == 0) {
					// This was read by create_world to use as number of base stations,
					// do nothing
				}
				if (strcmp(second, "x_coordinate") == 0) {
					(*base_stations[i]).x = strtod(first, NULL);
				} else if (strcmp(second, "y_coordinate") == 0) {
					(*base_stations[i]).y = strtod(first, NULL);
					sscanf(first, "%g", &(*base_stations[i]).y);
				} else if (strcmp(second, "z_coordinate") == 0) {
					(*base_stations[i]).z = strtod(first, NULL);
					sscanf(first, "%g", &(*base_stations[i]).z);
				} else if (strcmp(second, "effective_lai") == 0) {
					(*base_stations[i]).effective_lai = strtod(first, NULL);
					sscanf(first, "%g", &(*base_stations[i]).effective_lai);
				} else if (strcmp(second, "screen_height") == 0) {
					(*base_stations[i]).screen_height = strtod(first, NULL);
					sscanf(first, "%g", &(*base_stations[i]).screen_height);
				} else if (strcmp(second, "annual_climate_prefix") == 0) {
					sscanf(first, "%s", yearly_climate_prefix);
				} else if (strcmp(second, "monthly_climate_prefix") == 0) {
					sscanf(first, "%s", monthly_climate_prefix);
				} else if (strcmp(second, "daily_climate_prefix") == 0) {
					sscanf(first, "%s", daily_climate_prefix);
				} else if (strcmp(second, "hourly_climate_prefix") == 0) {
					sscanf(first, "%s", hourly_climate_prefix);
				}
			}
		}

		// Get the cell ID. i is iterating across columns, there is no guarentee
		// that each input file will have columns in the same order, so base the
		// column order off of the hourly tmax, since this is required information.
		FILE* tmax_file;
		char  old_prefix[MAXSTR];
		strcpy(old_prefix, daily_climate_prefix);
		if ( (tmax_file = fopen((char*)strcat(daily_climate_prefix, ".tmax"), "r")) == NULL ) {
			fprintf(stderr,
				"FATAL ERROR: in construct_ascii_grid unable to open tmax file %s", 
				(char*)strcat(daily_climate_prefix, ".tmax"));
				exit(0);
		}
		fgets(buffer, sizeof(buffer), tmax_file); 	// Skips the header line 
													// with the date
		fgets(buffer, sizeof(buffer), tmax_file);
		int column;
		char* tok;
		char* last;  // For strtok_r()
		tok = strtok_r(buffer, " ", &last);
		for (column = 0; column < i; ++column) {
			tok = strtok_r(NULL, " ", &last);	
		}
		sscanf(tok, "%d", &(*base_stations[i]).ID);	
		fclose(tmax_file);
		strcpy(daily_climate_prefix, old_prefix); // Restore the original prefix

		printf("Loading grid cell %ld\n", (*base_stations[i]).ID);

		(*base_stations[i]).yearly_clim = ascii_yearly_clim( 
						   yearly_climate_prefix,
						   start_date,
						   duration.year,
						  (*base_stations[i]).ID);

		(*base_stations[i]).monthly_clim = ascii_monthly_clim( 
		                    monthly_climate_prefix,
							start_date,
							duration.month,
						  (*base_stations[i]).ID);

		(*base_stations[i]).daily_clim = ascii_daily_clim(
						  daily_climate_prefix,
						  start_date,
						  duration.day,
						  (*base_stations[i]).ID);

		(*base_stations[i]).hourly_clim = ascii_hourly_clim( 
				  		   hourly_climate_prefix,
						   start_date,
						   duration.hour,
						  (*base_stations[i]).ID);

		// Move the file pointer back to the head of the file to reparse
		fseek(base_station_file, 0, SEEK_SET);
	}

	fclose( base_station_file );
}

struct	yearly_clim_object*	ascii_yearly_clim(
		char* file_prefix,
		struct date start_date, 
		long duration,
		int ID) {

	struct yearly_clim_object	*yearly_clim;
	
	/*--------------------------------------------------------------*/
	/*	Allocate the yearly clim object.								*/
	/*--------------------------------------------------------------*/
	yearly_clim = (struct yearly_clim_object *)
		malloc(1*sizeof(struct yearly_clim_object));
	
	/*--------------------------------------------------------------*/
	/*	Initialize non-critical sequences			*/
	/*--------------------------------------------------------------*/
	yearly_clim[0].temp = NULL;
	
	return yearly_clim;
}

struct	monthly_clim_object* ascii_monthly_clim(
		char* file_prefix, 
		struct date start_date, 
		long duration,
		int ID) {
	
	struct monthly_clim_object	*monthly_clim;
	
	
	/*--------------------------------------------------------------*/
	/*	Allocate the monthly clim object.								*/	
	/*--------------------------------------------------------------*/
	monthly_clim = (struct monthly_clim_object *)
		malloc(1*sizeof(struct monthly_clim_object));
	
	/*--------------------------------------------------------------*/
	/*	Initialize non-critical sequences			*/
	/*--------------------------------------------------------------*/
	monthly_clim[0].temp = NULL;
	
	return monthly_clim;
}

struct	daily_clim_object*	ascii_daily_clim(
		char* file_prefix, 
		struct date start_date, 
		long duration,
		int ID) {

	// Local function declarations
	double* ascii_clim_sequence(char*, struct date, long, int);


	char	file_name[MAXSTR];	
	struct	daily_clim_object	*daily_clim;

	/*--------------------------------------------------------------*/
	/*	Allocate the daily clim object.								*/
	/*--------------------------------------------------------------*/
	daily_clim = (struct daily_clim_object *)
		malloc(1*sizeof(struct daily_clim_object));
	
	/*--------------------------------------------------------------*/
	/*	Attempt to open the daily clim sequence file for each		*/
	/*	critical clim parameter and read them in.					*/
	/*--------------------------------------------------------------*/

	strcpy(file_name, file_prefix);
	daily_clim[0].tmin = ascii_clim_sequence(
		(char *)strcat(file_name,".tmin"),
		start_date,
		duration, 
		ID);
	
	strcpy(file_name, file_prefix);
	daily_clim[0].tmax = ascii_clim_sequence(
		(char *)strcat(file_name,".tmax"),
		start_date,
		duration, 
		ID);

	strcpy(file_name, file_prefix);
	daily_clim[0].rain = ascii_clim_sequence(
		(char *)strcat(file_name,".rain"),
		start_date,
		duration, 
		ID);

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
	daily_clim[0].ndep_NO3 = NULL;
	daily_clim[0].ndep_NH4 = NULL;
	
	return daily_clim;
}

struct	hourly_clim_object*	ascii_hourly_clim( 
		char* file_prefix, 
		struct date start_date, 
		long duration,
		int ID) {
	
	struct hourly_clim_object*	hourly_clim;
	/*--------------------------------------------------------------*/
	/*	Allocate the hourly clim object.								*/
	/*--------------------------------------------------------------*/
	hourly_clim = (struct hourly_clim_object *)
		malloc(1*sizeof(struct hourly_clim_object));
	
	/*-------------------------------------------------------------*/
	/*	Initialize non - critical sequences.			*/
	/*--------------------------------------------------------------*/
	hourly_clim[0].rain.inx = -999;
	hourly_clim[0].rain_duration.inx = -999;
	
	return hourly_clim;
}

double* ascii_clim_sequence(char* file_name, 
					  	    struct date start_date, 
						    long duration, 
						    int ID) {
	/*--------------------------------------------------------------*/
	/*	Allocate the clim sequence.									*/
	/*--------------------------------------------------------------*/
	double* sequence;
	sequence = (double *) malloc(duration*sizeof(double));

	/*--------------------------------------------------------------*/
	/*	Try to open the file containing the clim sequence.			*/
	/*--------------------------------------------------------------*/
	FILE* sequence_file;
	if ( (sequence_file = fopen(file_name, "r") ) == NULL ){
		fprintf(stderr,
			"FATAL ERROR: in construct_clim_sequence\nunable to open sequence file %s\n", file_name);
		fprintf(stderr, "\nFile name %s", file_name);
		exit(0);
	} /*end if*/
	
	/*--------------------------------------------------------------*/
	/*	Read in start date in clim file (calendar date to hour res)	*/
	/*--------------------------------------------------------------*/
	struct date first_date;
	fscanf(sequence_file,"%ld %ld %ld %ld",&first_date.year,
		&first_date.month,&first_date.day,&first_date.hour);

	/*--------------------------------------------------------------*/
	/*	Compute julian date of first date in sequence and 	start	*/
	/*	date of world.												*/
	/*--------------------------------------------------------------*/
	long first_date_julian, start_date_julian;
	first_date_julian = julday(first_date);
	start_date_julian = julday(start_date);

	/*--------------------------------------------------------------*/
	/*	Compute offset to reach start date.							*/
	/*--------------------------------------------------------------*/
	long offset;
	offset = (start_date_julian - first_date_julian);
	
	/*--------------------------------------------------------------*/
	/* 	If the start date is before the first date error.			*/
	/*--------------------------------------------------------------*/
	if ( offset < 0 ){
		fprintf(stderr,
			"FATAL ERROR: start date before first date of a clim sequence.\n");
		exit(0);
	}

	// Read in the row of IDs, until we find ID
	long	column = 0; // The column that we need to extract values from
	long	current_ID; // The ID of the current column for comparison
	char*	last;		// temp variable for strtok_r()
	char	buffer[MAXSTR]; // buffer for reading with fgets()
	char*	tok;	// Holds the results of each strtok_r() call

	// fscanf puts the file pointer to the end of the first line.
	// we do fgets twice to advance to the second line with the 
	// grid cell IDs
	fgets(buffer, sizeof(buffer), sequence_file);
	fgets(buffer, sizeof(buffer), sequence_file);
	tok = strtok_r(buffer, " ", &last);
	current_ID = strtod(tok, NULL);
	
	while (current_ID != ID) {
		tok = strtok_r(NULL, " ", &last);
		sscanf(tok, "%d", &current_ID);
		++column;
	}

	// At this point column should be the column to read data from

	/*--------------------------------------------------------------*/
	/*	Scan forwards in the sequence until the start date.			*/
	/*--------------------------------------------------------------*/
	int i;
	for ( i = 0 ; i<offset ; i++ ){
		if ( fgets(buffer, sizeof(buffer), sequence_file) == EOF  ) {
			fprintf(stderr,
				"FATAL ERROR: in construct_clim_sequence\n - start date beyond eof"); 
			exit(0);
		}
	}

	/*--------------------------------------------------------------*/
	/*	Read in the climate sequence data.							*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<duration ; i++ ){
		if ( fgets(buffer, sizeof(buffer), sequence_file) == EOF  ) {
			fprintf(stderr,"FATAL ERROR: in construct_clim_sequence\n");
			fprintf(stderr," end date beyond end of clim sequence\n");
			exit(0);
		}
		else{
			// Move to the correct column of data before adding value 
			// to sequence
			tok = strtok_r(buffer, " ", &last);
			int j;
			for (j = 0; j < column; ++j) {
				tok = strtok_r(NULL, " ", &last);	
			}

			sequence[i] = strtod(tok, NULL);
		}
	}

	fclose(sequence_file);

	return sequence;
}
