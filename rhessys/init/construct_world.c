/*--------------------------------------------------------------*/
/* 																*/
/*					construct_world								*/
/*																*/
/*	construct_world.c - creates a world object					*/
/*																*/
/*	NAME														*/
/*	construct_world.c - creates a world object					*/
/*																*/
/*	SYNOPSIS													*/
/*	struct world_object *construct_world_line(&command_line)	*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Constructs the world object which consists of:				*/
/*		- the start and end day of the world.					*/
/*		  (for which all climate files must have daily data)	*/
/*		- the number of basins in the world.					*/
/*		- the number and names of default files for				*/
/*			hillslopes, zones, and stratums.					*/
/*		- the number of base_stations in the world.				*/
/*																*/
/*	Below is documentation for ALL input files required with	*/
/*	rhessys3.0.  Child objects of the world make use of some	*/
/*	of the files.  The source code documentation for creation	*/
/*	fof child objects referes to the files described below at	*/
/*	times.  We have placed them all here to make them easy to	*/
/*  find no matter how future interfaces decide to partition	*/
/*	the required input data records into physical files.		*/
/*	At present a world file and a basin file are the only		*/
/*	physical files.  However, it may make more sense to :		*/
/*	a.  make one big file with nested records required for 		*/
/*		objects.  												*/
/*		or														*/
/*	b. make one file for each objects parameters.				*/
/*																*/
/*																*/
/*	INPUT DATA FILES											*/
/*																*/
/*	World Data File												*/
/*																*/
/*	The world data file should look like:						*/
/*																*/
/*	<start of file>												*/
/*	startyear (an integer corresponding to a julian year)		*/
/*	startmonth (an integer corresponding to a julian month)		*/
/*	startday (an integer corresponding to a monthly day)		*/
/*	starthour (an integer corresponding to a hour in 24 hours)	*/
/*	endyear	   (an integer corresponding to a julian year)		*/
/*	endmonth   (an integer corresponding to a julian month)		*/
/*	endday	 (an integer corresponding to a monthly day)		*/
/*	endhour (an integer corresponding to a hour in 24 hours)	*/
/*	number_of_basin_default_files (an integer)					*/
/*	basin_default_file_name_1									*/
/*	basin_default_file_name_2									*/
/*	basin_default_file_name_3									*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	hillslope_default_file_name_(number_of_hillslope_def_files)	*/
/*	number_of_hillslope_default_files (an integer)				*/
/*	hillslope_default_file_name_1								*/
/*	hillslope_default_file_name_2								*/
/*	hillslope_default_file_name_3								*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	hillslope_default_file_name_(number_of_hillslope_def_files)	*/
/*	number_of_zone_default_files (an integer)					*/
/*	zone_default_file_name_1									*/
/*	zone_default_file_name_2									*/
/*	zone_default_file_name_3									*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	zone_default_file_name_(number_of_zone_def_files)			*/
/*	number_of_landuse_default_files (an integer)				*/
/*	soil_default_file_name_1									*/
/*	soil_default_file_name_2									*/
/*	soil_default_file_name_3									*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	soil_default_file_name_(number_of_patch_def_files)			*/
/*	number_of_soil_default_files (an integer)					*/
/*	soil_default_file_name_1									*/
/*	soil_default_file_name_2									*/
/*	soil_default_file_name_3									*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	landuse_default_file_name_(number_of_patch_def_files)		*/
/*	number_of_stratum_default_files (an integer)				*/
/*	stratum_default_file_name_1									*/
/*	stratum_default_file_name_2									*/
/*	stratum_default_file_name_3									*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	stratum_default_file_name_(number_of_stratum_def_files)		*/
/*	number_of_base_stations (an integer)						*/
/*	base_station_file_name_1									*/
/*	base_station_file_name_2									*/
/*	base_station_file_name_3									*/
/*	base_station_file_name_(number_of_base_stations)			*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	num_basins	(an integer)									*/
/*	<basin record 1>											*/
/*	<basin record 2>											*/
/*	<basin record 3>											*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	<basin record num_basins>									*/
/*	<end of file>												*/
/*																*/
/*	The base station files are referenced by an ID so their 	*/
/*	order of appearance is not essential.						*/
/*																*/
/*	The default files are addressed in order of appearance for	*/
/*	each category.  i.e. a hillslope which has assigned to it	*/
/*	a hillslope default file of #3 will get the defaults given	*/
/*	in the third default file specified in the list.			*/
/*																*/
/*	Basin Record												*/
/*																*/
/*	<start of record>											*/
/*	ID															*/
/*	x co-ordinate												*/
/*	y co-ordinate												*/
/*	z co-ordinate												*/
/*	default_object_ID											*/
/*	basin_latitude												*/
/*	num_base_stations											*/
/*	base_station_ID#1											*/
/*	base_station_ID#1											*/
/*	base_station_ID#1											*/
/*			.													*/
/*			.													*/
/*			.													*/
/*	base_station_ID#1											*/
/*	number_of_hillslopes										*/
/*	<hillslope record #1>										*/
/*	<hillslope record #2>										*/
/*	<hillslope record #3>										*/
/*			.													*/
/*			.													*/
/*			.													*/
/*	<hillslope record #number_of_hillslopes>					*/
/* 	<end of basin record>										*/
/*																*/
/*																*/
/*	Hillslope Record											*/
/*																*/
/*	<start of hillslope record>									*/
/*	ID															*/
/*	x co-ordinate												*/
/*	y co-ordinate												*/
/*	z co-ordinate												*/
/*	default_object_ID											*/
/*	lambda														*/
/*	m															*/
/*	<num_base_stations>											*/
/*	<base station 1 ID>											*/
/*	<base station 2 ID>											*/
/*	<base station 3 ID>											*/
/*			.													*/
/*			.													*/
/*			.													*/
/*	<base station num_base_stations>							*/
/*	<num_zones>													*/
/*  <zone 1 record >											*/
/*				<patch 1 record>								*/
/*					<stratum 1 record>							*/
/*					<stratum 2 record>							*/
/*					<stratum 3 record>							*/
/*							.									*/
/*					<stratum n record>							*/
/*				<patch 2 record> ...							*/
/*				<patch 3 record> ... 							*/
/*						.										*/
/*				<patch n record>								*/
/*					.											*/
/*		<zone 2 record> ...										*/
/*		<zone 3 record>	...										*/
/*				,												*/
/*		<zone n record>											*/
/*	<end of hillslope #1 record>								*/
/*	<hillslope #2 record> ...									*/
/*	<hillslope #3 record> ...									*/
/*			.													*/
/*	<hillslope #n record> ...									*/
/*	<end of hillslopes file>									*/
/*																*/
/*	NOTE:  A < something record> indicates a record consisting	*/
/*			of a number of fields.  See below for definitions 	*/
/*			of each record.	A "." means that there are possibly	*/
/*			many records of the type sandwiching it. The number	*/
/*			"n" is just hypothetical and may vary from section	*/
/*			to section.  A "..." means that there are child		*/
/*			records that are not specified for brevity.			*/
/*																*/
/*	Record Documentation										*/
/*																*/
/*	Records indicated in the basin and hillslopes records are	*/
/*	specified below.  We have specified them in one place since	*/
/*	it is possible that in the future the actual file which     */
/*	holds the records may not correspond to the current division*/
/*	between basin and hillslopes files.							*/
/*																*/
/*	The fields which comprise a record may change depending		*/
/* 	on the command line options.  At the moment there are		*/
/*	four possible versions of each record based on the  		*/
/*	command line options: 										*/
/*																*/
/*		version 1 - no -e OR -g flags.							*/
/*		version 2 - -e flag AND no -g flag						*/
/*		version 3 - -g flag AND no -e flag						*/
/*		version 3 - -e flag AND -g flag							*/
/*																*/
/*	The records below begin with version 1, listing fields 		*/
/*	with the required data format and units.  The additional	*/ 
/*	versions follow.  Is is up to the user to provide the 		*/
/*	proper record format for the options used.					*/
/*																*/
/*	Version 1 Records											*/
/*																*/
/*	<start hillslope record>									*/
/*	ID - hillslope ID - 										*/
/*	x - x-co-ordinate of hillslope (centroid?) 					*/
/*	y - y-co-ordinate of hillslope (centroid?) 					*/
/*	z - z-co-ordinate of hillslope (centroid?) 				 	*/
/*	default_object_ID - hillslope default file 					*/
/*	m - hillslope mean m parameter		 						*/
/*	lambda - hillslope mean wetness index 						*/
/*	num_base_stations - number of base stations assigned		*/
/*			to hillslope - must match size of base station list	*/
/*	base_station_ID#1 - ID  of first base station- must be		*/
/*	base_station_ID#2 - same as ID#1 but for second 			*/
/*					base station assigned to the hillslope.		*/
/*	  ... for (num_base_stations) fields.						*/
/*	num_zones - number of zones in hillslope - no units			*/
/*	  ... zone records for the hillslope						*/
/*	<end hillslope record>										*/
/*																*/
/*																*/
/*	<start zone record>											*/
/*	ID - zone ID												*/ 
/*	x - x-co-ordinate of zone (centroid?) 						*/
/*	y - y-co-ordinate of zone (centroid?) 						*/
/*	z - z-co-ordinate of zone (centroid?) 						*/
/*	default_object_ID - zone default file 						*/
/*	area - zone area											*/
/*	slope - zone mean gradient 									*/
/*	aspect - zone mean aspect   								*/
/*	isohyet - isohyet of the zone 								*/
/* 	east_horizon												*/
/*	west_horizon												*/
/*	view_factor - obscured fraction of upper hemisphere 		*/
/*	num_base_stations - number of base stations assigned		*/
/*			to hillslope - must match size of base station list	*/
/*	base_station_ID#1 - ID  of first base station- must be		*/
/*			an ID of one of the world base stations.			*/
/*	base_station_ID#2 - same as ID#1 but for second 			*/
/*			base station assigned to the hillslope.				*/
/*	  ... for (num_base_stations) fields.						*/
/*	num_patches- number of patches in the zone 					*/
/*	<end zone record>											*/
/*																*/
/*	<start patch record>										*/
/*	ID - patch   ID												*/
/*	x - x-co-ordinate of patch	(centroid?)						*/
/*	y - y-co-ordinate of patch 	(centroid?)						*/
/*	z - z-co-ordinate of patch 	 (centroid?) 					*/
/*	default_object_ID - patch default file 						*/
/*	area	- patch area 										*/
/*	effective lai - total effective lai of patch				*/	
/*	ko - mean saturated hydraulic conductivity of the patch		*/
/*	lna - mean wetness index of patch 							*/
/*	unsat_storage												*/
/*	sat_deficit 												*/
/*	snowpack_water_equivalent_depth								*/
/*	snowpack_water_depth										*/
/*	snowpack_T													*/
/*	snowpack_surface_age										*/
/*	albedo														*/
/* 	surface_capacity	- 										*/
/* 	surface_depth		- 										*/
/*	humic_capacity	- 											*/
/*	humic_depth		- 											*/
/*	humic_ko		- 											*/
/*	num_base_stations - number of base stations assigned		*/
/*			to patch	- must match size of base station list	*/
/*	base_station_ID#1 - ID  of first base station- must be		*/
/*			an ID of one of the world base stations.			*/
/*	base_station_ID#2 - same as ID#1 but for second 			*/
/*			base station assigned to the patch  .				*/
/*	  ... for (num_base_stations) fields.						*/
/*	num_canopy strata- number of strata in the patch- no		*/
/*	  ... canopy_strata records for the patch					*/
/*	<end patch record>											*/
/*																*/
/*	<start stratum record>										*/
/*	ID - stratum   ID - no units								*/
/*	default_object_ID - stratum defalut file 				    */
/*	height														*/
/*	total_pai - surface area of all plants in stratum			*/
/*	total_lai - surface area of all leaves/needles in stratum 	*/
/*	gap_fraction - fraction of non-random gaps in canopy		*/
/*	rooting_depth												*/
/*	snow_stored													*/
/*	rain_stored													*/
/*	num_base_stations - number of base stations assigned		*/
/*			to patch	- must match size of base station list	*/
/*	base_station_ID#1 - ID  of first base station- must be		*/
/*			an ID of one of the world base stations.			*/
/*	base_station_ID#2 - same as ID#1 but for second 			*/
/*			base station assigned to the patch  .				*/
/*	  ... for (num_base_stations) fields.						*/
/*	<end stratum record>										*/
/*																*/	
/*	Default Files												*/
/*																*/
/*	Default files should, for uniformity, read in exactly the	*/
/*	same record structure as that currently read in by 			*/
/*	an object (from the basin file).  It is up to the interface	*/
/*	to make sure that the default files have sufficient data	*/
/*	to match the records being used in the world's objects.		*/
/*																*/
/*	Base Station Files											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "rhessys.h"


struct world_object *construct_world(struct command_line_object *command_line){
	/*----------------------------------------------------*/
	/*	Local function definition.			     		  */
	/*----------------------------------------------------*/
	char	**construct_filename_list( FILE *, int);
	long	julday( struct date );
	struct basin_default *construct_basin_defaults(int, char **, struct command_line_object *);
	struct hillslope_default *construct_hillslope_defaults(int, char **, struct command_line_object *);
	struct zone_default *construct_zone_defaults(int, char **, struct command_line_object *);
	struct soil_default *construct_soil_defaults(int, char **, struct command_line_object *);
	struct landuse_default *construct_landuse_defaults(int, char **, struct command_line_object *);
	struct stratum_default *construct_stratum_defaults(int, char **, struct command_line_object *);
	struct fire_default *construct_fire_defaults(int, char **, struct command_line_object *);
	struct surface_energy_default *construct_surface_energy_defaults(int, char **, struct command_line_object *);
	struct spinup_default *construct_spinup_defaults(int, char **, struct command_line_object *); 
	struct base_station_object *construct_base_station(char *,
		struct date, struct date, int);
	struct basin_object *construct_basin(struct command_line_object *, FILE *, int *, 
		struct base_station_object **, struct default_object *, 
        struct base_station_ncheader_object *,
        struct world_object *);
	struct fire_patch_object **construct_patch_fire_grid(struct world_object *, struct command_line_object *,struct fire_default def);
	struct fire_object **construct_fire_grid(struct world_object *);
	struct base_station_object **construct_ascii_grid(char *, struct date, struct date);
	struct base_station_ncheader_object *construct_netcdf_header(struct world_object *, char *);
	struct base_station_object *construct_netcdf_grid(struct base_station_object *, struct base_station_ncheader *, int *, float, float, float, struct date *, struct date *, struct command_line_object *);
  void *construct_spinup_thresholds(char *, struct world_object *, struct command_line_object *);	
	void *alloc(size_t, char *, char *);

	void resemble_hourly_date(struct world_object *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	FILE	*world_file;
	FILE	*header_file;
	int 	header_file_flag = 0;
	int		legacy_worldfile = 0;
	int	i;
	char	record[MAXSTR];
	struct world_object *world;
	/*--------------------------------------------------------------*/
	/*	Allocate a world array.										*/
	/*--------------------------------------------------------------*/
	world = (struct world_object *) alloc(1 * sizeof(struct world_object),
		"world", "construct_world");
	/*--------------------------------------------------------------*/
	/*	Try to open the world file in read mode.					*/
	/*--------------------------------------------------------------*/
	if ( (world_file = fopen(command_line[0].world_filename,"r")) == NULL ){
		fprintf(stderr,"FATAL ERROR:  Cannot open world file %s\n",
			command_line[0].world_filename);
		exit(EXIT_FAILURE);
	} /*end if*/

	/* Determine where to read worldfile header information from.
	 * The three options, in order of precedence are:
		1. -whdr command line option
		2. ${WORLDFILE_NAME}.hdr
		3. From legacy world file (deprecated)
	 */
	if ( command_line->world_header_flag ) {
		// Option 1. -whdr command line option
		header_file = fopen(command_line->world_header_filename, "r");
		if ( header_file == NULL ) {
			fprintf(stderr,"FATAL ERROR:  Cannot open world header file %s\n",
					command_line->world_header_filename);
			exit(EXIT_FAILURE);
		}
		header_file_flag = 1;
		printf("Reading specified world file header %s\n", command_line->world_header_filename);
	} else {
		// Set up file name for Option 2. ${WORLDFILE_NAME}.hdr
		if ( snprintf(command_line->world_header_filename, FILEPATH_LEN, "%s.hdr", command_line->world_filename) >= FILEPATH_LEN ) {
			fprintf(stderr,
					"Couldn't read world file header as filename would have been longer than the limit of %d\n", FILEPATH_LEN);
			exit(EXIT_FAILURE);
		}

		if ( access(command_line->world_header_filename, R_OK) == 0 ) {
			// Option 2. ${WORLDFILE_NAME}.hdr
			header_file = fopen(command_line->world_header_filename, "r");
			if ( header_file == NULL ) {
				fprintf(stderr,"FATAL ERROR:  Cannot open world header file %s\n",
						command_line->world_header_filename);
				exit(EXIT_FAILURE);
			}
			header_file_flag = 1;
			printf("\nFound world file header %s\n", command_line->world_header_filename);
		} else {
			// Option 3. From legacy world file (deprecated)
			header_file = world_file;
			legacy_worldfile = 1;
			printf("\nWARNING\nReading world file header from legacy world file.\nThis feature will be removed from a future release.\nPlease re-run g2w to generate a separate world file header.\nWARNING\n\n");
		}
	}

	if ( legacy_worldfile ) {
		/* For backward compatibility read date from worldfile if it is an old-style
		 * worldfile with an in-line header.
		 * NOTE: we are throwing these values away, letting the command line values
		 * take precedence.
		*/
		/*--------------------------------------------------------------*/
		/*	Read in the start calendar date (year, month, day, hour )	*/
		/*--------------------------------------------------------------*/
		fscanf( world_file , "%ld", &world[0].start_date.year);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].start_date.month);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].start_date.day);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].start_date.hour);
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*	Read in the end calendar date (year, month, day, hour )	*/
		/*--------------------------------------------------------------*/
		fscanf( world_file , "%ld", &world[0].end_date.year);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].end_date.month);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].end_date.day);
		read_record(world_file, record);
		fscanf( world_file , "%ld", &world[0].end_date.hour);
		read_record(world_file, record);
	}
	
	/*--------------------------------------------------------------
	 * Always use command line start and end dates
	 */
	world[0].start_date = command_line[0].start_date;
	world[0].end_date = command_line[0].end_date;

	/*--------------------------------------------------------------*/
	/*	Verify that the start hour was between 0 and 24.			*/
	/*--------------------------------------------------------------*/
	if ( (world[0].start_date.hour<=0) || (world[0].start_date.hour>24)){
		fprintf(stderr,"FATAL ERROR:  Start hour must be >0 and<=24.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Verify that the end hour was between 1 and 24.				*/
	/*--------------------------------------------------------------*/
	if ( (world[0].end_date.hour<=0) || (world[0].end_date.hour>24)){
		fprintf(stderr,"FATAL ERROR:  End hour must be >0 and<=24.\n");
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Compute the length of the worlds existance in each time step*/
	/*	Note that we use julday to compute the start and end days	*/
	/*	in absolute terms; and that we subtract the start and end	*/
	/*	days from the number of 24 hour days when finding the		*/
	/*	hours duration.												*/
	/*--------------------------------------------------------------*/
	world[0].duration.year = world[0].end_date.year - world[0].start_date.year;
	world[0].duration.month = (world[0].end_date.year
		- world[0].start_date.year)*12 + world[0].end_date.month
		- world[0].start_date.month;
	world[0].duration.day = julday( world[0].end_date)
		- julday( world[0].start_date) ;
	world[0].duration.hour = (world[0].duration.day) * 24
		+ world[0].end_date.hour - world[0].start_date.hour;
	/*--------------------------------------------------------------*/
	/*	Make sure that the world exists for 0 or more hours.		*/
	/*--------------------------------------------------------------*/
	if ( world[0].duration.hour < 0  ){
		fprintf(stderr,
			"FATAL ERROR: In construct_world, the end date given in %s world file is before the start date\n",
			command_line[0].world_filename);
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Allocate world defaults objects.							*/
	/*--------------------------------------------------------------*/
	world[0].defaults = (struct default_object *)
		alloc( sizeof(struct default_object ),"defaults","construct_world");
	
	/*--------------------------------------------------------------*/
	/*	Read in the number of basin default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_basin_default_files));
	read_record(header_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Read in the basin default files.			*/
	/*--------------------------------------------------------------*/
	world[0].basin_default_files = construct_filename_list( header_file,
		world[0].defaults[0].num_basin_default_files);
	
	/*-----------------------------------*/
	/*	Read in the number of hillslope default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_hillslope_default_files));
	read_record(header_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Read in the hillslope default files.			*/
	/*--------------------------------------------------------------*/
	world[0].hillslope_default_files = construct_filename_list(	header_file,
		world[0].defaults[0].num_hillslope_default_files);
	
	/*--------------------------------------------------------------*/
	/*	Read in the number of zone default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_zone_default_files));
	read_record(header_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Read in the zone default files.				*/
	/*--------------------------------------------------------------*/
	world[0].zone_default_files = construct_filename_list( header_file,
		world[0].defaults[0].num_zone_default_files);
	
	/*--------------------------------------------------------------*/
	/*	Read in the number of soil default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_soil_default_files));
	read_record(header_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Read in the soil default files.				*/
	/*--------------------------------------------------------------*/
	world[0].soil_default_files = construct_filename_list( header_file,
		world[0].defaults[0].num_soil_default_files);
	
	/*--------------------------------------------------------------*/
	/*	Read in the number of land cover default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_landuse_default_files));
	read_record(header_file, record);
        	
	/*--------------------------------------------------------------*/
	/*	Read in the land cover default files.			*/
	/*--------------------------------------------------------------*/
	world[0].landuse_default_files = construct_filename_list( header_file,
		world[0].defaults[0].num_landuse_default_files);
	
	/*--------------------------------------------------------------*/
	/*	Read in the number of veg default files.		*/
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].defaults[0].num_stratum_default_files));
	read_record(header_file, record);
	/*--------------------------------------------------------------*/
	/*	Read in the veg default files.			*/
	/*--------------------------------------------------------------*/
	world[0].stratum_default_files = construct_filename_list( header_file,
		world[0].defaults[0].num_stratum_default_files);

	
	/*--------------------------------------------------------------*/
	/*	If fire option has been set                             */
	/* Read in the number of fire default files.		*/
	/*--------------------------------------------------------------*/
	if (command_line[0].firespread_flag == 1) {
		fscanf(header_file,"%d",&(world[0].defaults[0].num_fire_default_files));
		read_record(header_file, record);
		/*--------------------------------------------------------------*/
		/*	Read in the fire default files.			*/
		/*--------------------------------------------------------------*/
		world[0].fire_default_files= construct_filename_list( header_file,
			world[0].defaults[0].num_fire_default_files);
	}
	
	/*--------------------------------------------------------------*/
	/*	If surface energy option has been set                             */
	/* Read in the number of surface energy default files.		*/
	/*--------------------------------------------------------------*/
	if (command_line[0].surface_energy_flag == 1) {
		fscanf(header_file,"%d",&(world[0].defaults[0].num_surface_energy_default_files));
		read_record(header_file, record);
		/*--------------------------------------------------------------*/
		/*	Read in the surface energy default files.			*/
		/*--------------------------------------------------------------*/
		world[0].surface_energy_default_files= construct_filename_list( header_file,
			world[0].defaults[0].num_fire_default_files);
	}
	
	/*--------------------------------------------------------------*/
	/*	If spinup flag has been set                             */
	/*      Read in the number of spinup default files              */
	/*--------------------------------------------------------------*/
	if (command_line[0].vegspinup_flag > 0) {
		fscanf(header_file,"%d",&(world[0].defaults[0].num_spinup_default_files));
		read_record(header_file, record);
		/*--------------------------------------------------------------*/
		/*	Read in the spinup default files.			*/
		/*--------------------------------------------------------------*/
	        world[0].spinup_default_files = construct_filename_list( header_file,
		        world[0].defaults[0].num_spinup_default_files);
	}

	/*--------------------------------------------------------------*/
	/*	Read in the number of base_station files.		*/
	/*  In the case of gridded climate input, there will only be    */
	/*  one entry in the world file, so we must get the number      */
	/*  of stations from the climate file instead of the worldfile. */
	/*--------------------------------------------------------------*/
	fscanf(header_file,"%d",&(world[0].num_base_stations));
	read_record(header_file, record);
	
	/*--------------------------------------------------------------*/
	/*	read in list of base_station files.			*/
	/*--------------------------------------------------------------*/
	world[0].base_station_files = construct_filename_list( header_file,
		world[0].num_base_stations);

	// If the file is an ascii gridded climate file, then the number
	// of base stations from the world file is wrong, and we need to
	// reset num_base_stations from the climate file
	if ( command_line[0].gridded_ascii_flag == 1) {
		printf("Opening climate grid %s\n", world[0].base_station_files[0]);
		FILE* grid_base;
		if ( (grid_base = fopen(world[0].base_station_files[0], "r")) == NULL ) {
			fprintf(stderr,
					"Unable to open climate grid file %s\n", 
					world[0].base_station_files[0]);
			exit(EXIT_FAILURE);
		}

		fscanf(grid_base, "%d", &world[0].num_base_stations);

		// Set the world.num_base_station_files to 1 for reference
		// when printing out the world
		world[0].num_base_station_files = 1;
	} else if (command_line[0].gridded_netcdf_flag == 1) {
        #ifndef LIU_NETCDF_READER
		world[0].num_base_station_files = world[0].num_base_stations;
        #endif
	} else {
		// Non-gridded climate, num_base_station_files = num_base_stations
		world[0].num_base_station_files = world[0].num_base_stations;
	}

	/*--------------------------------------------------------------*/
	/*	Construct the basin_defaults objects.			*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].basin = construct_basin_defaults(
		world[0].defaults[0].num_basin_default_files,
		world[0].basin_default_files, command_line);
	
	/*--------------------------------------------------------------*/
	/*	Construct the hillslope_defaults objects.		*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].hillslope = construct_hillslope_defaults(
		world[0].defaults[0].num_hillslope_default_files,
		world[0].hillslope_default_files, command_line);
	
	/*--------------------------------------------------------------*/
	/*	Construct the zones_defaults objects.			*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].zone = construct_zone_defaults(
		world[0].defaults[0].num_zone_default_files,
		world[0].zone_default_files, command_line);
	
	/*--------------------------------------------------------------*/
	/*	Construct the soil_defaults objects.			*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].soil = construct_soil_defaults(
		world[0].defaults[0].num_soil_default_files,
		world[0].soil_default_files, command_line);
	
	/*--------------------------------------------------------------*/
	/*	Construct the land_defaults objects.			*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].landuse = construct_landuse_defaults(
		world[0].defaults[0].num_landuse_default_files,
		world[0].landuse_default_files, command_line);
	
	/*--------------------------------------------------------------*/
	/*	Construct the stratum_defaults objects.			*/
	/*--------------------------------------------------------------*/
	world[0].defaults[0].stratum = construct_stratum_defaults(
		world[0].defaults[0].num_stratum_default_files,
		world[0].stratum_default_files, command_line);

	/*--------------------------------------------------------------*/
	/* if fire spread flag is set					*/
	/*	Construct the fire default objects.			*/
	/*--------------------------------------------------------------*/
	if (command_line[0].firespread_flag == 1) {
		world[0].defaults[0].fire = construct_fire_defaults(
			world[0].defaults[0].num_fire_default_files,
			world[0].fire_default_files, command_line);
	}

	printf("\nConstructed fire defaults\n");
	/*--------------------------------------------------------------*/
	/* if surface_energy spread flag is set					*/
	/*	Construct the fire default objects.			*/
	/*--------------------------------------------------------------*/
	if (command_line[0].surface_energy_flag == 1) {
		world[0].defaults[0].surface_energy = construct_surface_energy_defaults(
			world[0].defaults[0].num_surface_energy_default_files,
			world[0].surface_energy_default_files, command_line);
	}

	/*--------------------------------------------------------------*/
	/* if spinup flag is set				                              	*/
	/*	Construct the spinup default objects.	                    	*/
	/*--------------------------------------------------------------*/
	if (command_line[0].vegspinup_flag > 0) {
	  printf("\nConstructed spinup defaults \n");
		world[0].defaults[0].spinup = construct_spinup_defaults(
			world[0].defaults[0].num_spinup_default_files,
			world[0].spinup_default_files, command_line);
	}


	/*--------------------------------------------------------------*/
	/*	Construct the list of base stations.			*/
	/*--------------------------------------------------------------*/

	if (command_line[0].dclim_flag == 0) {
		/*--------------------------------------------------------------*/
		/*	Construct the base_stations.				*/
		/*--------------------------------------------------------------*/
		if ( command_line[0].gridded_ascii_flag == 1) {
			printf("\nConstructing base stations from ASCII GRID");
			world[0].base_stations = construct_ascii_grid( world[0].base_station_files[0],
												world[0].start_date, 
												world[0].duration);
		}
		else if(command_line[0].gridded_netcdf_flag == 1){
			printf("\nConstructing base stations from NETCDF GRID");
            #ifdef LIU_NETCDF_READER
            world[0].num_base_stations = get_netcdf_station_number(world[0].base_station_files[0]);
            #endif
			world[0].base_stations = (struct base_station_object **)
            alloc(
                        #ifdef LIU_NETCDF_READER
                        world[0].num_base_stations
                        #else
                        1000
                        #endif
                        * sizeof(struct base_station_object *),"base_stations","construct_world" );
            #ifdef LIU_NETCDF_READER
            for (int i = 0; i < world[0].num_base_stations; i++) {
                world[0].base_stations[i] = (struct base_station_object *)
                        alloc(sizeof(struct base_station_object),"base_station","construct_world");
            }
            #endif
            //world[0].base_station_ncheader = (struct base_station_ncheader_object *)
            //alloc(sizeof(struct base_station_ncheader_object),"base_station_ncheader","construct_world");
			world[0].base_station_ncheader = construct_netcdf_header(world,
                                                world[0].base_station_files[0]);
            #ifdef LIU_NETCDF_READER
            //#pragma omp parallel for
            for (int i = 0; i < world[0].num_base_stations; i++) {
                //printf("station %d ID:%d\n",i,world[0].base_stations[i]->ID);
                //fprintf(stderr,"\ni:%d\tstart_year:%d\tx:%lf\ty:%lf\tduration_days:%d\n",
                //        i,world[0].start_date.year,world[0].base_stations[i][0].x,world[0].base_stations[i][0].y,world[0].duration.day);
                world[0].base_stations[i] = construct_netcdf_grid(
                                                           world[0].base_stations[i],
                                                           world[0].base_station_ncheader,
                                                           &world[0].num_base_stations,
                                                           world[0].base_stations[i][0].proj_x,
                                                           world[0].base_stations[i][0].proj_y,
                                                           world[0].base_stations[i][0].z,
                                                           &world[0].start_date,
                                                           &world[0].duration,
							                                             command_line);

                //printf("new station %d ID:%d\n", i, world[0].base_stations[i][0].ID ); 
            }
            #endif
			/*printf("\n  file=%s firstID=%d num=%d numfiles=%d lai=%lf screenht=%lf sdist=%lf startyr=%d dayoffset=%d leapyr=%d precipmult=%lf",
				   world[0].base_station_ncheader[0].netcdf_tmax_filename,
				   world[0].ID,
				   world[0].num_base_stations,
				   world[0].num_base_station_files,
				   world[0].base_station_ncheader[0].effective_lai,
				   world[0].base_station_ncheader[0].screen_height,
				   world[0].base_station_ncheader[0].sdist,
				   world[0].base_station_ncheader[0].year_start,
				   world[0].base_station_ncheader[0].day_offset,
				   world[0].base_station_ncheader[0].leap_year,
				   world[0].base_station_ncheader[0].precip_mult);*/
		}
		else {
			printf("\nConstructing base stations");
			world[0].base_stations = (struct base_station_object **)
			alloc(world[0].num_base_stations *
				  sizeof(struct base_station_object *),"base_stations","construct_world" );
			
			
			for (i=0; i<world[0].num_base_stations; i++ ) {
				world[0].base_stations[i] = construct_base_station(
								world[0].base_station_files[i],
								world[0].start_date, world[0].duration,
								command_line[0].clim_repeat_flag);
			} /*end for*/

			/*--------------------------------------------------------------*/
			/* List the hourly record for all base station, resemble the hourly records*/
			/*--------------------------------------------------------------*/
			/*
			if(world[0].num_base_stations > 1){
			    resemble_hourly_date(world);
			}*/

		}
	} /*end if dclim_flag*/
	
        

	/*--------------------------------------------------------------*/
	/*	Read in the world ID.							*/
	/*--------------------------------------------------------------*/

	printf("\n Finished constructing base stations\n");
	fscanf(world_file,"%d",&(world[0].ID));
	read_record(world_file, record);

	printf("\n Constructing world %d\n", world[0].ID);
	/*--------------------------------------------------------------*/
	/*	Read in the number of basin	files.							*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(world[0].num_basin_files));
	read_record(world_file, record);

	printf("\n Constructing basins\n");
	/*--------------------------------------------------------------*/
	/*	Construct the list of basins. 								*/
	/*--------------------------------------------------------------*/
	world[0].basins = (struct basin_object **)
		alloc(world[0].num_basin_files * sizeof(struct basin_object *),
		"basins","construct_world");
	
	/*--------------------------------------------------------------*/
	/*	Construct the basins. 										*/
	/*--------------------------------------------------------------*/
	printf("\n Before for loop\n");  //XXX
	for (i=0; i<world[0].num_basin_files; i++ ){
	  printf("\n creating basin%d\n", i);  //XXX
		world[0].basins[i] = construct_basin(
			command_line, world_file, &(world[0].num_base_stations),
			world[0].base_stations,	world[0].defaults, 
            world[0].base_station_ncheader,
            world);
	} /*end for*/
	printf("\n After for loop\n");  //XXX

	/*--------------------------------------------------------------*/
	/*	If spinup flag is set construct the spinup thresholds object*/
	/*--------------------------------------------------------------*/
	if (command_line[0].vegspinup_flag > 0) {
    printf("\nReading spinup threshold file %s", command_line[0].vegspinup_filename);
		world[0].spinup_thresholds = construct_spinup_thresholds(command_line[0].vegspinup_filename, &world[0], command_line);
  }

	/*--------------------------------------------------------------*/
	/* if fire spread flag is set					*/
	/*	Construct the fire grid object.				*/
	/*--------------------------------------------------------------*/
	world[0].num_fire_grid_row = 0;
	world[0].num_fire_grid_col = 0;
	if (command_line[0].firespread_flag == 1) {
		world[0].patch_fire_grid = construct_patch_fire_grid(world, command_line,*(world[0].defaults[0].fire));
		world[0].fire_grid = construct_fire_grid(world);

	}	
	/*--------------------------------------------------------------*/
	/*	Close the world_file and header (if necessary)	         	*/
	/*--------------------------------------------------------------*/
	if ( fclose(world_file) != 0 ) exit(EXIT_FAILURE);
	if ( header_file_flag ) {
		fclose(header_file);
	}

	
	return(world);
} /*end construct_world.c*/
