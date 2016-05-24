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

// test comment
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
		double);


	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_fire_object **patch_fire_grid;
	struct patch_object *patch;
	int i,j,p,c,layer; 
	double pspread;
	double mean_fuel_veg=0,mean_fuel_litter=0,mean_soil_moist=0,mean_fuel_moist=0,mean_relative_humidity=0,
		mean_wind_direction=0,mean_wind=0,mean_z=0,mean_temp=0,mean_et=0,mean_pet=0,mean_et_under=0,mean_pet_under=0;
	double denom_for_mean=0;

	patch_fire_grid=world[0].patch_fire_grid;
	fire_grid = world[0].fire_grid;

	/*--------------------------------------------------------------*/
	/* update fire grid variables			*/
	/* first reset the values				*/
	/*--------------------------------------------------------------*/
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  	  for (j=0; j < world[0].num_fire_grid_col; j++) {
		if(world[0].patch_fire_grid[i][j].occupied_area==0)
		{
			  if(world[0].defaults[0].fire[0].fire_in_buffer==0)
			  {
				    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 0.0;
				    world[0].fire_grid[i][j].fuel_moist = 100.0;
				    world[0].fire_grid[i][j].soil_moist = 100.0;
				    world[0].fire_grid[i][j].relative_humidity = 100.0;
				    world[0].fire_grid[i][j].wind_direction = 0;
				    world[0].fire_grid[i][j].wind = 0.0;
				    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
				    world[0].fire_grid[i][j].et=0.0;
				    world[0].fire_grid[i][j].pet=1.0;
				    world[0].fire_grid[i][j].et_under=0.0;
				    world[0].fire_grid[i][j].pet_under=1.0;
				    world[0].fire_grid[i][j].ign_available=0;

			//	  printf("No fire in buffer\n");
			  }
			  else // if denom_for_mean==0, then this initializes the buffer, otherwise the mean is filled in below
			  {
				    world[0].fire_grid[i][j].fuel_veg = 10.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 10.0;
				    world[0].fire_grid[i][j].fuel_moist = 0;
				    world[0].fire_grid[i][j].soil_moist = 0;
				    world[0].fire_grid[i][j].relative_humidity = 0;
				    world[0].fire_grid[i][j].wind_direction = world[0].basins[0][0].hillslopes[0][0].zones[0][0].wind_direction;//patches[0].zone[0].wind_direction;// or pull the wind direction from the default wind
				    world[0].fire_grid[i][j].wind = 50.0;
				    world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
		  		    world[0].fire_grid[i][j].et=0.0;
				    world[0].fire_grid[i][j].pet=0.0;
				    world[0].fire_grid[i][j].et_under=0.0;
				    world[0].fire_grid[i][j].pet_under=0.0;
				  
				    world[0].fire_grid[i][j].ign_available=0;
			  }
		}
		else
		{
		    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
		    world[0].fire_grid[i][j].fuel_litter = 0.0;
		    world[0].fire_grid[i][j].fuel_moist = 0.0;
		    world[0].fire_grid[i][j].soil_moist = 0.0;
		    world[0].fire_grid[i][j].relative_humidity = 0.0;
		    world[0].fire_grid[i][j].wind_direction = 0.0;
		    world[0].fire_grid[i][j].wind = 0.0;
		    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
		    world[0].fire_grid[i][j].temp=0.0;
		    world[0].fire_grid[i][j].et=0.0;
		    world[0].fire_grid[i][j].pet=0.0;
		    world[0].fire_grid[i][j].et_under=0.0;
		    world[0].fire_grid[i][j].pet_under=0.0;
		    world[0].fire_grid[i][j].ign_available=1;	/* then make this available for ignition */
		}
	//    printf("checking num patches. row %d col %d numPatches %d\n",i,j,patch_fire_grid[i][j].num_patches);
		for (p=0; p < world[0].patch_fire_grid[i][j].num_patches; ++p) {
			patch = world[0].patch_fire_grid[i][j].patches[p];
			world[0].fire_grid[i][j].fuel_litter += (patch[0].litter_cs.litr1c +	patch[0].litter_cs.litr2c +	
				patch[0].litter_cs.litr3c +	patch[0].litter_cs.litr4c) * patch_fire_grid[i][j].prop_patch_in_grid[p];
			if( patch[0].litter.rain_capacity!=0)	// then update the fuel moisture, otherwise don't change it
			    world[0].fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
							patch_fire_grid[i][j].prop_patch_in_grid[p];
/*			fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
						patch_fire_grid[i][j].prop_patch_in_grid[p];
*/
			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
				world[0].fire_grid[i][j].fuel_veg += (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc) *
						patch_fire_grid[i][j].prop_patch_in_grid[p] ;
				}
			}
		//	printf("pixel veg and prop patch in grid: %lf\t%lf\n",world[0].fire_grid[i][j].fuel_veg,patch_fire_grid[i][j].prop_patch_in_grid[p]);
			

			world[0].fire_grid[i][j].soil_moist += patch[0].rootzone.S * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];	

			world[0].fire_grid[i][j].wind += patch[0].zone[0].wind * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].wind_direction += patch[0].zone[0].wind_direction * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].relative_humidity += patch[0].zone[0].relative_humidity * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].z += patch[0].z*patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].temp += patch[0].zone[0].metv.tavg*patch_fire_grid[i][j].prop_patch_in_grid[p];// temperature? mk
			world[0].fire_grid[i][j].et += patch[0].fire.et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].pet += patch[0].fire.pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
		//	world[0].fire_grid[i][j].et_under += patch[0].fire.et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
		//	world[0].fire_grid[i][j].pet_under += patch[0].fire.pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
	//printf("patch pet, patch et: %lf\t%lf\n",patch[0].fire.pet,patch[0].fire.et);

		}
		if(world[0].patch_fire_grid[i][j].occupied_area>0&&world[0].defaults[0].fire[0].fire_in_buffer==1)
		{
			denom_for_mean+=1;
			mean_fuel_veg+=world[0].fire_grid[i][j].fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
			mean_fuel_litter+=world[0].fire_grid[i][j].fuel_litter;
			mean_fuel_moist+=world[0].fire_grid[i][j].fuel_moist;
			mean_soil_moist+=world[0].fire_grid[i][j].soil_moist;
			mean_relative_humidity+=world[0].fire_grid[i][j].relative_humidity;
			mean_wind_direction+=world[0].fire_grid[i][j].wind_direction;
			mean_wind+=world[0].fire_grid[i][j].wind;
			mean_temp+=world[0].fire_grid[i][j].temp;	
			mean_et+=world[0].fire_grid[i][j].et;
			mean_pet+=world[0].fire_grid[i][j].pet;		
			mean_et_under+=world[0].fire_grid[i][j].et_under;
			mean_pet_under+=world[0].fire_grid[i][j].pet_under;		
		//	printf("et: %f  pet: %f  ",world[0].fire_grid[i][j].et,world[0].fire_grid[i][j].pet);
		}
		
		world[0].fire_grid[i][j].et=world[0].fire_grid[i][j].et*1000; // convert to mm
		world[0].fire_grid[i][j].pet=world[0].fire_grid[i][j].pet*1000; // convert to mm
		world[0].fire_grid[i][j].et_under=world[0].fire_grid[i][j].et_under*1000; // convert to mm
		world[0].fire_grid[i][j].pet_under=world[0].fire_grid[i][j].pet_under*1000; // convert to mm

			
	}
	}
