/*--------------------------------------------------------------*/
/* 																*/
/*					execute_csv_yearly_growth_output_event					*/
/*																*/
/*	execute_csv_yearly_growth_output_event - output_growths yearly data			*/
/*																*/
/*	NAME														*/
/*	execute_csv_yearly_growth_output_event - output_growths yearly data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	execute_csv_yearly_growth_output_event(						*/
/*					struct	world_object	*world,				*/
/*					struct	command_line_object *command_line,	*/
/*					struct	date	date,  						*/
/*					struct	world_output_file_object 	*outfile)*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_growths spatial structure according to commandline			*/
/*	specifications to specific files, for yearly info			*/
/*	a spatial structure is output_growth only if its appropriate		*/
/*	option flag has been set and its ID matches a 				*/
/*	specified ID, or -999 which indicates all					*/
/*	units at that spatial scale are to be output_growth				*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	execute_csv_yearly_growth_output_event(
										   struct	world_object	*world,
										   struct	command_line_object *command_line,
										   struct	date	date,
										   struct	world_output_file_object	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_yearly_growth_basin(
		struct	basin_object *,
		struct	date,
		FILE	*);
	
	void output_yearly_growth_hillslope( int,
		struct	hillslope_object *,
		struct	date,
		FILE	*);
	
	void output_yearly_growth_zone( 	int, int,
		struct	zone_object *,
		struct	date,
		FILE	*);
	
	void output_yearly_growth_patch(	int, int, int,
		struct	patch_object *,
		struct	date,
		FILE	*);
	
	void output_yearly_growth_canopy_stratum(
		int, int, int, int,
		struct	canopy_strata_object *,
		struct	date,
		FILE	*);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	basinID, hillID, patchID, zoneID, stratumID;
	int b,h,p,z,c;
	struct  cstate_struct *cs;
	/*--------------------------------------------------------------*/
	/*	check to see if there are any print options					*/
	/*--------------------------------------------------------------*/
	if ((command_line[0].b != NULL) || (command_line[0].h != NULL) ||
		(command_line[0].z != NULL) || (command_line[0].p != NULL) ||
		(command_line[0].c != NULL)){
		/*--------------------------------------------------------------*/
		/*	output_growth basins												*/
		/*--------------------------------------------------------------*/
		for (b=0; b < world[0].num_basin_files; ++ b ) {
			/*--------------------------------------------------------------*/
			/*	Construct the basin output_growth files.							*/
			/*--------------------------------------------------------------*/
			if ( command_line[0].b != NULL ){
				basinID = command_line[0].b->basinID;
				if (( world[0].basins[b][0].ID == basinID) || (basinID == -999))
					output_yearly_growth_basin(
					world[0].basins[b],
					date,
					outfile->basin->yearly);
			}
			/*--------------------------------------------------------------*/
			/*	output_growth hillslopes 											*/
			/*--------------------------------------------------------------*/
			for (h=0; h < world[0].basins[b][0].num_hillslopes; ++h) {
				/*--------------------------------------------------------------*/
				/*	Construct the hillslope output_growth files.						*/
				/*--------------------------------------------------------------*/
				if ( command_line[0].h != NULL ){
					basinID = command_line[0].h->basinID;
					hillID = command_line[0].h->hillID;
					if (( world[0].basins[b][0].ID == basinID) || (basinID == -999))
						if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
							|| (hillID == -999))
							output_yearly_growth_hillslope(
							world[0].basins[b][0].ID,
							world[0].basins[b]->hillslopes[h],
							date,
							outfile->hillslope->yearly);
				}
				/*--------------------------------------------------------------*/
				/*	output_growth zones												*/
				/*--------------------------------------------------------------*/
				for (z=0;z < world[0].basins[b][0].hillslopes[h][0].num_zones; ++z){
					/*----------------------------------------------------------*/
					/*	Construct the zone output_growth files.					  */
					/*-----------------------------------------------------------*/
					if ( command_line[0].z != NULL ){
						basinID = command_line[0].z->basinID;
						hillID = command_line[0].z->hillID;
						zoneID = command_line[0].z->zoneID;
						if (( world[0].basins[b][0].ID == basinID)||(basinID == -999))
							if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
								|| (hillID == -999))
								if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID == zoneID)
									|| (zoneID == -999))
									output_yearly_growth_zone(
									world[0].basins[b][0].ID,
									world[0].basins[b][0].hillslopes[h][0].ID,
									world[0].basins[b]->hillslopes[h]->zones[z],
									date,
									outfile->zone->yearly);
					}
					/*------------------------------------------------------------*/
					/*	output_growth patches 												*/
					/*-----------------------------------------------------------*/
					for(p=0;
					p<world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches;
					++p){
						/*----------------------------------------------------------*/
						/*	Construct the patch output_growth files.					  */
						/*----------------------------------------------------------*/
						if ( command_line[0].p != NULL ){
							basinID = command_line[0].p->basinID;
							hillID = command_line[0].p->hillID;
							zoneID = command_line[0].p->zoneID;
							patchID = command_line[0].p->patchID;
							if (( world[0].basins[b][0].ID == basinID)
								|| (basinID == -999))
								if((world[0].basins[b][0].hillslopes[h][0].ID == hillID)
									|| (hillID == -999))
									if((world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID == zoneID)
										|| (zoneID == -999))
										if((world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].ID == patchID)
											|| (patchID == -999))
											output_yearly_growth_patch(
											world[0].basins[b]->ID,
											world[0].basins[b]->hillslopes[h]->ID,
											world[0].basins[b]->hillslopes[h]->zones[z]->ID,
											world[0].basins[b]->hillslopes[h]->zones[z]->patches[p],
											date,
											outfile->patch->yearly);
						}
						/*----------------------------------------------------------*/
						/*	Construct the canopy_stratum output_growth files			*/
						/*---------------------------------------------------------*/
						for(c=0;
						c< world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].num_canopy_strata;
						++c){
							/*-------------------------------------------------------*/
							/*	output_growth canopy stratum 								  */
							/*------------------------------------------------------*/
							if ( command_line[0].c != NULL ){
								basinID = command_line[0].c->basinID;
								hillID = command_line[0].c->hillID;
								zoneID = command_line[0].c->zoneID;
								patchID = command_line[0].c->patchID;
								stratumID = command_line[0].c->stratumID;
								if (( world[0].basins[b][0].ID == basinID)
									|| (basinID == -999))
									if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
										|| (hillID == -999))
										if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID == zoneID)
											|| (zoneID == -999))
											if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].ID == patchID)
												||(patchID == -999))
												if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].canopy_strata[c][0].ID == stratumID)
													|| (stratumID == -999))
													output_yearly_growth_canopy_stratum(
													world[0].basins[b][0].ID,
													world[0].basins[b][0].hillslopes[h][0].ID,
													world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID,
													world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].ID,
													world[0].basins[b]->hillslopes[h]->zones[z]->patches[p]->canopy_strata[c],
													date,
													outfile->canopy_stratum->yearly);
							} /* end if options */
							/*------------------------------------------------------*/
							/* reset accumulated variables                          */
							/*------------------------------------------------------*/
							cs = &(world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].canopy_strata[c][0].cs);
							cs->gpsn_src = 0.0;
							cs->leaf_mr_snk = 0.0;
							cs->leaf_gr_snk = 0.0;
							cs->livestem_mr_snk = 0.0;
							cs->livestem_gr_snk = 0.0;
							cs->deadstem_gr_snk = 0.0;
							cs->livecroot_mr_snk = 0.0;
							cs->livecroot_gr_snk = 0.0;
							cs->deadcroot_gr_snk = 0.0;
							cs->froot_mr_snk = 0.0;
							cs->froot_gr_snk = 0.0;
						} /* end stratum (c) for loop */
					} /* end patch (p) for loop */
					} /* end zone (z) for  loop*/
				} /* end hillslope (h) for loop */
			} /* end basin (b) for loop */
		} /* end if options */
		return;
} /*end execute_csv_yearly_growth_output_event*/
