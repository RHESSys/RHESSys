/*--------------------------------------------------------------*/
/*	construct_netcdf_header.c - creates a netcdf climate object header	*/
/*	SYNOPSIS   */
/*	struct base_station_object **construct_netcdf_header(*/
/*		world);	*/
/*	OPTIONS	*/
/*	DESCRIPTION	*/
/*	PROGRAMMER NOTES*/
/***
	Read base station file and create netcdf 
	object header with common attributes.
	AD, 2014
***/



/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"


struct base_station_ncheader_object *construct_netcdf_header (	
							 struct	world_object	*world,
							 char *base_station_filename)

{
	void	*alloc( 	size_t, char *, char *);
	
	struct	base_station_object** base_stations;
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/

	int j;
	
	//struct	daily_optional_clim_sequence_flags	daily_flags;
	struct base_station_ncheader_object *base_station_ncheader;
	
	char	first[MAXSTR];		//I use first & second for the while loop that reads the first base station file
	char	second[MAXSTR];	
	char	buffer[MAXSTR*1000];
	
	FILE*	base_station_file;
	
	int baseid;


	setvbuf(stdout,NULL,_IONBF,0);
	/* allocate daily_optional_clim_sequence_flags struct and make sure set to 0 */
	//memset(&daily_flags, 0, sizeof(struct daily_optional_clim_sequence_flags));
	
	base_station_ncheader = (struct base_station_ncheader_object *)	alloc(sizeof(struct base_station_ncheader_object),"base_station_ncheader","construct_netcdf_header");

	/*--------------------------------------------------------------*/
	/*	Try to open and read the base station file.					*/
	/*--------------------------------------------------------------*/
	if ( (base_station_file = fopen(base_station_filename, "r")) == NULL ){
		fprintf(stderr,
				"FATAL ERROR:in construct_netcdf_grid unable to open base_station file %s\n",
				base_station_file);
		exit(0);
	} /*end if*/
	
	world[0].num_base_stations = 0;
	base_station_ncheader[0].lastID = 0;
	base_station_ncheader[0].elevflag = 0;
	 
	fseek(base_station_file,0,SEEK_SET);	
	baseid = -1;
	/* T.N, Oct 2015: include wind data */
	while (fgets(buffer, sizeof(buffer), base_station_file) != NULL) {
		if (buffer != NULL) {
			sscanf(buffer, "%s %s", first, second);
			if (strcmp(second, "effective_lai") == 0) {
				base_station_ncheader[0].effective_lai = atof(first);
			} else if (strcmp(second, "screen_height") == 0) {
				base_station_ncheader[0].screen_height = atof(first);
			} else if(strcmp(second,"location_searching_distance") == 0){
				base_station_ncheader[0].sdist = atof(first);
			} else if (strcmp(second, "year_start_index") == 0){
				base_station_ncheader[0].year_start = atoi(first);
			} else if (strcmp(second, "day_offset") == 0){
				base_station_ncheader[0].day_offset = atoi(first);
			} else if (strcmp(second, "leap_year_include") == 0){
				base_station_ncheader[0].leap_year = atoi(first);
			} else if (strcmp(second, "precip_multiplier") == 0){
				base_station_ncheader[0].precip_mult = atof(first);
			} else if(strcmp(second,"netcdf_var_x") == 0){
				strcpy(base_station_ncheader[0].netcdf_x_varname,first);
			} else if(strcmp(second,"netcdf_var_y") == 0){
				strcpy(base_station_ncheader[0].netcdf_y_varname,first);
			} else if(strcmp(second,"netcdf_tmax_filename") == 0){
				strcpy(base_station_ncheader[0].netcdf_tmax_filename,first);
			} else if(strcmp(second,"netcdf_tmin_filename") == 0){
				strcpy(base_station_ncheader[0].netcdf_tmin_filename,first);
			} else if(strcmp(second,"netcdf_rain_filename") == 0){
				strcpy(base_station_ncheader[0].netcdf_rain_filename,first);
			} else if(strcmp(second,"netcdf_wind_filename") == 0){
				strcpy(base_station_ncheader[0].netcdf_wind_filename,first);
			} else if(strcmp(second,"netcdf_elev_filename") == 0){
				strcpy(base_station_ncheader[0].netcdf_elev_filename,first);
			} else if(strcmp(second,"netcdf_var_tmax") == 0){
				strcpy(base_station_ncheader[0].netcdf_tmax_varname,first);
			} else if(strcmp(second,"netcdf_var_tmin") == 0){
				strcpy(base_station_ncheader[0].netcdf_tmin_varname,first);
			} else if(strcmp(second,"netcdf_var_rain") == 0){
				strcpy(base_station_ncheader[0].netcdf_rain_varname,first);
			} else if(strcmp(second,"netcdf_var_wind") == 0){
				strcpy(base_station_ncheader[0].netcdf_wind_varname,first);				
			} else if(strcmp(second,"netcdf_var_elev") == 0){
				strcpy(base_station_ncheader[0].netcdf_elev_varname,first);
				base_station_ncheader[0].elevflag = 1;
			}
			// NEED TO UPDATE THIS SECTION
			// Checking for optional climate sequences, store those found in the
			// optional_flag structs for sending to the appropriate create
			// clim sequence functions.
			
			/*} else if (strcmp(second, "number_non_critical_daily_sequences") == 0) {
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
				}*/
		}
		}//end_read_basestationfile
		
	fclose(base_station_file);	
	/*printf("finish reading base info\n");*/
	/*printf("\nFinished construct netcdf header: lastID=%d lai=%lf ht=%lf sdist=%lf yr=%d day=%d lpyr=%d pmult=%lf \n",
		   base_station_ncheader[0].lastID,
		   base_station_ncheader[0].effective_lai,
		   base_station_ncheader[0].screen_height,
		   base_station_ncheader[0].sdist,
		   base_station_ncheader[0].year_start,
		   base_station_ncheader[0].day_offset,
		   base_station_ncheader[0].leap_year,
		   base_station_ncheader[0].precip_mult);
	*/
	return(base_station_ncheader);
}
