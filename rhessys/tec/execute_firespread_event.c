/*--------------------------------------------------------------*/
/* 																*/
/*					execute_firespread_event								*/
/*																*/
/*	execute_firespread_event.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	execute_firespread_event.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

void execute_firespread_event(
									 struct	world_object *world,
									 struct	command_line_object	*command_line,
									 struct date	current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


	void *alloc(size_t, char *, char *);


	void	compute_fire_effects(
		struct patch_object *,
		double,
		struct	command_line_object	*);
	
	void	compute_family_fire_effects(
		struct patch_family_object *,
		double,
		struct	command_line_object	*);

	struct fire_object **WMFire(
		double cell_res, 
		int nrow, 
		int ncol, 
		long year, 
		long month, 
		struct fire_object **fire_grid, 
		struct fire_default def);


	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_fire_object **patch_fire_grid;
	struct patch_object *patch;
	struct patch_family_object *patch_family;
//	struct node_fire_wui_dist *tmp_node;
	int i,j,p,c,layer;
	double pspread;
	double mean_fuel_veg=0,mean_fuel_litter=0, mean_fuel_cwd,mean_soil_moist=0,mean_fuel_moist=0,mean_relative_humidity=0,
		mean_wind_direction=0,mean_wind=0,mean_z=0,mean_temp=0,mean_et=0,mean_pet=0,mean_understory_et=0,mean_understory_pet=0;
	double denom_for_mean=0;
	double understory_pct_cover;

	patch_fire_grid=world[0].patch_fire_grid;
	fire_grid = world[0].fire_grid;

// add code here to define understory et and pet to calculate understory deficit. The definition of understory here
// will be the same as that for fire effects below

// default value to determine if you are in the understory, compare layer height to stratum-level height threshold (canopy_strata_upper[0].defaults[0][0].understory_height_thresh,
// compared to patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])].epv.height

	/*--------------------------------------------------------------*/
	/* update fire grid variables			*/
	/* first reset the values				*/
	/*--------------------------------------------------------------*/
	//printf("In WMFire\n");
