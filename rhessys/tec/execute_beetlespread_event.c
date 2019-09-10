/*--------------------------------------------------------------*/
/* 																*/
/*					execute_beetlespread_event					*/
/*																*/
/*	execute_beetlespread_event.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	execute_beetle_event.c - creates a patch object				*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*	Using the execute_firespread_event.c as an example			*/
/*	Here because the beetle outbreak model is under development	*/
/*	we use prescribed mortality to simulate, or you can input	*/
/*	I can input the mortality map 								*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

// test comment
void execute_beetlespread_event(
									 struct	world_object *world,
									 struct	command_line_object	*command_line,
									 struct date	current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/

	void	compute_fire_effects(  //N.REN 20180629
		struct patch_object *,
		double);

	void *alloc(size_t, char *, char *);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct beetle_object **beetle_grid; // the temperature mortality and elevation
	struct patch_beetle_object **patch_beetle_grid; // number of patches
	struct patch_object *patch;
	struct canopy_strata_object *strata;
	struct canopy_strata_object *canopy_strata;
	struct mortality_struct mort;
	int i,j,p, c, layer;
	int thin_type;
	double attack_mortality;
	double mean_Tfall =0;
	double mean_Tss =0;
	double mean_Tmin =0;
	double mean_Precip_wy =0;
	double denom_for_mean=0;

	patch_beetle_grid=world[0].patch_beetle_grid;
	beetle_grid = world[0].beetle_grid;

	/*--------------------------------------------------------------*/
	/* update beetle grid variables			*/
	/* first reset the values				*/
	/*--------------------------------------------------------------*/
	for  (i=0; i< world[0].num_beetle_grid_row; i++) {
  	  for (j=0; j < world[0].num_beetle_grid_col; j++) {
		if(world[0].patch_beetle_grid[i][j].occupied_area==0)
		{
			  if(world[0].defaults[0].beetle[0].beetle_in_buffer==0)
			  {
				    world[0].beetle_grid[i][j].mort = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and beetle doesn't spread
				    world[0].beetle_grid[i][j].Tfall = 0.0;
				    world[0].beetle_grid[i][j].Tss = 0.0;
				    world[0].beetle_grid[i][j].Tmin = 0.0;
				    world[0].beetle_grid[i][j].Precip_wy = 0.0;
                    world[0].beetle_grid[i][j].abc = 0.0;  // abc is the above ground carbon to represent the stand structure


				  printf("No beetle in buffer\n");
			  }
			  else // if denom_for_mean==0, then this initializes the buffer, otherwise the mean is filled in below
			  {
			        world[0].beetle_grid[i][j].mort = 0.0;
				    world[0].beetle_grid[i][j].Tfall = 0.0;
				    world[0].beetle_grid[i][j].Tss = 0.0;
				    world[0].beetle_grid[i][j].Tmin = 0.0;
				    world[0].beetle_grid[i][j].Precip_wy = 0.0;
				    world[0].beetle_grid[i][j].abc = 0.0;  // abc is the above ground carbon to represent the stand structure
			  }
		}
		else
		{
			        world[0].beetle_grid[i][j].mort = 0.0;
				    world[0].beetle_grid[i][j].Tfall = 0.0; // below for future couple the beetle outbreak model
				    world[0].beetle_grid[i][j].Tss = 0.0;
				    world[0].beetle_grid[i][j].Tmin = 0.0;
				    world[0].beetle_grid[i][j].Precip_wy = 0.0;
				    world[0].beetle_grid[i][j].abc = 0.0;  // abc is the above ground carbon to represent the stand structure
		}
	    printf("checking num patches. row %d col %d numPatches %d\n",i,j,patch_beetle_grid[i][j].num_patches);
		for (p=0; p < world[0].patch_beetle_grid[i][j].num_patches; ++p) {
			patch = world[0].patch_beetle_grid[i][j].patches[p];
			printf("Patch p: %d,\n",p);
            printf("pixel veg and prop patch in grid: %lf\n", patch_beetle_grid[i][j].prop_patch_in_grid[p]);

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					printf("Layers: %d\n c: %d\n, count: %d\n patch_num_layers %d\n",layer, c, patch[0].layers[layer].count, patch[0].num_layers);

                   //  patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];

					strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];


				/*world[0].beetle_grid[i][j].abc += (strata.cs.leafc + strata.cs.leafc_store + strata.cs.leafc_transfer +
                                                   strata.cs.live_stemc + strata.cs.livestemc_store + strata.cs.livestemc_transfer +
                                                   strata.cs.dead_stemc + strata.cs.deadstemc_store + strata.cs.deadstemc_transfer)*patch_beetle_grid[i][j].prop_patch_in_grid[p] ; */


				world[0].beetle_grid[i][j].abc += (strata[0].cs.leafc + strata[0].cs.leafc_store + strata[0].cs.leafc_transfer +
                                                  strata[0].cs.live_stemc + strata[0].cs.livestemc_store + strata[0].cs.livestemc_transfer +
                                                  strata[0].cs.dead_stemc + strata[0].cs.deadstemc_store + strata[0].cs.deadstemc_transfer)*patch_beetle_grid[i][j].prop_patch_in_grid[p] ;
				// above is calculate the total biomass in the beetle grid for higher outbreak
				   printf("the total above ground biomass in current patch are %lf\n", world[0].beetle_grid[i][j].abc);

				}
			}
		//	printf("pixel veg and prop patch in grid: %lf\n",world[0].beetle_grid[i][j].abc);



			world[0].beetle_grid[i][j].Tfall += patch[0].Tfall * world[0].patch_beetle_grid[i][j].prop_patch_in_grid[p];
			world[0].beetle_grid[i][j].Tss += patch[0].Tss * world[0].patch_beetle_grid[i][j].prop_patch_in_grid[p];
			world[0].beetle_grid[i][j].Tmin += patch[0].Tmin * world[0].patch_beetle_grid[i][j].prop_patch_in_grid[p];
			world[0].beetle_grid[i][j].Precip_wy += patch[0].Precip_wy * world[0].patch_beetle_grid[i][j].prop_patch_in_grid[p];

	//printf("patch Tfall, patch Tss, patch Tmin, patch precip_wy: %lf\t%lf\t%lf\t%lf\n",patch[0].beetle.Tfall, patch[0].beetle.Tss, patch[0].Tmin, patch[0].Precip_wy);

		}
		if(world[0].patch_beetle_grid[i][j].occupied_area>0&&world[0].defaults[0].beetle[0].beetle_in_buffer==1)
		{
			denom_for_mean+=1;

			mean_Tfall+=world[0].beetle_grid[i][j].Tfall;
			mean_Tss+=world[0].beetle_grid[i][j].Tss;
			mean_Tmin+=world[0].beetle_grid[i][j].Tmin;
			mean_Precip_wy+=world[0].beetle_grid[i][j].Precip_wy;
		//	printf("et: %f  pet: %f  ",world[0].beetle_grid[i][j].et,world[0].fire_grid[i][j].pet);
		}

    // when the beetle outbreak is last year august, we start the beetle attack current year first day january 1st

    if (current_date.year == world[0].defaults[0].beetle[0].year_attack) {

	world[0].beetle_grid[i][j].mort = world[0].defaults[0].beetle[0].attack_mortality; // the mortality at current point is specified from output define file
    world[0].beetle_grid[i][j].Yattack = current_date.year;
	}

	}
	}
