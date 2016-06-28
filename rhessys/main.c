/*------------------------------------------------------------------
	main.c - MAIN PROGRAM FOR RUNNING BGG
	syntax as described below.

	NAME
		rhessys - core program in RHESSys regional hydroecological
				simulation system.

	SYNOPSIS
		rhessys [options]

	OPTIONS

		-b		Basin output option.  Print out response variables for
				specified basins.

		-c		Canopy stratum output option.  Print out response variables for
				specified strata.

		-g		Grow option.  Try to read in dynamic bgc input data and
				output dynamic bgc parameters.

		-h 		Hillslope output option.  Print out response variables
				for specified hillslopes.

		-p		Patch output option.  Print out response variables for
				specified patches.
        
		-r		Routing option. Gives name of flow_table to define explicit routing
				connectivity.  Also trigger use of explicit routing over TOPMODEL
				approach

		-c		Stratum output option.  Print out response variables for
				specified strata.

		-t		Name of the temporal event control file.

		-v		Verbose option.
					none or 0 = most basic reporting
					1 = interim results reporting (daily only)
					2 = code debugging reporting (daily only)
					3 = code debugging reporting (daily and hourly)

		-w		Name of world file.

		-z		Zone output option.  Print out response variables for
				specified zones.  

		-pre		Give a particular output prefix.
	
		-st		start date (optional over-ride of worldfile)
		-ed		end date (optional over-ride of worldfile)
		-cvs		comma separated output
		-tmp		temporary value (CURRENTLY DON)a
		-th		threshold sat deficit (for drought output)
		-gw		include hillslope scale groundwater (sat_to_gw_coeff; gw_loss_coeff parameters scaled)
        -str    Streamflow routing option. Gives name of stream_table to define explicit streamflow routing connectivit.     
        -stro   Streamflow routing output option. Print out streamflow for specified stream reaches.
		-version Prints the RHESSys version number, then exits immediately

	DESCRIPTION


		This routine is the main (and only) entry point into the
		Regional Hydro Ecological Simulation System   
		Version 4.0 (RHESSys4.0).  RHESSys4.0 code or data objects
		are not compatible with previous versions of RHESSys.
		Users of previous versions of RHESSys should consult 
		online RHESSys4.0 documentation at 

		http:\\eos.geog.utoronto.ca\\u\bandits\html\rhessys4

		for information regarding why this revision was designed.

		The RHESSys4.0 software implementation consists of 
		a data object definition and a compatible event driven
		simulation shell which executes routines that :

		a. take input from the UNIX command line, ASCII data files
		with suitable format and potentially events sent via
		UNIX pipes.  

		b.  use the input and physically based algorithms to
		update data object state variables.

		c.  produce output as directed by input events in the
		form of state variable histories or ASCII files suiatable
		for use as subsequent input. 

		For compactness all documentation of the software
		implementation, file formats and object formats is
		embedded in the C source code text file headers.
		It should be noted that RHESSys4.0 is not a completely
		object oriented system in that algorithms that operate
		on objects are not embedded in the object definitions. 
		This was a concious design decision to permit linking
		to third party algorithms.  However, the potential
		for the development of algorithms which are not compatible
		(i.e. error free) with data objects (or vice versa)
		is therefore a concern.  Modifications to RHESSys4.0
		should be performed only with full knowledge of both
		data objects and algorithms involved and should be
		accompanied by suitable documentation in line and in
		source code text fragment headers.

		This mainline routine executes algorithms for
		constructing data objects which define the
		simulation "world", control parameters regarding the
		simulation to be performed, ASCII data files for
		receiving potential model output and a temporal
		event control object (tec object).
		A time sequential event loop is then
		initiated.  At the end of the event loop the 
		existing objects are destroyed.

		OUTPUT OPTIONS

		The default output is NO OUTPUT.

		Output can be requested in a separate file corresponding
		to a certain level of the modelling object heirarchy:
		basins, hillslopes, zones, intervals, patches.  
		  
		If no arguements follow an output flag ALL of the objects
		defined at the selected level of the heirarchy will be
		printed to the appropriate SINGLE output file.    
		ID's at selected level in option structure
		are set to -999 to indicate this conditions

		One can also opt to print out only one those objects attached
		to one BRANCH of the modelling object heirarchy.  To do
		thi one should specify valid ID numbers defining the connection
		from the world object to the branch requested.  
		Verification of the fact that the connection between the
		world object and the branch requested does in fact exist will
		be performed.

		In all cases output files will be created for annual,
		monthly, daily and hourly state variables as defined
		in the source code which handles the output for the
		selected level of the hierarchy and selected time step.

		The -b option produces one output data file per time step.
		The output file gives the response of each basin for each
		model time step output data is requested.
		The output file has a default name world_basin.<time_step>.

		The -h option produces one output data file per time step.
		The output file gives the response of each hillslope in the
		basin for each model time step output is requested.
		The output file has a default name world_hillslope.<time_step>.

		The -z option produces one output data file per time step.
		The output file gives the response of each zone in the
		basin for each model time step output is requested.
		The output file has a default name world_zone.<time_step>.

		The -p option produces one output data file per time step.
		The output file gives the response of each patch in the
		basin for each model time step output is requested.
		The output file has a default name world_patch.<time_step>.

		The -c option produces one output data file per time step.
		The output file gives the response of each stratum in the
		basin for each model time step output is requested.
		The output file has a default name world_stratum.<time_step>.

		The -pre option followed by a character string will add a
		prefix to each output file. 

		The -s option produces one output data file per time step.
		The output file gives the response of each patch in the
		basin for each model time step output is requested.
		The output file has a default name world_patch.<time_step>.

		CONTROL OPTIONS

		The -g option implies that the user has provided input 
		specifiction files which have appropriate "grow" parameters
		required by dynamic bgc.  In addition, all output will also
		include grow parameters.  The "grow" parameters are ignored
		and dynamicgc does not execute many rountines if -g is not
		used.

		The -v option will result in various status reports being
		sent to stdout.  Lines of verbose output have a date stamp 
		and a stamp identifying what part of the source code they
		are from.

		The -w option followed by a valid world file name specifies
		the world file to use with the simulation run.

		The -t option allows the user to specify the time duration 
		over which the response variables for the selected output
		mapping unit will be printed.  If the -t option is not
		used the time interval is defaulted as the range specified
		by startday and endday in the "world.ini" file.

		The -t option requires the user to have already created an
		ASCII time event control (tec) file with the format:

		<start of file>	
		start_out_date end_out_date 
		start_out_date end_out_date
		start_out_date end_out_date
			.
			.
			. 
		start_out_date end_out_date
		<end of file>

		The additional constraints placed upon the TEC file are:

		1.  All of the dates must match entries in
			all the base station clim files for each time step. 
		2.  The days must be listed in non-decreasing order from
			the first to nth entry pair.
		3.  There must be both a start and end date for every entry.

		To implement this file we have had to also enforce that	
		all clim files have dates which increase sequentially and
		cover the requested intervals.

	EXAMPLES

	1.  Run the simulation using the world file "sobs.world"
		with the TEC file "sobs.tec" and get output for each basin
		present.  Use the prefix "boreas" for the output files:

		%rhessys -w sobs.world -t sobs.tec -p -pre boreas

		This will result in a four output files called 
		"boreas_patch.yearly", "boreas_patch.monthly",
		"boreas_patch.daily", and "boreas_patch.hourly"
		which will contain response at the
		patch level for each patch specified in the sobs.world file 
		for the time durations specified in sobs.tec.
		Each line in the output files will have a date stamp and
		will uniquely identify the patch the line corresponds to.

	2.  Repeat example 1. but only get output for the basin specified
		by patch ID 1111 found in basin ID 1 , hillslope ID 11 and
		zone 111 (note that the ID's used here are arbitrary and
		no world ID is required as only a single world object is
		permitted):

		%rhessys -w boreas.ini -t ndvi.tec -p 1 11 111 1111 -pre boreas

		The output files will have the same name
		as in example 1.

	3.  Run the simulation using the world file "tlw.world" with the
		TEC file "tlw.tec" and get output for:        

			- the basin with basinID = 1
			- all hillslopes in the basin with basinID = 2
			- stratum  2 in patch 5 of zone 2 of hillslope 1 of basin 1

		%rhessys -w boreas.ini -t century.tec -b 1 -h 2 -p 1 1 2 5 2


	NOTES

		The -b, -h, -z, -i and -p output options can be used in 
		parallel.

		We have purposely avoided the ability for separate -t files
		for separate mapping units.

		We have purposely avoided the ability for being able to
		specify output without traversing down a branch in the 
		modelling object heirarchy (e.g. no asking for zone 1 for
		all hillslopes for basin 1 ; but you can ask for all zones
		for all hillslopes for basin 1).

	PROGRAMMERS NOTES

		The -v verbose option results in responses being directed
		to stdout.  Any additional -v responses added later should
		follow this convention OR make a wholesale change on
		all -v responses.

------------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*	Define header files and libraries with global scope.		*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"
#include <time.h>

// The $$RHESSYS_VERSION$$ string will be replaced by the make
// script to reflect the current RHESSys version.
const char RHESSYS_VERSION[] = "5.14.3";

/*--------------------------------------------------------------*/
/*	Main line of code.											*/
/*--------------------------------------------------------------*/
int	main( int main_argc, char **main_argv)

