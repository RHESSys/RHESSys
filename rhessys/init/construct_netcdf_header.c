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

#ifdef LIU_NETCDF_READER
/*160419LML get the station numbers from station file                         */
int get_netcdf_station_number(char *base_station_filename)
{
    FILE*	base_station_file;
    char	first[MAXSTR];
    char	second[MAXSTR];
    char	buffer[MAXSTR*1000];
    int account = 0;
    if ( (base_station_file = fopen(base_station_filename, "r")) == NULL ){
        fprintf(stderr,
                "FATAL ERROR:in get_netcdf_station_number unable to open base_station file %s\n",
                base_station_file);
        exit(0);
    }
    fseek(base_station_file,0,SEEK_SET);
    while (fgets(buffer, sizeof(buffer), base_station_file) != NULL) {
        if (buffer != NULL) {
            sscanf(buffer, "%s %s", first, second);
            if (strcmp(second, "grid_cells") == 0) {
                account = atoi(first);
                break;
            }
        }
    }
    fclose(base_station_file);
    return account;
}
#endif
struct base_station_ncheader_object *construct_netcdf_header (	
							 struct	world_object	*world,
							 char *base_station_filename)

{
	void	*alloc( 	size_t, char *, char *);
	
    //160419LML struct	base_station_object** base_stations;
	
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
    #ifndef LIU_NETCDF_READER
	world[0].num_base_stations = 0;
    #endif
	base_station_ncheader[0].lastID = 0;
	base_station_ncheader[0].elevflag = 0;
    #ifdef LIU_NETCDF_READER
    for (int i = 0; i < world[0].num_base_stations; i++) {
        struct  base_station_object *basestation = world[0].base_stations[i];
        basestation[0].ID               = -9999;
        basestation[0].effective_lai    = -9999;
        basestation[0].screen_height    = -9999;
        basestation[0].x                = -9999;
        basestation[0].y                = -9999;
        basestation[0].lat              = -9999;
        basestation[0].lon              = -9999;
        //basestation[0].has_constructed  = 0;
    }
    #endif
	 
	fseek(base_station_file,0,SEEK_SET);	
	baseid = -1;
	while (fgets(buffer, sizeof(buffer), base_station_file) != NULL) {
		if (buffer != NULL) {
			sscanf(buffer, "%s %s", first, second);
            if(strcmp(second,"location_searching_distance") == 0){
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
            } else if(strcmp(second,"netcdf_var_tmax") == 0){
                strcpy(base_station_ncheader[0].netcdf_tmax_varname,first);
            } else if(strcmp(second,"netcdf_tmin_filename") == 0){
                strcpy(base_station_ncheader[0].netcdf_tmin_filename,first);
            } else if(strcmp(second,"netcdf_var_tmin") == 0){
                strcpy(base_station_ncheader[0].netcdf_tmin_varname,first);
            } else if(strcmp(second,"netcdf_rain_filename") == 0){
                strcpy(base_station_ncheader[0].netcdf_rain_filename,first);
            } else if(strcmp(second,"netcdf_var_rain") == 0){
                strcpy(base_station_ncheader[0].netcdf_rain_varname,first);
            } else if(strcmp(second,"netcdf_elev_filename") == 0){
                strcpy(base_station_ncheader[0].netcdf_elev_filename,first);
            } else if(strcmp(second,"netcdf_var_elev") == 0){
                strcpy(base_station_ncheader[0].netcdf_elev_varname,first);
                base_station_ncheader[0].elevflag = 1;
            }
            #ifdef LIU_NETCDF_READER
            else if (strcmp(second, "base_station_id") == 0) {
                baseid++;
                world[0].base_stations[baseid][0].ID = atoi(first);
            } else if (strcmp(second, "x_coordinate") == 0) {
                world[0].base_stations[baseid][0].x = atof(first);
            } else if (strcmp(second, "y_coordinate") == 0) {
                world[0].base_stations[baseid][0].y = atof(first);
            } else if (strcmp(second, "z_coordinate") == 0) {
                world[0].base_stations[baseid][0].z = atof(first);
            } else if (strcmp(second, "lon_coordinate") == 0) {
                world[0].base_stations[baseid][0].lon = atof(first);
            } else if (strcmp(second, "lat_coordinate") == 0) {
                world[0].base_stations[baseid][0].lat = atof(first);
            }
            #endif
            else if (strcmp(second, "effective_lai") == 0) {
                #ifdef LIU_NETCDF_READER
                world[0].base_stations[baseid][0].effective_lai = atof(first);
                #else
                base_station_ncheader[0].effective_lai = atof(first);
                #endif
            }
            else if (strcmp(second, "screen_height") == 0) {
                #ifdef LIU_NETCDF_READER
                world[0].base_stations[baseid][0].screen_height = atof(first);
                #else
                base_station_ncheader[0].screen_height = atof(first);
                #endif
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
    printf("finish reading base info\n");
    //printf("\nFinished construct netcdf header: lastID=%d lai=%lf ht=%lf sdist=%lf yr=%d day=%d lpyr=%d pmult=%lf",
    printf("\nFinished construct netcdf file:%s: lastID=%d\tsdist=%lf\tyr=%d\tday=%d\tlpyr=%d\tpmult=%lf\n",
           base_station_filename,
		   base_station_ncheader[0].lastID,
           //base_station_ncheader[0].effective_lai,
           //base_station_ncheader[0].screen_height,
		   base_station_ncheader[0].sdist,
		   base_station_ncheader[0].year_start,
		   base_station_ncheader[0].day_offset,
		   base_station_ncheader[0].leap_year,
           base_station_ncheader[0].precip_mult);
	
	return(base_station_ncheader);
}