//	printf("denom: %lf\t",denom_for_mean);
	if(denom_for_mean>0&&world[0].defaults[0].beetle[0].beetle_in_buffer==1)
	{
//		printf("in denom if\n");

		mean_Tfall=mean_Tfall/denom_for_mean;
		mean_Tss=mean_Tss/denom_for_mean;
		mean_Tmin=mean_Tmin/denom_for_mean;
		mean_Precip_wy=mean_Precip_wy/denom_for_mean;
	//	printf("mean Tfall: %f  mean Tss: %f  ",mean_Tfall,mean_Tss);

	//	printf("mean Tmin, mean Precip water year: %lf\t%lf\n",mean_Tmin,mean_Precip_wy);

		for  (i=0; i< world[0].num_beetle_grid_row; i++) {
		  for (j=0; j < world[0].num_beetle_grid_col; j++) {
			  if(world[0].patch_beetle_grid[i][j].occupied_area==0)
			  {

				world[0].beetle_grid[i][j].Tfall = mean_Tfall; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and eetle doesn't spread
				world[0].beetle_grid[i][j].Tss = mean_Tss;
				world[0].beetle_grid[i][j].Tmin= mean_Tmin;
				world[0].beetle_grid[i][j].Precip_wy = mean_Precip_wy;

	//	printf("in denom if take 2 update values\n");
			  }
		     }
		}
	}

	/*--------------------------------------------------------------*/
	/* now to pass these inputs to the beetle population model, which still under development					*/
	/*--------------------------------------------------------------*/
	printf("calling beetle outbreak model: month %ld year %ld  cell res %lf  nrow %d ncol % d\n",current_date.month,current_date.year,command_line[0].beetle_grid_res,world[0].num_beetle_grid_row,world[0].num_beetle_grid_col);

	/* build a simple mortality model  once the carbon is above  60Mg/ha is converted to 6000g/m2*/


	//if (world[0].beetle_grid[i][j].abc *1000 >=5500) {
   printf("the current date year is %d, and the default mortality is%lf \n", current_date.day, world[0].defaults[0].beetle[0].attack_mortality);



	//world[0].beetle_grid=WMFire(command_line[0].beetle_grid_res,world[0].num_beetle_grid_row,world[0].num_beetle_grid_col,current_date.year,current_date.month,world[0].beetle_grid,*(world[0].defaults[0].beetle));
 	printf("Finished calling outbreak model\n");
	/*--------------------------------------------------------------*/
	/* update biomass after beetle					*/
	/*--------------------------------------------------------------*/

	//thin_type =5;
	for  (i=0; i< world[0].num_beetle_grid_row; i++) {
  	  for (j=0; j < world[0].num_beetle_grid_col; j++) {
	    for (p=0; p < patch_beetle_grid[i][j].num_patches; ++p) {
			patch = world[0].patch_beetle_grid[i][j].patches[p];

//			printf("in update mortality\n");
			patch[0].mort = world[0].beetle_grid[i][j].mort * world[0].patch_beetle_grid[i][j].prop_grid_in_patch[p];
			attack_mortality = world[0].beetle_grid[i][j].mort * world[0].patch_beetle_grid[i][j].prop_grid_in_patch[p];
			/*printf("in update mortality, the mortality is %lf, and the prop_grid_in_patch is %lf\n",world[0].beetle_grid[i][j].mort, world[0].patch_beetle_grid[i][j].prop_grid_in_patch[p]  );*///N.R 20180619
            int inx=0; //the index of beetle attack squencies, may need to change
            int min_abc = world[0].defaults[0].beetle[0].min_abc;
            int root_alive = world[0].defaults[0].beetle[0].root_alive;
            int harvest_dead_root = world[0].defaults[0].beetle[0].harvest_dead_root;

            compute_beetle_effects(
						patch,
						inx,
						min_abc,
						root_alive,
						harvest_dead_root,
						attack_mortality);


			/*mort.mort_cpool = loss;
			mort.mort_leafc = loss;
			mort.mort_frootc = loss;
			mort.mort_deadstemc = loss;
			mort.mort_livestemc = loss;
			mort.mort_deadcrootc = loss;
			mort.mort_livecrootc = loss;

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					canopy_strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
					update_beetle_attack_mortality(canopy_strata[0].defaults[0][0].epc,
						 &(canopy_strata[0].cs),
						 &(canopy_strata[0].cdf),
						 &(patch[0].cdf),
						 &(canopy_strata[0].ns),
						 &(canopy_strata[0].ndf),
						 &(patch[0].ndf),
						 &(patch[0].litter_cs),
						 &(patch[0].litter_ns),
						 thin_type,
						 mort);
				}
			} */
//			printf("in update mortality3\n");

		}

		}
	}
printf("Finished updating mortality\n");



	return;
} /*end execute_beetlespread_event.c*/

