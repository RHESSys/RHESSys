/*--------------------------------------------------------------*/
/* 																*/
/*					construct_ascii_grid						*/
/*																*/
/*	construct_ascii_grid.c - creates a zone object				*/
/*																*/
/*	NAME														*/
/*	construct_ascii_grid.c - creates a zone object				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct ascii_grid *construct_ascii_grid(					*/
/*					base_station_file_name,						*/
/*					start_date,									*/
/*					duration									*/
/*					column_index);								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	Assumes all climate files start on the same date as tmax	*/
/*	and are at least equal to, if not greater, than tmax's		*/
/*	duration													*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

struct base_station_object **construct_ascii_grid (
								char		*base_station_filename,
								struct		date start_date,
								struct		date duration)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


	struct daily_optional_clim_sequence_flags
	{
		int atm_trans;
		int CO2;
		int cloud_fration;
		int cloud_opacity;
		int dayl;
		int Delta_T;
		int dewpoint;
		int base_station_effective_lai;
		int Kdown_diffuse;
		int Kdown_direct;
		int LAI_scalar;
		int Ldown;
		int PAR_diffuse;
		int PAR_direct;
		int daytime_rain_duration;
		int relative_humidity;
		int snow;
		int tdewpoint;
		int tday;
		int tnight;
		int tnightmax;
		int tavg;
		int tsoil;
		int vpd;
		int wind;
		int ndep_NO3;
		int ndep_NH4;
		int lapse_rate_tmax;
		int lapse_rate_tmin;
	};
	
	void	*alloc( 	size_t, char *, char *);
	
	struct	base_station_object** base_stations;
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int 	num_base_stations;
	int		i;
	int		j;

	long	julday();
	long	first_date_julian;
	long	start_date_julian;
	long	offset;	

		
	struct	daily_optional_clim_sequence_flags	daily_flags;
	struct	date first_date;
	
	char	first[MAXSTR];		//I use first & second for the while loop that reads the first base station file
	char	second[MAXSTR];	
	char	eff_lai[MAXSTR];
	char	screen_height[MAXSTR];
	char	daily_clim_prefix[MAXSTR];
	char	old_prefix[MAXSTR];
	char	buffer[MAXSTR*100];
	char	elevs[MAXSTR*100];
	char	ids[MAXSTR*100];
	char	buffertmax[MAXSTR*100];
	char	buffertmin[MAXSTR*100];
	char	bufferrain[MAXSTR*100];
	char	bufferalt1[MAXSTR*100];
	char	*tokc;
	char	*last;
	char	*laste;
	char	*lasttmax;
	char	*lasttmin;
	char	*lastrain;
	char	*lastalt1;
	
	FILE*	base_station_file;
	FILE*   tmax_file;
	FILE*   tmin_file;
	FILE*   rain_file;	
	FILE*   alt1_clim_file;	
	

	/* allocate daily_optional_clim_sequence_flags struct and make sure set to 0 */
	memset(&daily_flags, 0, sizeof(struct daily_optional_clim_sequence_flags));
	
	
	/*--------------------------------------------------------------*/
	/*	Try to open and read the base station file.					*/
	/*--------------------------------------------------------------*/
	if ( (base_station_file = fopen(base_station_filename, "r")) == NULL ){
		fprintf(stderr,
				"FATAL ERROR:in construct_ascii_grid unable to open base_station file %s\n",
				base_station_filename);
		exit(EXIT_FAILURE);
	} /*end if*/	
	
	
	/*--------------------------------------------------------------*/
	/*	Below reads the base station file like construct_ascii_grid.c, 
	/*--------------------------------------------------------------*/
	
	while (fgets(buffer, sizeof(buffer), base_station_file) != NULL) {
		if (buffer != NULL) {
			sscanf(buffer, "%s %s", first, second);
			
			if (strcmp(second, "grid_cells") == 0) {
				num_base_stations = atoi(first);
				printf("\nYou have %d base stations \n", num_base_stations);
			}

			if (strcmp(second, "daily_climate_prefix") == 0) {
				sscanf(first, "%s", daily_clim_prefix);
				printf("Daily clim is %s \n", daily_clim_prefix);
				strcpy(old_prefix, daily_clim_prefix);	//copy into prefix holder for using with strcat to open clim files
			} else if (strcmp(second, "effective_lai") == 0) {
					sscanf(first, "%s", eff_lai);		
			} else if (strcmp(second, "screen_height") == 0) {
				sscanf(first, "%s", screen_height);
				// Checking for optional climate sequences, store those found in the
				// optional_flag structs for sending to the appropriate create
				// clim sequence functions.
			} else if (strcmp(second, "number_non_critical_daily_sequences") == 0) {
				int num_daily_optional = strtod(first, NULL);
				for ( j=0; j < num_daily_optional; ++j ) {
					fgets(buffer, sizeof(buffer), base_station_file);
					if (buffer != NULL) {
						if (strcmp(buffer, "atm_trans") == 0 ) {
							daily_flags.atm_trans = 1;
						} else if (strcmp(buffer, "CO2") == 0) {
							daily_flags.CO2 = 1;
						} 
						else if (strcmp(buffer, "daytime_rain_duration") ) {
							daily_flags.daytime_rain_duration = 1;
						} 
					}  
				}
			} 
		}
	}

	
	/*-----------------------------------------------------------------------*/	
	/*	Allocate a structure for a base station object num_base_stations big.*/
	/*-----------------------------------------------------------------------*/
	base_stations = (struct base_station_object **)	alloc(num_base_stations * sizeof(struct base_station_object *), 
											"base_station","construct_ascii_grid");	
		   
	/* Open the tmax clim file, call it tmax_file. Use to assign ID, elevs as allocating base_stations[i] */
	if ( (tmax_file = fopen((char*)strcat(daily_clim_prefix, ".tmax"), "r")) == NULL ) {
		fprintf(stderr,
				"FATAL ERROR: in construct_ascii_grid unable to open tmax file %s", 
				(char*)strcat(daily_clim_prefix, ".tmax"));
		exit(EXIT_FAILURE);
	}
	strcpy(daily_clim_prefix, old_prefix);					// Restore the original daily_clim prefix
	fgets(buffer, sizeof(buffer), tmax_file);
	fgets(buffer, sizeof(buffer), tmax_file);		
	fgets(ids, sizeof(ids), tmax_file);
	fgets(elevs, sizeof(elevs), tmax_file);
	
	/*--------------------------------------------------------------*/
	/* Allocate daily clim structures and clim seqs					*/
	/*--------------------------------------------------------------*/	

	for ( i = 0; i < num_base_stations; i++) {
		/*Allocate base_stations */
		base_stations[i] = (struct base_station_object*) alloc(1 * sizeof(struct base_station_object),
											"base_station", "construct_ascii_grid");
		/* Populate base_stations[i] */
		base_stations[i][0].effective_lai = strtod(eff_lai,NULL);
		base_stations[i][0].screen_height = strtod(screen_height,NULL);
		if (i==0) {
		tokc = strtok_r(ids," ",&last);
		sscanf(tokc, "%d", &(base_stations[i][0].ID));		// reading 3rd line (should be IDs) at ith integer 
		tokc = strtok_r(elevs," ",&laste);
		sscanf(tokc, "%lf", &(base_stations[i][0].z));		// reading 4th line (should be elevs) at ith integer 
		}
		else {
			tokc = strtok_r(NULL," ",&last);
			sscanf(tokc, "%d", &(base_stations[i][0].ID)); //read 3rd line (should be IDs) 
			tokc = strtok_r(NULL," ",&laste);
			sscanf(tokc, "%lf", &(base_stations[i][0].z)); //read 4th line (should be elevs) 
		}
		
		
		/* For each daily clim structure allocate clim seqs for all required & optional clims */
		base_stations[i][0].daily_clim = (struct daily_clim_object *)
			alloc(1*sizeof(struct daily_clim_object),"daily_clim","construct_daily_clim" );
		//duration.day is a long that was passed into construct_ascii as a date struct
		base_stations[i][0].daily_clim[0].tmax = (double *) alloc(duration.day * sizeof(double),"tmax", "construct_ascii_grid");
		base_stations[i][0].daily_clim[0].tmin = (double *) alloc(duration.day * sizeof(double),"tmin", "construct_ascii_grid");
		base_stations[i][0].daily_clim[0].rain = (double *) alloc(duration.day * sizeof(double),"rain", "construct_ascii_grid");
		/*--------------------------------------------------------------*/
		/*	initialize the rest of the clim sequences as null	*/
		/*--------------------------------------------------------------*/
		base_stations[i][0].daily_clim[0].atm_trans = NULL;
		base_stations[i][0].daily_clim[0].CO2 = NULL;
		base_stations[i][0].daily_clim[0].cloud_fraction = NULL;
		base_stations[i][0].daily_clim[0].cloud_opacity = NULL;
		base_stations[i][0].daily_clim[0].dayl = NULL;
		base_stations[i][0].daily_clim[0].Delta_T = NULL;
		base_stations[i][0].daily_clim[0].dewpoint = NULL;
		base_stations[i][0].daily_clim[0].base_station_effective_lai = NULL;
		base_stations[i][0].daily_clim[0].Kdown_diffuse = NULL;
		base_stations[i][0].daily_clim[0].Kdown_direct = NULL;
		base_stations[i][0].daily_clim[0].LAI_scalar = NULL;
		base_stations[i][0].daily_clim[0].Ldown = NULL;
		base_stations[i][0].daily_clim[0].PAR_diffuse = NULL;
		base_stations[i][0].daily_clim[0].PAR_direct = NULL;
		base_stations[i][0].daily_clim[0].daytime_rain_duration = NULL; 
		base_stations[i][0].daily_clim[0].relative_humidity = NULL;
		base_stations[i][0].daily_clim[0].snow = NULL;
		base_stations[i][0].daily_clim[0].tdewpoint = NULL;
		base_stations[i][0].daily_clim[0].tday = NULL;
		base_stations[i][0].daily_clim[0].tnight = NULL;
		base_stations[i][0].daily_clim[0].tnightmax = NULL;
		base_stations[i][0].daily_clim[0].tavg = NULL;
		base_stations[i][0].daily_clim[0].tsoil = NULL;
		base_stations[i][0].daily_clim[0].vpd = NULL;
		base_stations[i][0].daily_clim[0].wind = NULL;
		base_stations[i][0].daily_clim[0].ndep_NO3 = NULL;
		base_stations[i][0].daily_clim[0].ndep_NH4 = NULL;
		
		/*Check if any flags are set in the optional clim sequence struct*/
		if ( daily_flags.daytime_rain_duration == 1 ) {
			   base_stations[i][0].daily_clim[0].daytime_rain_duration = (double *) 
			alloc(duration.day * sizeof(double),"day_rain_dur", "construct_ascii_grid");

		}
		/*--------------------------------------------------------------*/
		/*	Allocate the yearly clim object.								*/
		/*--------------------------------------------------------------*/
		base_stations[i][0].yearly_clim = (struct yearly_clim_object *)
		alloc(1*sizeof(struct yearly_clim_object), "yearly_clim", "construct_ascii_grid" );
		/*	Initialize non-critical sequences							*/
		base_stations[i][0].yearly_clim[0].temp = NULL;		
		/*--------------------------------------------------------------*/
		/*	Allocate the monthly clim object.							*/	
		/*--------------------------------------------------------------*/
		base_stations[i][0].monthly_clim = (struct monthly_clim_object *)
		alloc(1*sizeof(struct monthly_clim_object), "monthly_clim", "construct_ascii_grid" );
		/*	Initialize non-critical sequences							*/
		base_stations[i][0].monthly_clim[0].temp = NULL;
		/*--------------------------------------------------------------*/
		/*	Allocate the hourly clim object.							*/
		/*--------------------------------------------------------------*/
		base_stations[i][0].hourly_clim = (struct hourly_clim_object *)
		alloc(1*sizeof(struct hourly_clim_object), "hourly_clim", "construct_ascii_grid" );
		/*	Initialize non - critical sequences.						*/
		base_stations[i][0].hourly_clim[0].rain.inx = -999;
		base_stations[i][0].hourly_clim[0].rain_duration.inx = -999;
	}

	fclose(base_station_file );	
	fclose(tmax_file);
		   
	/*--------------------------------------------------------------*/
	/*	open TMAX sequence file again. closed to get back up to top */
	/*	what follows is the construct_clim_sequence.c portion		*/
	/*--------------------------------------------------------------*/
	if ( (tmax_file = fopen((char*)strcat(daily_clim_prefix, ".tmax"), "r")) == NULL ) 
	{			   fprintf(stderr,
					   "FATAL ERROR: in construct_construct_ascii_grid\n unable to open tmax sequence file\n");
			   fprintf(stderr, "\nFile name %s", daily_clim_prefix);
			   exit(EXIT_FAILURE);
	} /*end if*/
	
	strcpy(daily_clim_prefix, old_prefix);					// Restore the original daily_clim prefix	   
	printf("Opened tmax file %s.tmax\n", daily_clim_prefix);
	
	/*--------------------------------------------------------------*/
	/*	Read in start date in clim file (calendar date to hour res)	*/
	/*--------------------------------------------------------------*/
	fgets(buffer, sizeof(buffer), tmax_file);				/* skip line with #grid_cells */
	fscanf(tmax_file,"%ld %ld %ld %ld",&first_date.year,	/* read date */
				  &first_date.month,&first_date.day,&first_date.hour);
	fgets(buffer, sizeof(buffer), tmax_file);				/* skip carriage return */
	fgets(buffer, sizeof(buffer), tmax_file);				/* skip line with IDs */
	fgets(buffer, sizeof(buffer), tmax_file);				/* skip line with elevs */ //here, reading IDs
	
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
	/*	open remaining clim seqs. a good programmer will generalize	*/
	/*	reading the opt clim seq using the clim_flag struct, like at*/
	/*	the end of construct_daily_clim.c							*/	
	/*	skip 1st 4 lines of clim seq file to beginning of clim data	*/	
	/*--------------------------------------------------------------*/
	/* TMIN */
	if ( (tmin_file = fopen((char*)strcat(daily_clim_prefix, ".tmin"), "r"))  == NULL ){
		fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\nunable to open tmin sequence file\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Opened tmin file %s\n", daily_clim_prefix);
		strcpy(daily_clim_prefix, old_prefix);				// Restore the original daily_clim prefix
	}
	for ( i = 0; i < 4; i++) {								// skip 4 lines: # grid cells, date, ID & elev 
		fgets(buffer, sizeof(buffer), tmin_file);
	}
	/* RAIN */
	if ( (rain_file = fopen((char*)strcat(daily_clim_prefix, ".rain"), "r"))  == NULL ){
		fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\nunable to open rain sequence file\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Opened rain file %s\n", daily_clim_prefix);
		strcpy(daily_clim_prefix, old_prefix);				// Restore the original daily_clim prefix
	}
	for ( i = 0; i < 4; i++) {								// skip 4 lines: # grid cells, date, ID & elev 
		fgets(buffer, sizeof(buffer), rain_file);
	}
	
	/* ALT1 */
	if ( (alt1_clim_file = fopen((char*)strcat(daily_clim_prefix, ".daytime_rain_duration"), "r"))  == NULL ){
		fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\nunable to open alt1 sequence file\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Opened rain duration file %s\n", daily_clim_prefix);
		strcpy(daily_clim_prefix, old_prefix);			// Restore the original daily_clim prefix
	}
	/* skip 4 lines: # grid cells, date, ID & elev */
	for ( i = 0; i < 4; i++) {							
		fgets(buffer, sizeof(buffer), alt1_clim_file);
	}
	
	/*--------------------------------------------------------------*/
	/*	Scan forward in the sequences until the start date.			*/
	/*--------------------------------------------------------------*/
	for ( i = 0 ; i<offset ; i++ ){
		if ( fgets(buffer, sizeof(buffer), tmax_file) == NULL  ) {
			fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\n - tmax start date beyond eof"); 
			exit(EXIT_FAILURE);
		} else if ( fgets(buffer, sizeof(buffer), tmin_file) == NULL  ) {
			fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\n - tmin start date beyond eof"); 
			exit(EXIT_FAILURE);
		} else if ( fgets(buffer, sizeof(buffer), rain_file) == NULL  ) {
			fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\n - rain start date beyond eof"); 
			exit(EXIT_FAILURE);
		} else if ( fgets(buffer, sizeof(buffer), alt1_clim_file) == NULL  ) {
			fprintf(stderr, "FATAL ERROR: in construct_ascii_grid\n - alt1 start date beyond eof"); 
			exit(EXIT_FAILURE);
		}
	}
	
	/*--------------------------------------------------------------*/
	/* Fill daily clim structures and clim seqs						*/
	/*--------------------------------------------------------------*/
	for (j=0; j < duration.day; j++) {
		// printf("Assign daily clim sequence\n");
		/*--------------------------------------------------------------*/
		/*	Assign the climate sequence data.							*/
		/*--------------------------------------------------------------*/
		if (fgets(buffertmax, sizeof(buffertmax), tmax_file)==NULL) {
			fprintf(stderr,"FATAL ERROR: in construct_ascii_grid \n");
			fprintf(stderr,"\n tmax end date beyond end of clim sequence\n");
			exit(EXIT_FAILURE);
		}
		if (fgets(buffertmin, sizeof(buffertmin), tmin_file)==NULL) {
			fprintf(stderr,"FATAL ERROR: in construct_ascii_grid \n");
			fprintf(stderr,"\n tmin end date beyond end of clim sequence\n");
			exit(EXIT_FAILURE);
		}	
		if (fgets(bufferrain, sizeof(bufferrain), rain_file)==NULL) {
			fprintf(stderr,"FATAL ERROR: in construct_ascii_grid \n");
			fprintf(stderr,"\n rain end date beyond end of clim sequence\n");
			exit(EXIT_FAILURE);
		}
		if (fgets(bufferalt1, sizeof(bufferalt1), alt1_clim_file)==NULL) {
			fprintf(stderr,"FATAL ERROR: in construct_ascii_grid \n");
			fprintf(stderr,"\n optional clim end date beyond end of clim sequence\n");
			exit(EXIT_FAILURE);
		}
		for (i=0; i < num_base_stations; i++) {
			
			if (i==0) {
				tokc = strtok_r(buffertmax, " ", &lasttmax);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].tmax[j]));
				tokc = strtok_r(buffertmin, " ", &lasttmin);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].tmin[j]));
				tokc = strtok_r(bufferrain, " ", &lastrain);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].rain[j]));
				tokc = strtok_r(bufferalt1, " ", &lastalt1);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].daytime_rain_duration[j]));
			} else {
				tokc = strtok_r(NULL," ",&lasttmax);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].tmax[j]));
				tokc = strtok_r(NULL," ",&lasttmin);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].tmin[j]));
				tokc = strtok_r(NULL," ",&lastrain);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].rain[j]));
				tokc = strtok_r(NULL," ",&lastalt1);
				sscanf(tokc, "%lf", &(base_stations[i][0].daily_clim[0].daytime_rain_duration[j]));
			}
			

		}

	}
	fclose(tmax_file);
	fclose(tmin_file);
	fclose(rain_file);
	fclose(alt1_clim_file);
	return(base_stations);
	
}

