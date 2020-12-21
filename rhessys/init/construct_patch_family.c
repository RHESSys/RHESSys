/*--------------------------------------------------------------*/
/*						                                		*/
/*		    construct_patch_family  		                   	*/
/*							                                   	*/
/*	construct_patch_family.c - creates a patch family object    */
/*							                                	*/
/*	NAME					                            		*/
/*	construct_patch_family.c - creates a patch family object    */
/*							                                	*/
/*	SYNOPSIS					                            	*/
/*	struct patch_family_object construct_patch_family(          */
/*                          struct zone_object *zone,           */
/*                          int     patch_family_ID)            */
/*										                       	*/
/*	OPTIONS								                   		*/
/*										                       	*/
/*										                       	*/
/*	DESCRIPTION								                   	*/
/*	Create a patch_family_object structure which points to 		*/
/*	the patch objects within it (defined by patch family IDs  	*/
/*	set in preprocessing). Structure also includes the family  	*/
/*	ID and number of patches in that family.                   	*/
/*                                                              */
/*	PROGRAMMER NOTES							               	*/
/*										                       	*/
/*	July, 2019 William Burke						           	*/
/*	                     	                                	*/
/*											                    */
/*--------------------------------------------------------------*/

// includes
#include <stdio.h>
#include "rhessys.h"
//#include "params.h"

struct patch_family_object *construct_patch_family(
    struct zone_object *zone,
    int patch_family_ID,
    struct command_line_object *command_line)
{

    /*--------------------------------------------------------------*/
    /*	Local function definition.				                    */
    /*--------------------------------------------------------------*/
    void *alloc(size_t, char *, char *);

    /*--------------------------------------------------------------*/
    /*	Local variable definition.			                    	*/
    /*--------------------------------------------------------------*/
    int i, j, count, cs_count, rec, max_under_loc, min_over_loc;
    struct patch_family_object *patch_family;

    /*--------------------------------------------------------------*/
    /*  Allocate a patch family object(s)                           */
    /*--------------------------------------------------------------*/
    patch_family = (struct patch_family_object *)alloc(1 * sizeof(struct patch_family_object), "patch_family", "construct_patch_family");

    /*--------------------------------------------------------------*/
    /*	Loop through patches           	                        	*/
    /*--------------------------------------------------------------*/

    //set family ID for family obj, this could happen outside of function but whatever
    patch_family[0].family_ID = patch_family_ID;

    patch_family[0].num_patches_in_fam = 0;
    for (i = 0; i < zone[0].num_patches; i++)
    {
        if (zone[0].patches[i][0].family_ID == patch_family_ID)
            patch_family[0].num_patches_in_fam++;
    }

    if (command_line[0].verbose_flag == -6)
        printf("%d patches in patch family %d \n", patch_family[0].num_patches_in_fam, patch_family[0].family_ID);

    /*--------------------------------------------------------------*/
    /*	Allocate pointers to patches   	                        	*/
    /*--------------------------------------------------------------*/

    patch_family[0].patches = (struct patch_object **)
        alloc(patch_family[0].num_patches_in_fam * sizeof(struct patch_object *),
              "patches", "construct_patch_family");

    // finally actually point to the correct patches
    count = 0;
    patch_family[0].num_canopy_strata = 0;
    for (i = 0; i < zone[0].num_patches; i++)
    {
        if (zone[0].patches[i][0].family_ID == patch_family_ID)
        {
            patch_family[0].patches[count] = zone[0].patches[i];
            if (command_line[0].verbose_flag == -6)
                printf("%d ", patch_family[0].patches[count][0].ID);

            patch_family[0].area += zone[0].patches[i][0].area;
            patch_family[0].num_canopy_strata += zone[0].patches[i][0].num_canopy_strata;

            count++;
        }
    }
    if (command_line[0].verbose_flag == -6)
        printf("\n");

    patch_family[0].canopy_strata = (struct canopy_strata_object **)
        alloc(patch_family[0].num_canopy_strata * sizeof(struct canopy_strata_object *),
              "canopy_strata", "construct_patch_family");
    
    // to track which strata belong to which patch - for compute fire effects
    // int strata_patch_loc[patch_family[0].num_canopy_strata];

    
    // loop to add the family_pct_cover to patch object - assumes patch family area is not 0 
    cs_count = 0;
    max_under_loc = 0;
    min_over_loc = 0;

    for (i = 0; i < patch_family[0].num_patches_in_fam; i++)
    {
        patch_family[0].patches[i][0].family_pct_cover = patch_family[0].patches[i][0].area / patch_family[0].area;
        for (j = 0; j < patch_family[0].patches[i][0].num_canopy_strata; j++)
        {
            patch_family[0].canopy_strata[cs_count] = patch_family[0].patches[i][0].canopy_strata[j];
            patch_family[0].canopy_strata[cs_count][0].fam_patch_ind = i;
            //strata_patch_loc[cs_count] = i;
            cs_count++;
        }

        if (command_line[0].firespread_flag == 1)
        {
            if (patch_family[0].patches[i][0].soil_defaults[0][0].understory_height_thresh > 
            patch_family[0].patches[max_under_loc][0].soil_defaults[0][0].understory_height_thresh)
                max_under_loc = i;
            if (patch_family[0].patches[i][0].soil_defaults[0][0].overstory_height_thresh < 
            patch_family[0].patches[min_over_loc][0].soil_defaults[0][0].overstory_height_thresh)
                min_over_loc = i;
        }
    }

    patch_family[0].understory_height_thresh = patch_family[0].patches[max_under_loc][0].soil_defaults[0][0].understory_height_thresh;    
    patch_family[0].overstory_height_thresh = patch_family[0].patches[min_over_loc][0].soil_defaults[0][0].overstory_height_thresh;
    //patch_family[0].strata_patch_index = strata_patch_loc;

    /*--------------------------------------------------------------*/
    /*	Define a list of canopy strata layers that can at least	*/
    /*	fit all of the canopy strata.				*/
    /*--------------------------------------------------------------*/
    patch_family[0].layers = (struct layer_object *)alloc(patch_family[0].num_canopy_strata *
                                                              sizeof(struct layer_object),
                                                          "layers", "construct_patch_family");
    patch_family[0].num_layers = 0;
    rec = 0;
    sort_patch_family_layers(patch_family, &rec);

    return (patch_family);
}