//	printf("Start execute firespread event\n");
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  	  for (j=0; j < world[0].num_fire_grid_col; j++) {
		  world[0].fire_grid[i][j].fire_size=0; // reset grid to no fire
		  understory_pct_cover = 0; // set to 0 for each grid/patch family

		if(world[0].patch_fire_grid[i][j].occupied_area==0)
		{
			  if(world[0].defaults[0].fire[0].fire_in_buffer==0)
			  {
				    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 0.0;
				    world[0].fire_grid[i][j].fuel_cwd = 0.0;
				    world[0].fire_grid[i][j].fuel_moist = 100.0;
				    world[0].fire_grid[i][j].soil_moist = 100.0;
				    world[0].fire_grid[i][j].relative_humidity = 100.0;
				    world[0].fire_grid[i][j].wind_direction = 0;
				    world[0].fire_grid[i][j].wind = 0.0;
				    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
				    world[0].fire_grid[i][j].et=1.0; // mk: should be 1 so that the deficit in the buffer is zero
				    world[0].fire_grid[i][j].pet=1.0;
				    world[0].fire_grid[i][j].understory_et=0.0;
				    world[0].fire_grid[i][j].understory_pet=1.0;
				    world[0].fire_grid[i][j].ign_available=0;

				     if(world[0].defaults[0].fire[0].fire_verbose==1)

					  printf("No fire in buffer\n");

			  }
			  else // if denom_for_mean==0, then this initializes the buffer, otherwise the mean is filled in below
			  {
				    world[0].fire_grid[i][j].fuel_veg = 10.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 10.0;
				    world[0].fire_grid[i][j].fuel_cwd = 10.0;
				    world[0].fire_grid[i][j].fuel_moist = 0;
				    world[0].fire_grid[i][j].soil_moist = 0;
				    world[0].fire_grid[i][j].relative_humidity = 0;
				    world[0].fire_grid[i][j].wind_direction = world[0].basins[0][0].hillslopes[0][0].zones[0][0].wind_direction;//patches[0].zone[0].wind_direction;// or pull the wind direction from the default wind
				    world[0].fire_grid[i][j].wind = 50.0;
				    world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
		  		    world[0].fire_grid[i][j].et=0.0;
				    world[0].fire_grid[i][j].pet=0.0;
				    world[0].fire_grid[i][j].understory_et=0.0;
				    world[0].fire_grid[i][j].understory_pet=0.0;

				    world[0].fire_grid[i][j].ign_available=0;
			  }
		}
		else
		{
		    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
		    world[0].fire_grid[i][j].fuel_litter = 0.0;
		    world[0].fire_grid[i][j].fuel_cwd = 0.0;
		    world[0].fire_grid[i][j].fuel_moist = 0.0;
		    world[0].fire_grid[i][j].soil_moist = 0.0;
		    world[0].fire_grid[i][j].relative_humidity = 0.0;
		    world[0].fire_grid[i][j].wind_direction = 0.0;
		    world[0].fire_grid[i][j].wind = 0.0;
		    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
		    world[0].fire_grid[i][j].temp=0.0;
		    world[0].fire_grid[i][j].et=0.0;
		    world[0].fire_grid[i][j].pet=0.0;
		    world[0].fire_grid[i][j].understory_et=0.0;
		    world[0].fire_grid[i][j].understory_pet=0.0;
		    world[0].fire_grid[i][j].ign_available=1;	/* then make this available for ignition */
		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("Initialized fire grid patches\n");
		}

		//printf("Num patches: %d\n", world[0].patch_fire_grid[i][j].num_patches)
		//printf("checking num patches. row %d col %d numPatches %d\n",i,j,patch_fire_grid[i][j].num_patches);
		for (p=0; p < world[0].patch_fire_grid[i][j].num_patches; ++p) { // should just be 1 now... - 1 if non MSR, n if MSR where n is patches in patch fam
			if (world[0].defaults[0].fire[0].fire_verbose == 1)
			{
				printf("\nPatch p: %d, i: %d, j:%d\n", p, i, j);
			}

			patch = world[0].patch_fire_grid[i][j].patches[p]; //patch is still always a patch, loop over num patches w/ p will iterate over patches in patch family
			if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("Patch litter 1c %lf\n", patch[0].litter_cs.litr1c); 
			
			world[0].fire_grid[i][j].fuel_litter += (patch[0].litter_cs.litr1c +	patch[0].litter_cs.litr2c +	// This sums the litter pools
				patch[0].litter_cs.litr3c +	patch[0].litter_cs.litr4c) * patch_fire_grid[i][j].prop_patch_in_grid[p];
			if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("Fire grid fuel litter %lf\n",world[0].fire_grid[i][j].fuel_litter);

			if( patch[0].litter.rain_capacity!=0)	// then update the fuel moisture, otherwise don't change it
			    world[0].fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
							patch_fire_grid[i][j].prop_patch_in_grid[p];
/*			fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
						patch_fire_grid[i][j].prop_patch_in_grid[p];
*/
			 if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("Patch p: %d\n",p);
	
	// this is the canopy fuels

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
		
						if(world[0].defaults[0].fire[0].fire_verbose==1)				
							printf("Current layer: %d, count: %d\n",layer,c);

					world[0].fire_grid[i][j].fuel_veg += (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc) *
							patch_fire_grid[i][j].prop_patch_in_grid[p] ;
              world[0].fire_grid[i][j].fuel_litter +=(patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
                * patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.dead_leafc) *
                patch_fire_grid[i][j].prop_patch_in_grid[p]; // adds standing dead grass to fuel litter for firespread
					world[0].fire_grid[i][j].fuel_cwd += (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction //coarse woody fuels
                                                * patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.cwdc) *
                                                 patch_fire_grid[i][j].prop_patch_in_grid[p] ;
					}
				}
		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("pixel veg: %lf | prop patch in grid: %lf\n",world[0].fire_grid[i][j].fuel_veg,patch_fire_grid[i][j].prop_patch_in_grid[p]);
			

			world[0].fire_grid[i][j].soil_moist += patch[0].rootzone.S * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];	//soil moisture, divided by proportion of the patch in that grid cell;

			world[0].fire_grid[i][j].wind += patch[0].zone[0].wind * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].wind_direction += patch[0].zone[0].wind_direction * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].relative_humidity += patch[0].zone[0].relative_humidity * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].z += patch[0].z*patch_fire_grid[i][j].prop_patch_in_grid[p]; // elevation
			world[0].fire_grid[i][j].temp += patch[0].zone[0].metv.tavg*patch_fire_grid[i][j].prop_patch_in_grid[p];// temperature? mk
			world[0].fire_grid[i][j].et += patch[0].fire.et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].pet += patch[0].fire.pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			
			if (command_line[0].multiscale_flag = 1)
			{
				if (patch[0].fire.understory_et != 0 || patch[0].fire.understory_pet != 0)
				{
					world[0].fire_grid[i][j].understory_et += patch[0].fire.understory_et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
					world[0].fire_grid[i][j].understory_pet += patch[0].fire.understory_pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
					understory_pct_cover += world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
				}
			}
			else
			{
				if (patch[0].fire.understory_et == 0 && patch[0].fire.understory_pet == 0) // means no understory present, then use overall et and pet for deficit calculation for this patch
				{
					world[0].fire_grid[i][j].understory_et = world[0].fire_grid[i][j].et;
					world[0].fire_grid[i][j].understory_pet = world[0].fire_grid[i][j].pet;
				}
				else
				{
					world[0].fire_grid[i][j].understory_et += patch[0].fire.understory_et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
					world[0].fire_grid[i][j].understory_pet += patch[0].fire.understory_pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
				}
			}

		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("Patch PET: %lf | ET: %lf\n",patch[0].fire.pet,patch[0].fire.et);
		}

		// normalize understory et and PET by the pct coverage that actually has a understory, if no understory then use global values like above
		if (command_line[0].multiscale_flag = 1)
		{
			if (understory_pct_cover == 0)
			{
				world[0].fire_grid[i][j].understory_et = world[0].fire_grid[i][j].et;
				world[0].fire_grid[i][j].understory_pet = world[0].fire_grid[i][j].pet;
			}
			else
			{
				// if all patches have understory == 1 and no change, if half have understory == 0.5, and adjusts et and pet to correct patch (family) scale values
				world[0].fire_grid[i][j].understory_et /= understory_pct_cover;
				world[0].fire_grid[i][j].understory_pet /= understory_pct_cover;
			}
		}

		if(world[0].patch_fire_grid[i][j].occupied_area>0&&world[0].defaults[0].fire[0].fire_in_buffer==1) // if allowing fire into the buffer (on raster grid outside of watershed boundaries), then fill with mean field values within watershed boundary
		{ // this loop fills sums to calculate the mean value across watershed
			denom_for_mean+=1;
			mean_fuel_veg+=world[0].fire_grid[i][j].fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
			mean_fuel_litter+=world[0].fire_grid[i][j].fuel_litter;
			mean_fuel_cwd+=world[0].fire_grid[i][j].fuel_cwd;
			mean_fuel_moist+=world[0].fire_grid[i][j].fuel_moist;
			mean_soil_moist+=world[0].fire_grid[i][j].soil_moist;
			mean_relative_humidity+=world[0].fire_grid[i][j].relative_humidity;
			mean_wind_direction+=world[0].fire_grid[i][j].wind_direction;
			mean_wind+=world[0].fire_grid[i][j].wind;
			mean_temp+=world[0].fire_grid[i][j].temp;
			mean_et+=world[0].fire_grid[i][j].et;
			mean_pet+=world[0].fire_grid[i][j].pet;
			mean_understory_et+=world[0].fire_grid[i][j].understory_et;
			mean_understory_pet+=world[0].fire_grid[i][j].understory_pet;
		//	printf("et: %f  pet: %f  ",world[0].fire_grid[i][j].et,world[0].fire_grid[i][j].pet);
		}

		world[0].fire_grid[i][j].et=world[0].fire_grid[i][j].et*1000; // convert to mm
		world[0].fire_grid[i][j].pet=world[0].fire_grid[i][j].pet*1000; // convert to mm
		world[0].fire_grid[i][j].understory_et=world[0].fire_grid[i][j].understory_et*1000; // convert to mm
		world[0].fire_grid[i][j].understory_pet=world[0].fire_grid[i][j].understory_pet*1000; // convert to mm
	}
	}