{
	clock_t startClock = clock();
	/*--------------------------------------------------------------*/
	/*	Non-function definitions. 									*/
	/*--------------------------------------------------------------*/
	struct	command_line_object 	*command_line;
	struct	tec_object				*tec;
	struct	world_object			*world;
	struct	world_output_file_object	*output;
	struct	world_output_file_object	*growth_output;
	char	*prefix;
	
	/*--------------------------------------------------------------*/
	/* Local Function declarations 									*/
	/*--------------------------------------------------------------*/
	struct   command_line_object *construct_command_line(
		int,
		char **);
	
	struct   world_object *construct_world(
		struct command_line_object *);
	
	struct	world_output_file_object	*construct_output_files(
		char *,
		struct command_line_object	*);
	
	
	struct	tec_object	*construct_tec(
		struct command_line_object *,
		struct world_object * );
	
	void 	execute_tec(
		struct	tec_object	*,
		struct	command_line_object	*,
		struct	world_output_file_object *,
		struct	world_output_file_object *,
		struct	world_object *);
	
	void	destroy_tec(
		struct tec_object * );
	
	void	destroy_output_files(
		struct	command_line_object	*,
		struct	world_output_file_object	*);
	
	void	destroy_world(
		struct command_line_object *,
		struct world_object *);
	
	void	destroy_command_line(
		struct command_line_object * );

	void   add_headers(
		struct world_output_file_object *,
		struct command_line_object * );

	void   add_growth_headers(
		struct world_output_file_object *,
		struct command_line_object * );


	srand((unsigned)(time(0)));

	/*--------------------------------------------------------------*/
	/*	Command line parsing.										*/
	/*--------------------------------------------------------------*/
	command_line = construct_command_line(main_argc, main_argv);


	/*--------------------------------------------------------------*/
	/* Check if print version flag was set. If so, just print out   */
	/* the version and return.                                      */
	/*--------------------------------------------------------------*/
	if (command_line[0].version_flag > 0 ) {
		printf("RHESSys Version: %s\n", RHESSYS_VERSION);
		return(EXIT_SUCCESS);
	}


	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED CON COMMAND LINE ***\n");
	
	/*--------------------------------------------------------------*/
	/*	Construct the world object.									*/
	/*--------------------------------------------------------------*/
	world = construct_world( command_line );
	if (command_line[0].verbose_flag > 0  )
		fprintf(stderr,"FINISHED CON WORLD ***\n");
	/*--------------------------------------------------------------*/
	/*	Construct the output file objects.							*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*      Make up the prefix for the output files.                */
	/*--------------------------------------------------------------*/
	prefix = (char *)calloc(256, sizeof(char));
	if ( command_line[0].output_prefix != NULL ){
		strcpy(prefix,command_line[0].output_prefix);
	}
	else{
		strcpy(prefix,PRE);
	}
	output = construct_output_files( prefix, command_line );
	if (command_line[0].grow_flag > 0) {
		strcat(prefix,"_grow");
		growth_output = construct_output_files(prefix, command_line );
	}
	else growth_output = NULL;

	add_headers(output, command_line);
		if (command_line[0].grow_flag > 0)
			add_growth_headers(growth_output, command_line);



	if(command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED CON OUTPUT\n");
	
	/*--------------------------------------------------------------*/
	/*	Create the tec object (temporal event control)				*/
	/*																*/
	/*	This object specifies temporal events such as output.		*/
	/*--------------------------------------------------------------*/
	tec = construct_tec( command_line, world);
	
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED CON TEC\n");
	/*--------------------------------------------------------------*/
	/*	AN EVENT LOOP WOULD GO HERE.								*/
	/*--------------------------------------------------------------*/
	fprintf(stderr,"Beginning Simulation\n");
	execute_tec( tec, command_line, output, growth_output, world );
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED EXE TEC\n");
	
	/*--------------------------------------------------------------*/
	/*	Destroy the tec object.										*/
	/*--------------------------------------------------------------*/
	destroy_tec( tec );
	
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED DES TEC\n");
	
	/*--------------------------------------------------------------*/
	/*	Destroy output file objects (close them)					*/
	/*--------------------------------------------------------------*/
	destroy_output_files( command_line, output );
	
	if (command_line[0].grow_flag > 0)
		destroy_output_files( command_line, growth_output );
	
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED DES OUTPUT FILES\n");
	
	/*--------------------------------------------------------------*/
	/*	Destroy the world.											*/
	/*--------------------------------------------------------------*/
	destroy_world(command_line, world );
	
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED DES WORLD\n");
	
	/*--------------------------------------------------------------*/
	/*	Destroy the command_line_object								*/
	/*--------------------------------------------------------------*/
	destroy_command_line( command_line );
	
	if (command_line[0].verbose_flag > 0 )
		fprintf(stderr,"FINISHED DES COMMAND LINE\n");
	
	/*--------------------------------------------------------------*/
	/*	The end.													*/
	/*--------------------------------------------------------------*/
    clock_t endClock =clock();
	
    printf("\ntime cost = %ld seconds\n",(endClock - startClock)/CLOCKS_PER_SEC);

	return(EXIT_SUCCESS);
	
} /*end main*/

/* this is a branch test by LMF */

