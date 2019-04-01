/*--------------------------------------------------------------*/
/*                                                              */
/*					construct_basin								                      */
/*																                              */
/*	construct_basin.c - creates a basin object					        */
/*																                              */
/*	NAME														*/
/*	construct_basin.c - creates a basin object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void construct_basin(										*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file									*/
/*			int		num_world_base_stations,					*/
/*			struct base_station_object	**world_base_stations,	*/
/*			struct basin_object	**basin_list,					*/
/*			struct default_object *defaults)					*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Constructs the basin object which consists of:				*/
/*		- basin specific parameters and identification			*/
/*		- a possible extension to a grow object					*/
/*		- a list of hillslopes in the basin.					*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Basins dont own climate files since all of their hillslopes	*/
/*	own them instead.  I guess this means we have to compute	*/
/*	a lot of local climate info but perhaps this is more		*/
/*	correct anyways.  As usual, it is up to the user to         */
/*	aggregate model  output from each hillslope if they want	*/
/*	basin values.												*/
/*																*/
/*	We use a list of pointers to hillslope objects rather than	*/
/*	a contiguous array of hillslope objects with a pointer to	*/
/*	the head of the array.  The list of pointers is a bit less	*/
/*	efficient since the pointer must be placed in the heap (RAM)*/
/*	at the start of each object BUT								*/
/*																*/
/*		1.  We can dynamically add and remove hillslopes.		*/
/*		2.  Most of the processing time is required on the 		*/
/*			sub-hillslope basis so the repositioning of pointers*/
/*			will not be too drastic if it is limited to the 	*/
/*			hillslope level or up.								*/
/*		3.  We will be able to make use of smaller chunks of 	*/
/*			RAM.  												*/
/*	Original code, January 16, 1996.							*/
/*	May 7, 1997	C.Tague											*/
/* 		- added a routine to sort hierarchy by elevation 		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rhessys.h"
#include "functions.h"
#include "params.h"

struct basin_object *construct_basin(
    struct	command_line_object	*command_line,
    FILE	*world_file,
    int	*num_world_base_stations,
    struct base_station_object	**world_base_stations,
    struct	default_object	*defaults,
    struct base_station_ncheader_object *base_station_ncheader,
    struct world_object *world)
{
  /*--------------------------------------------------------------*/
  /*	Local function definition.									*/
  /*--------------------------------------------------------------*/
  struct base_station_object *assign_base_station(
      int,
      int,
      struct base_station_object **);

  struct hillslope_object *construct_hillslope(
      struct	command_line_object *,
      FILE    *,
      int		*,
      struct base_station_object **,
      struct	default_object *,
      struct base_station_ncheader_object *,
      struct world_object *);

  void	*alloc( 	size_t, char *, char *);

  void	sort_by_elevation( struct basin_object *);

  struct stream_list_object construct_stream_routing_topology(
      char *,
      struct basin_object *,
      struct	command_line_object *);


  struct hillslope_object *find_hillslope_in_basin(
      int hillslope_ID,
      struct basin_object *basin);

  /*--------------------------------------------------------------*/
  /*	Local variable definition.									*/
  /*--------------------------------------------------------------*/
  int	base_stationID;
  int		i,j,z;
  double		check_snow_scale;
  double		n_routing_timesteps;
  char		record[MAXSTR];
  struct basin_object	*basin;
  param	*paramPtr=NULL;
  int	paramCnt=0;
  FILE	*routing_file;
  FILE  *surface_routing_file;
  struct hillslope_object *hillslope;
  int hillslope_ID;

  /*--------------------------------------------------------------*/
  /*	Allocate a basin object.								*/
  /*--------------------------------------------------------------*/
  basin = (struct basin_object *) alloc( 1 *
      sizeof( struct basin_object ),"basin","construct_basin");

  /*--------------------------------------------------------------*/
  /*	Read in the basinID.									*/
  /*--------------------------------------------------------------*/
  paramPtr=readtag_worldfile(&paramCnt,world_file,"Basin");
  /*for (i=0;i<paramCnt;i++){
    printf("value=%s,name =%s\n",paramPtr[i].strVal,paramPtr[i].name);
    }*/
  basin[0].ID = getIntWorldfile(&paramCnt,&paramPtr,"basin_ID","%d",-9999,0);
  basin[0].x = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",0.0,1);
  basin[0].y = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",0.0,1);
  basin[0].z = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",-9999,0);
  basin[0].basin_parm_ID = getIntWorldfile(&paramCnt,&paramPtr,"basin_parm_ID","%d",-9999,0);
  basin[0].latitude = getDoubleWorldfile(&paramCnt,&paramPtr,"latitude","%lf",-9999,0);
  basin[0].num_base_stations = getIntWorldfile(&paramCnt,&paramPtr,"n_basestations","%d",0,0);

  /*--------------------------------------------------------------*/
  /*	Create cosine of latitude to save future computations.		*/
  /*--------------------------------------------------------------*/
  basin[0].cos_latitude = cos(basin[0].latitude*DtoR);
  basin[0].sin_latitude = sin(basin[0].latitude*DtoR);

  /*--------------------------------------------------------------*/
  /*    Allocate a list of base stations for this basin.			*/
  /*--------------------------------------------------------------*/
  basin[0].base_stations = (struct base_station_object **)
    alloc(basin[0].num_base_stations *
        sizeof(struct base_station_object *),"base_stations","construct_basin");
  /*--------------------------------------------------------------*/
  /*      Read each base_station ID and then point to that base_statio*/
  /*--------------------------------------------------------------*/
  for (i=0 ; i<basin[0].num_base_stations; i++) {

    fscanf(world_file,"%d",&(base_stationID));
    printf( "*** RECORD %d ***\n", i );
    read_record(world_file, record);
    //printf ("Base Station ID %d \n", basin[0].base_stations[i][0].ID);
    /*--------------------------------------------------------------*/
    /*	Point to the appropriate base station in the base       	*/
    /*              station list for this world.					*/
    /*--------------------------------------------------------------*/
    basin[0].base_stations[i] = assign_base_station(
        base_stationID,
        *num_world_base_stations,
        world_base_stations);

  } /*end for*/
  /*--------------------------------------------------------------*/
  /*	Create the grow subobject if needed.						*/
  /*--------------------------------------------------------------*/
  if ( command_line[0].grow_flag == 1 ){
    /*--------------------------------------------------------------*/
    /*		Allocate memory for the grow subobject.					*/
    /*--------------------------------------------------------------*/
    basin[0].grow = (struct grow_basin_object *)
      alloc(1 * sizeof(struct grow_basin_object),
          "grow","construct_basin");
    /*--------------------------------------------------------------*/
    /*	NOTE:  PUT READS FOR GROW SUBOBJECT HERE.					*/
    /*--------------------------------------------------------------*/
  } /*end if*/
  /*--------------------------------------------------------------*/
  /*  Assign  defaults for this basin                             */
  /*--------------------------------------------------------------*/
  basin[0].defaults = (struct basin_default **)
    alloc( sizeof(struct basin_default *),"defaults","construct_basin" );
  i = 0;
  while (defaults[0].basin[i].ID != basin[0].basin_parm_ID) {
    i++;
    /*--------------------------------------------------------------*/
    /*  Report an error if no match was found.  Otherwise assign    */
    /*  the default to point to this basin.                         */
    /*--------------------------------------------------------------*/
    if ( i>= defaults[0].num_basin_default_files ){
      fprintf(stderr,
          "\nFATAL ERROR: in construct_basin,basin default ID %d not found.\n",
          basin[0].basin_parm_ID);
      exit(EXIT_FAILURE);
    }
  } /* end-while */
  basin[0].defaults[0] = &defaults[0].basin[i];

  /*--------------------------------------------------------------*/
  /*	Read in the number of hillslopes.						*/
  /*--------------------------------------------------------------*/
  fscanf(world_file,"%d",&(basin[0].num_hillslopes));
  read_record(world_file, record);

  /*--------------------------------------------------------------*/
  /*	Allocate a list of pointers to hillslope objects.			*/
  /*--------------------------------------------------------------*/
  basin[0].hillslopes = (struct hillslope_object **)
    alloc(basin[0].num_hillslopes * sizeof(struct hillslope_object *),
        "hillslopes","construct_basin");

  basin[0].area = 0.0;
  basin[0].max_slope = 0.0;
  n_routing_timesteps = 0.0;
  check_snow_scale = 0.0;
  /*--------------------------------------------------------------*/
  /*	Construct the hillslopes for this basin.					*/
  /*--------------------------------------------------------------*/
  for (int i=0; i<basin[0].num_hillslopes; i++){
    printf("reading hillslope %d\n", i);
    basin[0].hillslopes[i] = construct_hillslope(
        command_line, world_file, num_world_base_stations,
        world_base_stations, defaults, base_station_ncheader, world
        );

    basin[0].area += basin[0].hillslopes[i][0].area;
    n_routing_timesteps += basin[0].hillslopes[i][0].area * basin[0].hillslopes[i][0].defaults[0][0].n_routing_timesteps;
    if (basin[0].max_slope < basin[0].hillslopes[i][0].slope)
      basin[0].max_slope = basin[0].hillslopes[i][0].slope;
    if (command_line[0].snow_scale_flag == 1) {
      for (z = 0; z < basin[0].hillslopes[i][0].num_zones; z++) {
        for (j=0; j < basin[0].hillslopes[i][0].zones[z][0].num_patches; j++) {
          check_snow_scale += basin[0].hillslopes[i][0].zones[z][0].patches[j][0].snow_redist_scale * basin[0].hillslopes[i][0].zones[z][0].patches[j][0].area;
        }
      }
    }
  };
  printf("hillslopes complete\n");

  basin[0].defaults[0][0].n_routing_timesteps =
    (int) (n_routing_timesteps / basin[0].area);

  if (basin[0].defaults[0][0].n_routing_timesteps < 1)
    basin[0].defaults[0][0].n_routing_timesteps = 1;

  if (command_line[0].snow_scale_flag == 1) {
    check_snow_scale /= basin[0].area;
    if (fabs(check_snow_scale - 1.0) > ZERO	) {
      printf("\n *******  WARNING  ********** ");
      printf("\n Basin-wide  average snow scale is %lf", check_snow_scale);
      printf("\n Snow rescaling will alter net precip input by this scale factor\n\n");
    }
    if (command_line[0].snow_scale_tol > ZERO) {
      if ((check_snow_scale > command_line[0].snow_scale_tol) ||
          (check_snow_scale < 1/command_line[0].snow_scale_tol)) {
        printf("Basin-wide  average snow scale %lf is outside tolerance %lf",
            check_snow_scale, command_line[0].snow_scale_tol);
        printf("\n Exiting\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  /*--------------------------------------------------------------*/
  /*      initialize accumulator variables for this patch         */
  /*--------------------------------------------------------------*/
  basin[0].acc_month.et = 0.0;
  basin[0].acc_month.snowpack = 0.0;
  basin[0].acc_month.theta = 0.0;
  basin[0].acc_month.streamflow = 0.0;
  basin[0].acc_month.length = 0;
  basin[0].acc_month.denitrif = 0.0;
  basin[0].acc_month.nitrif = 0.0;
  basin[0].acc_month.mineralized = 0.0;
  basin[0].acc_month.uptake = 0.0;
  basin[0].acc_month.lai = 0.0;
  basin[0].acc_month.leach = 0.0;
  basin[0].acc_month.DOC_loss = 0.0;
  basin[0].acc_month.DON_loss = 0.0;
  basin[0].acc_month.stream_NO3 = 0.0;
  basin[0].acc_month.stream_NH4 = 0.0;
  basin[0].acc_month.stream_DON = 0.0;
  basin[0].acc_month.stream_DOC = 0.0;
  basin[0].acc_month.PET = 0.0;
  basin[0].acc_month.psn = 0.0;
  basin[0].acc_month.num_threshold = 0;


  basin[0].acc_year.et = 0.0;
  basin[0].acc_year.snowpack = 0.0;
  basin[0].acc_year.theta = 0.0;
  basin[0].acc_year.streamflow = 0.0;
  basin[0].acc_year.length = 0;
  basin[0].acc_year.denitrif = 0.0;
  basin[0].acc_year.nitrif = 0.0;
  basin[0].acc_year.mineralized = 0.0;
  basin[0].acc_year.uptake = 0.0;
  basin[0].acc_year.lai = 0.0;
  basin[0].acc_year.leach = 0.0;
  basin[0].acc_year.DOC_loss = 0.0;
  basin[0].acc_year.DON_loss = 0.0;
  basin[0].acc_year.stream_NO3 = 0.0;
  basin[0].acc_year.stream_NH4 = 0.0;
  basin[0].acc_year.stream_DON = 0.0;
  basin[0].acc_year.stream_DOC = 0.0;
  basin[0].acc_year.PET = 0.0;
  basin[0].acc_year.psn = 0.0;
  basin[0].acc_year.num_threshold = 0;

  /*--------------------------------------------------------------*/
  /*	Sort sub-hierarchy in the basin by elevation				*/
  /*--------------------------------------------------------------*/
  sort_by_elevation(basin);

  /*--------------------------------------------------------------*/
  /*	Read in flow routing topology for routing option	*/
  /*--------------------------------------------------------------*/
  if ( command_line[0].routing_flag == 1 ) {

    /*--------------------------------------------------------------*/
    /*  Try to open the routing file in read mode.                    */
    /*--------------------------------------------------------------*/
    if( (routing_file = fopen(command_line[0].routing_filename,"r")) == NULL ){
      fprintf(
          stderr,
          "FATAL ERROR:  Cannot open routing file %s\n",
          command_line[0].routing_filename
          );
      exit(EXIT_FAILURE);
    }

    int num_hillslopes;
    fscanf(routing_file,"%d",&num_hillslopes);
    struct hillslope_object **list = (struct hillslope_object **) alloc(
        num_hillslopes * sizeof(struct hillslope_object *),
        "hillslope list", //should still be patch list, but rlist needs to be attached to hillslope not the basin
        "construct_basin"
        );

    if( command_line[0].surface_routing_flag == 1 ) {
      if( (surface_routing_file = fopen(command_line[0].surface_routing_filename,"r")) == NULL ){
        fprintf(
            stderr,
            "FATAL ERROR:  Cannot open surface routing file %s\n",
            command_line[0].surface_routing_filename
            );
        exit(EXIT_FAILURE);
      }
    }

    // THIS IS WHERE OPENMP WILL PARALLELIZE
    for (int i=0; i<num_hillslopes; i++){
      fscanf( routing_file, "%d", &hillslope_ID );
      hillslope = find_hillslope_in_basin( hillslope_ID, basin );
      if ( command_line[0].ddn_routing_flag == 1 ) {
        hillslope->route_list = construct_ddn_routing_topology( routing_file, hillslope);
      } else {
        hillslope->route_list = construct_routing_topology( routing_file, hillslope, command_line, false);

        if ( command_line->surface_routing_flag == 1 ) {
          printf("\tReading surface routing table\n");
          hillslope->surface_route_list = construct_routing_topology( surface_routing_file, hillslope, command_line, true);

          if ( hillslope->surface_route_list->num_patches != hillslope->route_list->num_patches ) {
            fprintf(
                stderr,
                "\nFATAL ERROR: in construct_hillslope, surface routing table has %d patches, but subsurface routing table has %d patches. The number of patches must be identical.\n",
                hillslope->surface_route_list->num_patches, hillslope->route_list->num_patches
                );
            exit(EXIT_FAILURE);
          }
        }
      }
    }

    // XXX do we need to populate the surface routing objects if the ddn_routing_flag is set?
    // right now we're are not populating.
    if( command_line[0].surface_routing_flag == 0 && command_line[0].ddn_routing_flag != 1 ) {
      // we neeed to re-read the regular routing file and use this to create
      // the surface route list

      // close and re-open routing file to reset the read counter
      fclose(routing_file);

      if( (routing_file = fopen(command_line[0].routing_filename,"r")) == NULL ){
        fprintf(
            stderr,
            "FATAL ERROR:  Cannot open routing file %s\n",
            command_line[0].routing_filename
        );
        exit(EXIT_FAILURE);
      }

      fscanf(routing_file,"%d",&num_hillslopes);

      for (int i=0; i<num_hillslopes; i++){
        fscanf( routing_file, "%d", &hillslope_ID );
        hillslope = find_hillslope_in_basin( hillslope_ID, basin );
        hillslope->surface_route_list = construct_routing_topology( routing_file, hillslope, command_line, true );
      }
    }
  } else { // command_line[0].routing_flag != 1
    // For TOPMODEL mode, make a dummy route list consisting of all patches
    // in the hillslope, in no particular order.
   // hillslope->route_list = construct_topmodel_patchlist(hillslope);
   basin->route_list = construct_topmodel_patchlist(basin); //Ning Ren here may be an error..2019/03/31
  }

  /*--------------------------------------------------------------*/
  /*	Read in stream routing topology if needed	*/
  /*--------------------------------------------------------------*/
  if ( command_line[0].stream_routing_flag == 1) {
    basin[0].stream_list = construct_stream_routing_topology( command_line[0].stream_routing_filename, basin, command_line);
  } else {
    basin[0].stream_list.stream_network = NULL;
    basin[0].stream_list.streamflow = 0.0;
  }
  printf( "END CONSTRUCT BASIN\n");

  if ( command_line[0].routing_flag == 1 ) {
        fclose(routing_file); //N Ren 2019/03/31
   }

  if( command_line->surface_routing_flag == 1 ) {
    fclose( surface_routing_file );
  }
  return(basin);
} /*end construct_basin.c*/