//	printf("denom: %lf\t",denom_for_mean);
	if(denom_for_mean>0&&world[0].defaults[0].fire[0].fire_in_buffer==1) // so here we calculate the mean value
	{
//		printf("in denom if\n");
		mean_fuel_veg=mean_fuel_veg/denom_for_mean;
		mean_fuel_litter=mean_fuel_litter/denom_for_mean;
		mean_fuel_cwd=mean_fuel_cwd/denom_for_mean;
		mean_fuel_moist=mean_fuel_moist/denom_for_mean;
		mean_soil_moist=mean_soil_moist/denom_for_mean;
		mean_relative_humidity=mean_relative_humidity/denom_for_mean;
		mean_wind_direction=mean_wind_direction/denom_for_mean;
		mean_wind=mean_wind/denom_for_mean;
		mean_temp=mean_temp/denom_for_mean;
		mean_et=mean_et/denom_for_mean;
		mean_pet=mean_pet/denom_for_mean;
		mean_understory_et=mean_understory_et/denom_for_mean;
		mean_understory_pet=mean_understory_pet/denom_for_mean;
	//	printf("mean et: %f  mean pet: %f  ",mean_et,mean_pet);

	//	printf("mean pet, mean et: %lf\t%lf\n",mean_pet,mean_et);
	//	printf("mean wind: %lf, mean direction %lf \n",mean_wind,mean_wind_direction);
		for  (i=0; i< world[0].num_fire_grid_row; i++) {
		  for (j=0; j < world[0].num_fire_grid_col; j++) {
			  if(world[0].patch_fire_grid[i][j].occupied_area==0) // and here we fill in the buffer
			  {

				world[0].fire_grid[i][j].fuel_veg = mean_fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				world[0].fire_grid[i][j].fuel_litter = mean_fuel_litter;
				world[0].fire_grid[i][j].fuel_cwd = mean_fuel_cwd;
				world[0].fire_grid[i][j].fuel_moist = mean_fuel_moist;
				world[0].fire_grid[i][j].soil_moist = mean_soil_moist;
				world[0].fire_grid[i][j].relative_humidity = mean_relative_humidity;
				world[0].fire_grid[i][j].wind_direction = mean_wind_direction;
				world[0].fire_grid[i][j].wind = mean_wind;
				world[0].fire_grid[i][j].temp=mean_temp;
				world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev;
				world[0].fire_grid[i][j].et=mean_et*1000; // convert to mm
				world[0].fire_grid[i][j].pet=mean_pet*1000; // convert to mm
				world[0].fire_grid[i][j].understory_et=mean_understory_et*1000; // convert to mm
				world[0].fire_grid[i][j].understory_pet=mean_understory_pet*1000; // convert to mm
	//	printf("in denom if take 2 update values\n");
			  }
		     }
		}
	}

	/*--------------------------------------------------------------*/
	/* Call WMFire	 												*/
	/*--------------------------------------------------------------*/
	if (command_line[0].verbose_flag <= -7) {printf("\n----- Starting WMFire -----\n");}
	printf("Calling WMFire on: month %ld year %ld  cell res %lf  nrow %d ncol % d\n",current_date.month,current_date.year,command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col);
	// needs to return fire size, not just grid--create structure that includes fire size, or a 12-member array of fire sizes, and/or a tally of fires > 1000 acres
	world[0].fire_grid = WMFire(command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col,current_date.year,current_date.month,world[0].fire_grid,*(world[0].defaults[0].fire));
	if (command_line[0].verbose_flag <= -7) {printf("----- Finished WMFire -----\n");}
	/*--------------------------------------------------------------*/
	/* update biomass after fire									*/
	/*--------------------------------------------------------------*/
	if (command_line[0].verbose_flag <= -7) { printf("Updating biomass\n");}
	double fire_size_ha;// convert fire_size in pixels to fire size in ha, for salience
	struct WUI_object *WUI_ptr;//pointers to navigate salience lists
	struct wui_dist_list *patch_wui_dist_list_ptr;
	struct wui_dist_list *sal_wui_dist_list_ptr;
	fire_size_ha=world[0].fire_grid[0][0].fire_size*command_line[0].fire_grid_res*command_line[0].fire_grid_res*.0001;//.0001 ha/m^2, assumes resolution is in m
	if(world[0].fire_grid[0][0].fire_size>0) // only do this if there was a fire
	{
	if (world[0].defaults[0].fire[0].calc_fire_effects == 1||command_line[0].salience_flag == 1)// enter loop if either fire effects or salience is flagged
	{
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  		for (j=0; j < world[0].num_fire_grid_col; j++) {
	

				if (command_line[0].multiscale_flag == 1)
				{

					if (world[0].patch_fire_grid[i][j].num_patches > 0)
					{
						patch_family = world[0].patch_fire_grid[i][j].patch_families[0];
						// grid in patch is length 1 since only the [0] is ever set, regardless of allocation
						pspread = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[0];
						// Add pspread to patch
						for (p = 0; p < patch_family[0].num_patches_in_fam; p++) {
							patch_family[0].patches[p][0].pspread = pspread;
						}

						if (world[0].defaults[0].fire[0].fire_verbose == 1)
						{
							printf("Start fire effects: grid i:%d j:%d | patch family %d | num patches %d | pspread %lf | burn %lf\n",
								i, j, patch_family[0].family_ID, patch_family[0].num_patches_in_fam, pspread, world[0].fire_grid[i][j].burn);
						}
						compute_family_fire_effects(
							patch_family,
							pspread,
							command_line);
					}
/*                                        compute_family_fire_effects(
                                                patch_family,
                                                pspread,
                                                command_line);
*/
// make sure to change back to >400, using 0 for patch-level testing
					if(fire_size_ha>0&&command_line[0].salience_flag == 1&pspread>0)// then potential salience event. Check if any  burned pixels are within 2,5,10 km of wui
					{
//patch[0].fire.severity >= patch[0].landuse_defaults[0][0].salience_fire_level
// a fire severity flag rather than just burned or not?	
 					//	printf("patch ID: %d\n",patch_family[0].patches[0][0].ID);
						patch_wui_dist_list_ptr=patch_family[0].patches[0][0].wui_dist->next;// skip the first wui, which represents non-salience
						WUI_ptr=world[0].WUI_list->next;
						printf("Pre salience event dist: %d, patch ID %d and dist %d, wui ID %d\n", WUI_ptr->fire_occurence,patch_family[0].patches[0][0].ID,patch_wui_dist_list_ptr->dist,WUI_ptr->ID);
						if(patch_wui_dist_list_ptr->dist<WUI_ptr->fire_occurence) // for this wui, is this a more "salient" event?
						     WUI_ptr->fire_occurence=patch_wui_dist_list_ptr->dist; // then a fire occurred this year closer to the wui. Record this one					
						printf("Salience event loc1 dist: %d, id: %d\n", WUI_ptr->fire_occurence, WUI_ptr->ID);
						while( patch_wui_dist_list_ptr->next!=NULL) //move on to the next wui for this patch
						{
							 patch_wui_dist_list_ptr= patch_wui_dist_list_ptr->next;
							WUI_ptr=WUI_ptr->next; // move through the lists in parallel
							 if(patch_wui_dist_list_ptr->dist<WUI_ptr->fire_occurence)
                                                     		WUI_ptr->fire_occurence=patch_wui_dist_list_ptr->dist; // then a fire occurred this year closer to the wui. Record this one
						}					
						printf("Salience event dist: %d, id: %d\n", WUI_ptr->fire_occurence,WUI_ptr->ID);
					}
					
				}
				else
				{
					for (p = 0; p < patch_fire_grid[i][j].num_patches; ++p)
					{
						patch = world[0].patch_fire_grid[i][j].patches[p];
						pspread = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];
						patch[0].pspread = pspread;
						
						// so I think here we could flag whether to turn salient fire on in wui; convert fire size in pixels to ha, assuming the cell_res is in m
						if (world[0].defaults[0].fire[0].calc_fire_effects == 1)
						{
							compute_fire_effects(
								patch,
								pspread,
								command_line);
						}
					}
				} // end IF NOT MSR

			} // end for grid col
		} // end for grid row
	}

		} // end for grid col
//	}// end for grid row
//	} 
//	}
	return;
} /*end execute_firespread_event.c*/

// ----------printing code samples -----------
//	printf("in execute_firespread_event_1\n");
//	printf("*********************\n");
//	printf("Understory Height = %f\n", height_under);

/*

// For 	Local variable definition.
struct fire_veg_loss_struct	fire_veg_effects;

// RHESSys.h
/*----------------------------------------------------------*/
/*      Define post fire vegetation loss structure.    	    */
/*----------------------------------------------------------*/
/*struct fire_veg_loss_struct
{
	double pspread
	double layer_upper_height
	double layer_lower_height
	double layer_lower_c
	double understory_litter_c
};*/
