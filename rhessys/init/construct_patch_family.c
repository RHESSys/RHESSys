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
    int     patch_family_ID,
    struct 	command_line_object *command_line)
{

/*--------------------------------------------------------------*/
/*	Local function definition.				                    */
/*--------------------------------------------------------------*/

void	*alloc(size_t, char *, char *);

/*--------------------------------------------------------------*/
/*	Local variable definition.			                    	*/
/*--------------------------------------------------------------*/
int i, count;

struct patch_family_object *patch_family;

/*--------------------------------------------------------------*/
/*  Allocate a patch family object(s)                           */
/*--------------------------------------------------------------*/
patch_family = (struct patch_family_object *) alloc( 1 * sizeof( struct patch_family_object ),"patch_family","construct_patch_family");

/*--------------------------------------------------------------*/
/*	Loop through patches           	                        	*/
/*--------------------------------------------------------------*/

//set family ID for family obj, this could happen outside of function but whatever
patch_family[0].family_ID = patch_family_ID;

patch_family[0].num_patches_in_fam = 0;
for (i = 0; i < zone[0].num_patches; i++)
{
    if(zone[0].patches[i][0].family_ID == patch_family_ID) patch_family[0].num_patches_in_fam++;
}

if (command_line[0].verbose_flag == -6) printf("%d patches in patch family %d \n",patch_family[0].num_patches_in_fam, patch_family[0].family_ID);

/*--------------------------------------------------------------*/
/*	Allocate pointers to patches   	                        	*/
/*--------------------------------------------------------------*/

patch_family[0].patches = (struct patch_object ** ) 
		alloc( patch_family[0].num_patches_in_fam * sizeof( struct patch_object *),
		"patches","construct_patch_family");

// finally actually point to the correct patches
count = 0;
for (i = 0; i < zone[0].num_patches; i++)
{
    if(zone[0].patches[i][0].family_ID == patch_family_ID)
    {
        patch_family[0].patches[count] = zone[0].patches[i];
        if (command_line[0].verbose_flag == -6) printf("%d ", patch_family[0].patches[count][0].ID);

        patch_family[0].area += zone[0].patches[i][0].area;

        count++;
    }
}
if (command_line[0].verbose_flag == -6) printf("\n");

// loop to add the family_pct_cover to patch object - assumes patch family area is not 0
for (i = 0; i < patch_family[0].num_patches_in_fam ; i++)
{
    patch_family[0].patches[i][0].family_pct_cover = patch_family[0].patches[i][0].area / patch_family[0].area;
}

return(patch_family);

}
