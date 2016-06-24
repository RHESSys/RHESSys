/*--------------------------------------------------------------*/
/* 																*/
/*					execute_csv_monthly_output_event					*/
/*																*/
/*	execute_csv_monthly_output_event - outputs monthly data			*/
/*																*/
/*	NAME														*/
/*	execute_csv_monthly_output_event - outputs monthly data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	execute_csv_monthly_output_event(						*/
/*					struct	world_object	*world,				*/
/*					struct	command_line_object *command_line,	*/
/*					struct	date	date,  						*/
/*					struct	world_output_file_object 	*outfile)*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs spatial structure according to commandline			*/
/*	specifications to specific files, for monthly info			*/
/*	a spatial structure is output only if its appropriate		*/
/*	option flag has been set and its ID matches a 				*/
/*	specified ID, or -999 which indicates all					*/
/*	units at that spatial scale are to be output				*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*	March 14, 1997	- 	RAF				*/ 
/*	Allowed output patch to also output the moss strata if	*/
/*		moss is present.				*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	execute_csv_monthly_output_event(
									 struct	world_object	*world,
									 struct	command_line_object *command_line,
									 struct	date	date,
									 struct	world_output_file_object	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void output_csv_monthly_basin(
		struct	basin_object *,
		struct	date,
		FILE	*);
	
	void output_csv_monthly_hillslope(	int,
		struct	hillslope_object *,
		struct	date,
		FILE	*);
	
	void output_csv_monthly_zone(	int, int,
		struct	zone_object *,
		struct	date,
		FILE	*);
	
	void output_csv_monthly_patch(
		int, int, int,
		struct	patch_object *,
		struct	date,
		FILE	*);
	
	void output_csv_monthly_canopy_stratum(
		int, int, int, int,
		struct	canopy_strata_object *,
		struct	date,
		FILE	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	basinID, hillID, patchID, zoneID, stratumID;
	int b,h,p,z,c;
	/*--------------------------------------------------------------*/
	/*	check to see if there are any print options					*/
	/*--------------------------------------------------------------*/
	if ((command_line[0].b != NULL) || (command_line[0].h != NULL) ||
		(command_line[0].z != NULL) || (command_line[0].p != NULL) ||
		(command_line[0].c != NULL)){
		/*--------------------------------------------------------------*/
		/*	output_csv_monthly basins												*/
		/*--------------------------------------------------------------*/
		for (b=0; b < world[0].num_basin_files; ++ b ) {
			/*--------------------------------------------------------------*/
			/*	Construct the basin output_csv_monthly files.							*/
			/*--------------------------------------------------------------*/
			if ( command_line[0].b != NULL ){
				basinID = command_line[0].b->basinID;
				if (( world[0].basins[b][0].ID == basinID) || (basinID == -999))
					output_csv_monthly_basin(
					world[0].basins[b],
					date,
					outfile->basin->monthly);
			}
			/*--------------------------------------------------------------*/
			/*	check to see if there are any lower print options			*/
			/*--------------------------------------------------------------*/
			if ((command_line[0].h != NULL) || (command_line[0].z != NULL) ||
				(command_line[0].p != NULL) || (command_line[0].c != NULL)){
				/*--------------------------------------------------------------*/
				/*	output_csv_monthly hillslopes 											*/
				/*--------------------------------------------------------------*/
				for (h=0; h < world[0].basins[b][0].num_hillslopes; ++h) {
					/*-----------------------------------------------------------*/
					/*	Construct the hillslope output_csv_monthly files.				  */
					/*----------------------------------------------------------*/
					if ( command_line[0].h != NULL ){
						basinID = command_line[0].h->basinID;
						hillID = command_line[0].h->hillID;
						if (( world[0].basins[b][0].ID == basinID)
							|| (basinID == -999))
							if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
								|| (hillID == -999))
								output_csv_monthly_hillslope(
								world[0].basins[b][0].ID,
								world[0].basins[b]->hillslopes[h],
								date,
								outfile->hillslope->monthly);
					}
					/*------------------------------------------------------------*/
					/*	check to see if there are any lower print options			*/
					/*----------------------------------------------------------*/
					if ((command_line[0].z != NULL) || (command_line[0].p != NULL) ||
						(command_line[0].c != NULL)){
						/*------------------------------------------------------*/
						/*	output_csv_monthly zones												*/
						/*-----------------------------------------------------*/
						for(z=0;
						z < world[0].basins[b][0].hillslopes[h][0].num_zones;
						++z) {
							/*------------------------------------------------------*/
							/*	Construct the zone output_csv_monthly files. 				*/
							/*------------------------------------------------------*/
							if ( command_line[0].z != NULL ){
								basinID = command_line[0].z->basinID;
								hillID = command_line[0].z->hillID;
								zoneID = command_line[0].z->zoneID;
								if (( world[0].basins[b][0].ID == basinID)
									|| (basinID == -999))
									if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
										|| (hillID == -999))
										if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID == zoneID)
											|| (zoneID == -999))
											output_csv_monthly_zone(
											world[0].basins[b][0].ID,
											world[0].basins[b][0].hillslopes[h][0].ID,
											world[0].basins[b]->hillslopes[h]->zones[z],
											date,
											outfile->zone->monthly);
							}
							/*------------------------------------------------------*/
							/*	check to see if there are any lower print options	  */
							/*------------------------------------------------------*/
							if ((command_line[0].p != NULL)
								|| (command_line[0].c != NULL)){
								/*--------------------------------------------------*/
								/*	output_csv_monthly patches 									 */
								/*--------------------------------------------------*/
								for (p=0;
								p < world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches;
								++p){
									/*------------------------------------------------*/
									/*	Construct the patch output_csv_monthly files.		  */
									/*------------------------------------------------*/
									if ( command_line[0].p != NULL ){
										basinID = command_line[0].p->basinID;
										hillID = command_line[0].p->hillID;
										zoneID = command_line[0].p->zoneID;
										patchID = command_line[0].p->patchID;
										if (( world[0].basins[b][0].ID == basinID)
											|| (basinID == -999))
											if (( world[0].basins[b][0].hillslopes[h][0].ID == hillID)
												|| (hillID == -999))
												if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID == zoneID)
													|| (zoneID == -999))
													if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].ID == patchID)
														|| (patchID == -999))
														output_csv_monthly_patch(
														world[0].basins[b]->ID,
														world[0].basins[b]->hillslopes[h]->ID,
														world[0].basins[b]->hillslopes[h]->zones[z]->ID,
														world[0].basins[b]->hillslopes[h]->zones[z]->patches[p],
														date,
														outfile->patch->monthly);
									}
									/*-----------------------------------------------*/
									/*	Construct the canopy_stratum output_csv_monthly file*/
									/*-----------------------------------------------*/
									if ( command_line[0].c != NULL ){
										/*---------------------------------------------*/
										/*	output_csv_monthly canopy stratum 				  */
										/*--------------------------------------------*/
										for(c=0;
										c < world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].num_canopy_strata;
										++c) {
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
															||	(patchID == -999))
															if (( world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].canopy_strata[c][0].ID == stratumID)
																|| (stratumID == -999))
																output_csv_monthly_canopy_stratum(
																world[0].basins[b][0].ID,
																world[0].basins[b][0].hillslopes[h][0].ID,
																world[0].basins[b][0].hillslopes[h][0].zones[z][0].ID,
																world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p][0].ID,
																world[0].basins[b]->hillslopes[h]->zones[z]->patches[p]->canopy_strata[c],
																date,
																outfile->canopy_stratum->monthly);
										} /* end stratum (c) for loop */
									} /* end if options */
								} /* end patch (p) for loop */
							} /* end if options */
						} /* end zone (z) for  loop*/
						} /* end if options */
					} /* end hillslope (h) for loop */
				} /* end if options */
			} /* end basin (b) for loop */
		} /* end if options */
		return;
} /*end execute_csv_monthly_output_event*/
