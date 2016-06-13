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

	void	update_mortality(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		int,
		struct mortality_struct);

	void *alloc(size_t, char *, char *);


	void	update_litter_soil_mortality(
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct soil_c_object *,
		struct soil_n_object *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct fire_litter_soil_loss_struct);



	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_fire_object **patch_fire_grid;
	struct patch_object *patch;
	struct canopy_strata_object *canopy_strata_upper;
	struct canopy_strata_object *canopy_strata_lower;
	struct mortality_struct mort;
	struct fire_litter_soil_loss_struct fire_loss;
	int i,j,p, c, layer, strata; 
	int thin_type;
	double understory_litter_c;
	double layer_upper_height, layer_lower_height;
	double layer_upper_height_adj, layer_lower_height_adj;
	double layer_lower_c;
	double pspread;
	double layer_lower_c_loss_percent, understory_c_loss;
	double layer_upper_c_loss_percent, layer_upper_c_loss_percent_understory_comp;
	double layer_upper_c_loss_percent_adj1, layer_upper_c_loss_percent_adj2;
	double loss_vapor_percent, c_loss_vapor_percent;
	double c_loss_remain_percent, c_loss_remain_percent_alt;
	double mean_fuel_veg=0,mean_fuel_litter=0,mean_soil_moist=0,mean_fuel_moist=0,mean_relative_humidity=0,
		mean_wind_direction=0,mean_wind=0,mean_z=0,mean_temp=0,mean_et=0,mean_pet=0,mean_et_under=0,mean_pet_under=0;
	double denom_for_mean=0;

	patch_fire_grid=world[0].patch_fire_grid;
	fire_grid = world[0].fire_grid;

// add code here to define understory et and pet to calculate understory deficit. The definition of understory here
// will be the same as that for fire effects below