//	printf("denom: %lf\t",denom_for_mean);
	if(denom_for_mean>0&&world[0].defaults[0].fire[0].fire_in_buffer==1)
	{
//		printf("in denom if\n");
		mean_fuel_veg=mean_fuel_veg/denom_for_mean;
		mean_fuel_litter=mean_fuel_litter/denom_for_mean;  
		mean_fuel_moist=mean_fuel_moist/denom_for_mean;
		mean_soil_moist=mean_soil_moist/denom_for_mean;
		mean_relative_humidity=mean_relative_humidity/denom_for_mean;
		mean_wind_direction=mean_wind_direction/denom_for_mean;
		mean_wind=mean_wind/denom_for_mean;
		mean_temp=mean_temp/denom_for_mean;
		mean_et=mean_et/denom_for_mean;
		mean_pet=mean_pet/denom_for_mean;
		mean_et_under=mean_et_under/denom_for_mean;
		mean_pet_under=mean_pet_under/denom_for_mean;
	//	printf("mean et: %f  mean pet: %f  ",mean_et,mean_pet);

	//	printf("mean pet, mean et: %lf\t%lf\n",mean_pet,mean_et);
	//	printf("mean wind: %lf, mean direction %lf \n",mean_wind,mean_wind_direction);
		for  (i=0; i< world[0].num_fire_grid_row; i++) {
		  for (j=0; j < world[0].num_fire_grid_col; j++) {
			  if(world[0].patch_fire_grid[i][j].occupied_area==0)
			  {

				world[0].fire_grid[i][j].fuel_veg = mean_fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				world[0].fire_grid[i][j].fuel_litter = mean_fuel_litter;
				world[0].fire_grid[i][j].fuel_moist = mean_fuel_moist;
				world[0].fire_grid[i][j].soil_moist = mean_soil_moist;
				world[0].fire_grid[i][j].relative_humidity = mean_relative_humidity;
				world[0].fire_grid[i][j].wind_direction = mean_wind_direction;
				world[0].fire_grid[i][j].wind = mean_wind;
				world[0].fire_grid[i][j].temp=mean_temp;
				world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev;
				world[0].fire_grid[i][j].et=mean_et*1000; // convert to mm
				world[0].fire_grid[i][j].pet=mean_pet*1000; // convert to mm
				world[0].fire_grid[i][j].et_under=mean_et_under*1000; // convert to mm
				world[0].fire_grid[i][j].pet_under=mean_pet_under*1000; // convert to mm
	//	printf("in denom if take 2 update values\n");
			  }
		     }
		}
	}
	
	/*--------------------------------------------------------------*/
	/* Call WMFire	 						*/
	/*--------------------------------------------------------------*/
	printf("calling WMFire: month %ld year %ld  cell res %lf  nrow %d ncol % d\n",current_date.month,current_date.year,command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col);
	world[0].fire_grid=WMFire(command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col,current_date.year,current_date.month,world[0].fire_grid,*(world[0].defaults[0].fire));
 	printf("Finished calling WMFire\n");
	/*--------------------------------------------------------------*/
	/* update biomass after fire					*/
	/*--------------------------------------------------------------*/

	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  		for (j=0; j < world[0].num_fire_grid_col; j++) {
			for (p=0; p < patch_fire_grid[i][j].num_patches; ++p) {
				patch = world[0].patch_fire_grid[i][j].patches[p];

				patch[0].burn = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];
				pspread = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];

	
				compute_fire_effects(
					patch,
					pspread);

			}
		}
	}
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



