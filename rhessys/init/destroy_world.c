/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_world								*/
/*																*/
/*	destroy_world.c - destroy a world object					*/
/*																*/
/*	NAME														*/
/*	destroy_world.c - destroy a world object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void destroy_world_line(&command_line, &world)				*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	Destroys a world object by: 								*/
/*																*/
/*	1.  closing any open world file handles.					*/
/*	2.  freeing any basestation arrays.							*/
/*	3.  recursively freeing arrays of any components of world.  */
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, March 15, 1996.								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
void destroy_world(struct command_line_object *command_line,
				   struct world_object *world)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	void	destroy_basin_defaults(
		int,
		int,
		struct basin_default *);
	
	void 	destroy_basin(
		struct command_line_object *,
		struct basin_object **);
	
	void	destroy_hillslope_defaults(
		int,
		int,
		struct hillslope_default *);
	void	destroy_zone_defaults(
		int,
		int,
		struct zone_default * );
	void	destroy_soil_defaults(
		int,
		int,
		struct soil_default * );
	void	destroy_landuse_defaults(
		int,
		int,
		struct landuse_default * );
	void	destroy_surface_energy_defaults(
		int,
		int,
		struct surface_energy_default * );
	void	destroy_fire_defaults(
		int,
		int,
		struct fire_default * );
	void	destroy_stratum_defaults(
		int,
		int,
		struct stratum_default * );
	void	destroy_base_station(
		struct command_line_object *,
		struct base_station_object *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	/*--------------------------------------------------------------*/
	/*	Destroy the basin_defaults objects.					*/
	/*--------------------------------------------------------------*/
	destroy_basin_defaults(
		world[0].defaults[0].num_basin_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].basin);
	/*--------------------------------------------------------------*/
	/*	Destroy the hillslope_defaults objects.					*/
	/*--------------------------------------------------------------*/
	destroy_hillslope_defaults(
		world[0].defaults[0].num_hillslope_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].hillslope);
	/*--------------------------------------------------------------*/
	/*	Destroy the zones_defaults objects.					*/
	/*--------------------------------------------------------------*/
	destroy_zone_defaults(
		world[0].defaults[0].num_zone_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].zone);
	/*--------------------------------------------------------------*/
	/*	Destroy the patch_defaults objects.					*/
	/*--------------------------------------------------------------*/
	destroy_soil_defaults(
		world[0].defaults[0].num_soil_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].soil);
	destroy_landuse_defaults(
		world[0].defaults[0].num_landuse_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].landuse);
	if (command_line[0].surface_energy_flag == 1)
		destroy_surface_energy_defaults(
		world[0].defaults[0].num_surface_energy_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].surface_energy);
	if (command_line[0].firespread_flag == 1)
		destroy_fire_defaults(
		world[0].defaults[0].num_fire_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].fire);
	/*--------------------------------------------------------------*/
	/*	Destroy the stratum_defaults objects.					*/
	/*--------------------------------------------------------------*/
	destroy_stratum_defaults(
		world[0].defaults[0].num_stratum_default_files,
		command_line[0].grow_flag,
		world[0].defaults[0].stratum);
	free(world[0].defaults);
	/*--------------------------------------------------------------*/
	/*	Destroy the base_stations objects.					*/
	/*--------------------------------------------------------------*/
	for ( i=0; i<world[0].num_base_stations; i++){
		destroy_base_station( command_line,
			world[0].base_stations[i]);
	} /*end for*/
	free( world[0].base_stations );
	/*--------------------------------------------------------------*/
	/*	Destroy the basins. 										*/
	/*--------------------------------------------------------------*/
	for (i=0; i<world[0].num_basin_files; i++ ){
		destroy_basin( 	command_line,
			&(world[0].basins[i]) );
	} /*end for*/
	free( world[0].basins );

	if (command_line[0].firespread_flag == 1)
	/*	free(world[0].fire_grid);*/
	/*--------------------------------------------------------------*/
	/*	Destroy the world.											*/
	/*--------------------------------------------------------------*/
	free( world );
	return;
} /*end destroy_world.c*/