// default value to determine if you are in the understory canopy_strata_upper[0].defaults[0][0].understory_height_thresh

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

	if(world[0].defaults[0].fire[0].calc_fire_effects==1)
	{
		for  (i=0; i< world[0].num_fire_grid_row; i++) {
		  for (j=0; j < world[0].num_fire_grid_col; j++) {
		    for (p=0; p < patch_fire_grid[i][j].num_patches; ++p) {
			patch = world[0].patch_fire_grid[i][j].patches[p];

			patch[0].burn = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];
			pspread = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];

			if (pspread > 0){

				/* Calculate litter biomass for use later in canopy effects */
				understory_litter_c = patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c + patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c;

				fire_loss.loss_litr1c = 1;
				fire_loss.loss_litr2c = 1;
				fire_loss.loss_litr3c = 1;
				fire_loss.loss_litr4c = 1;
				fire_loss.loss_soil1c = 1;
				fire_loss.loss_soil2c = 0;
				fire_loss.loss_soil3c = 0;
				fire_loss.loss_soil4c = 0;
				fire_loss.loss_litr1n = 1;
				fire_loss.loss_litr2n = 1;
				fire_loss.loss_litr3n = 1;
				fire_loss.loss_litr4n = 1;
				fire_loss.loss_soil1n = 1;
				fire_loss.loss_soil2n = 0;
				fire_loss.loss_soil3n = 0;
				fire_loss.loss_soil4n = 0;


				update_litter_soil_mortality(
					 &(patch[0].cdf),
					 &(patch[0].ndf),
					 &(patch[0].soil_cs),
					 &(patch[0].soil_ns),
					 &(patch[0].litter_cs),
					 &(patch[0].litter_ns),
					 fire_loss);


				for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){

						/* Calculates metrics for target layer */
						canopy_strata_upper = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
						layer_upper_height = canopy_strata_upper[0].epv.height;

						/* Calculates metrics for next lowest layer (Can be redone to calculate all remaining lower layers) */
						if (patch[0].num_layers > (layer+1)){
							canopy_strata_lower = patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])];
							layer_lower_height = canopy_strata_lower[0].epv.height;
							layer_lower_c = canopy_strata_lower[0].cs.leafc + canopy_strata_lower[0].cs.live_stemc + canopy_strata_lower[0].cs.dead_stemc;
						} else {
							layer_lower_height = 0;
							layer_lower_c = 0;
						}

						/*--------------------------------------------------------------*/
						/* Calculate effects when upper layer is tall			*/
						/*--------------------------------------------------------------*/

						if (layer_upper_height > canopy_strata_upper[0].defaults[0][0].overstory_height_thresh){

							if (layer_lower_height > canopy_strata_upper[0].defaults[0][0].overstory_height_thresh){

								understory_c_loss = understory_litter_c;

							} else if (layer_lower_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_lower_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

								/* Determines the percent of loss attributed to understory. 1-layer_lower_height_adj gives loss attributed to overstory) */
								layer_lower_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_lower_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);

								/* Determine the amount of carbon lost in the understory component of the lower layer */
								if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
									fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
									exit(EXIT_FAILURE);
								} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
									layer_lower_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
								} else {
									layer_lower_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
								}
								understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent * layer_lower_height_adj) + understory_litter_c;	// layer_lower_height_adj accounts for adjustment for lower layer height

							} else if (layer_lower_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

								/* Determine the amount of carbon lost in the lower layer */
								if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
									fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
									exit(EXIT_FAILURE);
								} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
									layer_lower_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
								} else {
									layer_lower_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
								}
								understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent) + understory_litter_c;
							}


							/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
							layer_upper_c_loss_percent = 1 - (1/(1+exp(-(canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))));

							/* Determine the portion of c_loss_percent in the upper layer that is vaporized */
							if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
								fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
								exit(EXIT_FAILURE);
							} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
								loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
							} else {
								loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
							}

						/*--------------------------------------------------------------*/
						/* Calculate effects when upper layer is an intermediate height	*/
						/*--------------------------------------------------------------*/

						} else if (layer_upper_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_upper_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

							/* Determines the percent of loss attributed to understory. 1-layer_upper_height_adj gives loss attributed to overstory) */
							layer_upper_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_upper_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);


							if (layer_lower_height <= canopy_strata_upper[0].defaults[0][0].overstory_height_thresh && layer_lower_height >= canopy_strata_upper[0].defaults[0][0].understory_height_thresh){

								/* Determines the percent of loss attributed to understory. 1-layer_lower_height_adj gives loss attributed to overstory) */
								layer_lower_height_adj = (canopy_strata_upper[0].defaults[0][0].overstory_height_thresh - layer_lower_height)/(canopy_strata_upper[0].defaults[0][0].overstory_height_thresh-canopy_strata_upper[0].defaults[0][0].understory_height_thresh);


								/* ---Determine loss for understory component of upper layer--- */

								/* Function to relate pspread to percent loss in the upper layer */
								if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
									fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
									exit(EXIT_FAILURE);
								} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
									layer_upper_c_loss_percent_understory_comp = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
								} else {
									layer_upper_c_loss_percent_understory_comp = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
								}

								layer_upper_c_loss_percent_adj1 = layer_upper_c_loss_percent_understory_comp * layer_lower_height_adj * layer_upper_height_adj;	// layer_lower_height_adj and layer_upper_height_adj account for height adjustments to both layers 


								/* ---Determine loss for overstory component of upper layer--- */

								layer_lower_c_loss_percent = layer_upper_c_loss_percent_understory_comp;		/* percent c lost is same for upper and lower layer since lost is based on pspread for both */
								understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent * layer_lower_height_adj) + understory_litter_c;	// layer_lower_height_adj accounts for height adjustment for lower layer

								/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
								layer_upper_c_loss_percent_adj2 = (1 - 1/(1+exp(-canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))) * (1-layer_upper_height_adj);		// layer_upper_height_adj accounts for upper layer height adjustment

								/* Combine losses due to overstory and understory components of upper layer */
								layer_upper_c_loss_percent = layer_upper_c_loss_percent_adj1 + layer_upper_c_loss_percent_adj2;		// This may need to be capped at 1.


							} else if (layer_lower_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

								/* ---Determine loss for understory component of upper layer--- */

								/* Function to relate pspread to percent loss in the upper layer */
								if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
									fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
									exit(EXIT_FAILURE);
								} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
									layer_upper_c_loss_percent_understory_comp = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
								} else {
									layer_upper_c_loss_percent_understory_comp = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
								}

								layer_upper_c_loss_percent_adj1 = layer_upper_c_loss_percent_understory_comp * layer_upper_height_adj;		// layer_upper_height_adj accounts for upper layer height adjustment


								/* ---Determine loss for overstory component of upper layer--- */

								layer_lower_c_loss_percent = layer_upper_c_loss_percent_understory_comp;		/* percent c lost is same for upper and lower layer since lost is based on pspread for both */
								understory_c_loss = (layer_lower_c * layer_lower_c_loss_percent) + understory_litter_c;

								/* Sigmoidal function to relate understory carbon loss to percent loss in the upper layer */
								layer_upper_c_loss_percent_adj2 = (1 - 1/(1+exp(-canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k1*(understory_c_loss - canopy_strata_upper[0].defaults[0][0].biomass_loss_rel_k2)))) * (1-layer_upper_height_adj);		// layer_upper_height_adj accounts for upper layer height adjustment

								/* Combine losses due to overstory and understory components of upper layer */
								layer_upper_c_loss_percent = layer_upper_c_loss_percent_adj1 + layer_upper_c_loss_percent_adj2;		// This may need to be capped at 1.
							}


							/* Determine the portion of c_loss_percent that is vaporized from strata */
							if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
								fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
								exit(EXIT_FAILURE);
							} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
								loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
							} else {
								loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
							}

						/*--------------------------------------------------------------*/
						/* Calculate effects when upper layer is short			*/
						/*--------------------------------------------------------------*/

						} else if (layer_upper_height < canopy_strata_upper[0].defaults[0][0].understory_height_thresh) {

							/* Upper layer acts as a understory. Loss is a function of pspread */
							if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel <= 0){
								fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].pspread_loss_rel must be greater than 0.\n");
								exit(EXIT_FAILURE);
							} else if (canopy_strata_upper[0].defaults[0][0].pspread_loss_rel == 1){
								layer_upper_c_loss_percent = canopy_strata_upper[0].defaults[0][0].pspread_loss_rel * pspread;
							} else {
								layer_upper_c_loss_percent = (pow(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel,pspread)-1)/(canopy_strata_upper[0].defaults[0][0].pspread_loss_rel-1);
							}


							/* Determine the portion of c_loss_percent that is vaporized from landscape */
							if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel <= 0){
								fprintf(stderr, "ERROR: canopy_strata_upper[0].defaults[0][0].vapor_loss_rel must be greater than 0.\n");
								exit(EXIT_FAILURE);
							} else if (canopy_strata_upper[0].defaults[0][0].vapor_loss_rel == 1){
								loss_vapor_percent = canopy_strata_upper[0].defaults[0][0].vapor_loss_rel * layer_upper_c_loss_percent;
							} else {
								loss_vapor_percent = (pow(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel,layer_upper_c_loss_percent)-1)/(canopy_strata_upper[0].defaults[0][0].vapor_loss_rel-1);
							}
						}

						/*--------------------------------------------------------------*/
						/* Compute effects						*/
						/*--------------------------------------------------------------*/

						/* Compute percent of total carbon that is vaporized */
						c_loss_vapor_percent = loss_vapor_percent * layer_upper_c_loss_percent;
						/* Compute percent of total carbon that remains as litter/cwd */
						c_loss_remain_percent = layer_upper_c_loss_percent - c_loss_vapor_percent;
						/* Adjust c_loss_remain_percent since update mortality is run twice, with vaporized C removed first */
						c_loss_remain_percent_alt = c_loss_remain_percent / (1 - c_loss_vapor_percent);

						mort.mort_cpool = c_loss_vapor_percent;
						mort.mort_leafc = c_loss_vapor_percent;
						mort.mort_deadstemc = c_loss_vapor_percent;
						mort.mort_livestemc = c_loss_vapor_percent;
						mort.mort_frootc = c_loss_vapor_percent;
						mort.mort_deadcrootc = c_loss_vapor_percent;
						mort.mort_livecrootc = c_loss_vapor_percent;

						thin_type =2;	/* Harvest option */
						update_mortality(
							canopy_strata_upper[0].defaults[0][0].epc,
							&(canopy_strata_upper[0].cs),
							&(canopy_strata_upper[0].cdf),
							&(patch[0].cdf),
							&(canopy_strata_upper[0].ns),
							&(canopy_strata_upper[0].ndf),
							&(patch[0].ndf),
							&(patch[0].litter_cs),
							&(patch[0].litter_ns),
							thin_type,
							mort);

						/* Determine the portion of loss that remains on landscape */
						mort.mort_cpool = c_loss_remain_percent_alt;
						mort.mort_leafc = c_loss_remain_percent_alt;
						mort.mort_deadstemc = c_loss_remain_percent_alt;
						mort.mort_livestemc = c_loss_remain_percent_alt;
						mort.mort_frootc = c_loss_remain_percent_alt;
						mort.mort_deadcrootc = c_loss_remain_percent_alt;
						mort.mort_livecrootc = c_loss_remain_percent_alt;


						thin_type =3;
						update_mortality(
							canopy_strata_upper[0].defaults[0][0].epc,
							&(canopy_strata_upper[0].cs),
							&(canopy_strata_upper[0].cdf),
							&(patch[0].cdf),
							&(canopy_strata_upper[0].ns),
							&(canopy_strata_upper[0].ndf),
							&(patch[0].ndf),
							&(patch[0].litter_cs),
							&(patch[0].litter_ns),
							thin_type,
							mort);

					}
				}
			    }	
			}
		    }
		}
	}
	return;
} /*end execute_firespread_event.c*/


// ----------printing code samples -----------
//	printf("in execute_firespread_event_1\n");
//	printf("*********************\n");
//	printf("Understory Height = %f\n", height_under);


