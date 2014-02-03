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

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_object *patch;
	struct canopy_strata_object *canopy_strata;
	struct mortality_struct mort;
	int i,j,p, c, layer; 
	int thin_type;
	double loss;


	fire_grid = world[0].fire_grid;

	/*--------------------------------------------------------------*/
	/* update fire grid variables					*/
	/*--------------------------------------------------------------*/

	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  	  for (j=0; j < world[0].num_fire_grid_col; j++) {
	    fire_grid[i][j].fuel_veg = 0.0;
	    fire_grid[i][j].fuel_litter = 0.0;
	    fire_grid[i][j].fuel_moist = 0.0;
	    fire_grid[i][j].soil_moist = 0.0;
	    fire_grid[i][j].relative_humidity = 0.0;
	    fire_grid[i][j].wind_direction = 0.0;
	    fire_grid[i][j].wind = 0.0;
	    for (p=0; p < fire_grid[i][j].num_patches; ++p) {
			patch = fire_grid[i][j].patches[p];
			fire_grid[i][j].fuel_litter += (patch[0].litter_cs.litr1c +
				patch[0].litter_cs.litr2c +	
				patch[0].litter_cs.litr3c +	
				patch[0].litter_cs.litr4c) * fire_grid[i][j].prop_patch_in_grid[p];
			fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
						fire_grid[i][j].prop_patch_in_grid[p];

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
				fire_grid[i][j].fuel_veg += (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc) *
						fire_grid[i][j].prop_patch_in_grid[p] ;
				}
			}

			fire_grid[i][j].soil_moist += patch[0].rootzone.S * fire_grid[i][j].prop_patch_in_grid[p];	

			fire_grid[i][j].wind += patch[0].zone[0].wind * fire_grid[i][j].prop_patch_in_grid[p];
			fire_grid[i][j].wind_direction += patch[0].zone[0].wind_direction * fire_grid[i][j].prop_patch_in_grid[p];
			fire_grid[i][j].relative_humidity += patch[0].zone[0].relative_humidity * fire_grid[i][j].prop_patch_in_grid[p];
			fire_grid[i][j].et += patch[0].fire.et * fire_grid[i][j].prop_patch_in_grid[p];
			fire_grid[i][j].pet += patch[0].fire.pet * fire_grid[i][j].prop_patch_in_grid[p];
		}
			
	}
	}
				
	/*--------------------------------------------------------------*/
	/* maureens stuff here 						*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/* update biomass after fire					*/
	/*--------------------------------------------------------------*/

	thin_type =2;
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  	  for (j=0; j < world[0].num_fire_grid_col; j++) {
	    for (p=0; p < fire_grid[i][j].num_patches; ++p) {
			patch = fire_grid[i][j].patches[p];

			
			patch[0].burn = fire_grid[i][j].burn * fire_grid[i][j].prop_grid_in_patch[p];
			loss = fire_grid[i][j].burn * fire_grid[i][j].prop_grid_in_patch[p];

			mort.mort_cpool = loss;
			mort.mort_leafc = loss;
			mort.mort_frootc = loss;
			mort.mort_deadstemc = loss;
			mort.mort_livestemc = loss;
			mort.mort_deadcrootc = loss;
			mort.mort_livecrootc = loss;

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					canopy_strata = patch[0].layers[layer].strata[c];
					update_mortality(canopy_strata[0].defaults[0][0].epc,
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
			}

		}
			
	}
	}


		

	return;
} /*end execute_firespread_event.c*/
