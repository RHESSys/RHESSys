/*--------------------------------------------------------------*/
/* 																*/
/*					construct_WUI_list					*/
/*																*/
/*	construct_WUI_list.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	construct_WUI_list.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object construct_WUI_list( 		*/
/*				char * WUI_filename, */
/*							struct world_object *world)			*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/* 	Creates a list of patches associated with WUI (and distance from WUI) */
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rhessys.h"

struct WUI_object *construct_WUI_list(
      char *WUI_filename,
      struct world_object *world,
      struct command_line_object  *command_line)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void *alloc(size_t, char *, char *);

	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i, h,z,pf,p,b;
	int fnd, n_WUI;
	int		line_n;
	int		patch_ID, WUI_ID, wui_dist;
	double dist;

        struct basin_object *basin;
        struct zone_object *zone;
        struct hillslope_object *hillslope;
        struct patch_object *patch;
        struct patch_family_object *patch_family;

	struct WUI_object *WUI_list;	
	struct WUI_object *WUI_ptr;	

	FILE *WUI_file;
 
	struct patch_object_list *patches_dist2km_ptr;
	struct patch_object_list *patches_dist5km_ptr;
	struct patch_object_list *patches_dist10km_ptr;


	/*--------------------------------------------------------------*/
        /*  Try to open the WUI file in read mode.                    */
        /*--------------------------------------------------------------*/
        if ( (WUI_file = fopen(WUI_filename,"r")) == NULL ){
                fprintf(stderr,"FATAL ERROR:  Cannot open WUI file %s\n",
                                WUI_filename);
                exit(0);
        } /*end if*/
	


	n_WUI = 0;

	/* read through file line by line */
	while (!feof(WUI_file)) {
	fscanf(WUI_file, "%d %d %d %lf %d",
			&line_n,
			&patch_ID,
			&WUI_ID,
			&dist,
			&wui_dist);


	/* have we created any WUI's yet?, if not create one */
	if (n_WUI == 0) {
	WUI_list = (struct WUI_object *) malloc(sizeof(struct WUI_object));
	WUI_list->ID = WUI_ID;
	n_WUI += 1;
	WUI_ptr = WUI_list;
	WUI_ptr->patches_dist2km = NULL;
	WUI_ptr->patches_dist5km = NULL;
	WUI_ptr->patches_dist10km = NULL;
	}
	else {
	/* try to an existing WUI  */
	fnd = 0;
	i = 0;
	WUI_ptr = WUI_list;
	while ((fnd==0) && (i < n_WUI)) {
		if (WUI_ptr->ID == WUI_ID)
			fnd=1;
		else {
			if (WUI_ptr->next==NULL)
				i = n_WUI;
			else {
				i=i+1;
				WUI_ptr = WUI_ptr->next;
				}
		}
	}
	}

	/* couldn't find an existing WUI so make a new one */
	if ((WUI_ptr->ID != WUI_ID)) {
		WUI_ptr->next = (struct WUI_object *)malloc(sizeof(struct WUI_object));
		WUI_ptr = WUI_ptr->next;
		WUI_ptr->ID = WUI_ID;
		WUI_ptr->patches_dist2km = NULL;
		WUI_ptr->patches_dist5km = NULL;
		WUI_ptr->patches_dist10km = NULL;
		n_WUI +=1;
		} 
	
		

	h=0; z=0; pf=0; p=0; b=0;
	fnd = 0;
	/* find the patch that this WUI is refering too */

         while ( (fnd == 0) && (b >= 0) && (b < world[0].num_basin_files)) { 
		basin = world[0].basins[b];		

         	while ( (fnd == 0) && (h >= 0) && (h < basin[0].num_hillslopes)) { 
                hillslope =  basin[0].hillslopes[h];

                while ( (fnd == 0) && (z >= 0) && (z <= hillslope[0].num_zones)) {
                zone = hillslope[0].zones[z];
                
        	/* are we looking for patch families or patches */
                if (command_line[0].multiscale_flag == 1) {
                while ( (fnd == 0) && (pf >= 0) && (pf <= zone[0].num_patch_families)) {
                patch_family = zone[0].patch_families[pf];

                if (patch_ID == patch_family[0].family_ID) {
                        fnd = 1;}
		else { pf += 1; }
                }
                }
                else { 
                while ( (fnd == 0) && (p >= 0) && (p <= zone[0].num_patches)) {
                        patch = zone[0].patches[p];
                        if (patch_ID == patch[0].ID) {
                        fnd = 1; }
			else { p += 1; }
                }}
		z += 1;
        }
		h += 1;
        }
		b += 1;
	}


	/* now add this to the appropriate patch list for the current WUI */
	if (wui_dist == 2) {

		/* first in this list so allocate room */
		if (WUI_ptr->patches_dist2km == NULL) {
			WUI_ptr->patches_dist2km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			if (command_line[0].multiscale_flag == 1) {
				WUI_ptr->patches_dist2km->patch = patch_family[0].patches[0];;
				patches_dist2km_ptr = WUI_ptr->patches_dist2km;
				/* add all the patches in the family */
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist2km_ptr = patches_dist2km_ptr->next;
					patches_dist2km_ptr->patch = patch_family[0].patches[i];
					patches_dist2km_ptr->next = NULL;
					}
			} 
			else {
				WUI_ptr->patches_dist2km->patch = patch;
				WUI_ptr->patches_dist2km->next = NULL;
				patches_dist2km_ptr = WUI_ptr->patches_dist2km;
			 }
		}
		/* list exists so add additional patches */
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist2km_ptr = patches_dist2km_ptr->next;
					patches_dist2km_ptr->patch = patch_family[0].patches[i];
					patches_dist2km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family */
			patches_dist2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_dist2km_ptr = patches_dist2km_ptr->next;
			patches_dist2km_ptr->patch = patch;
			patches_dist2km_ptr->next = NULL;
			}
			}
	}


	if (wui_dist == 5) {

		/* first in this list so allocate room */
		if (WUI_ptr->patches_dist5km == NULL) {
			WUI_ptr->patches_dist5km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			if (command_line[0].multiscale_flag == 1) {
				WUI_ptr->patches_dist5km->patch = patch_family[0].patches[0];;
				patches_dist5km_ptr = WUI_ptr->patches_dist5km;
				/* add all the patches in the family */
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist5km_ptr = patches_dist5km_ptr->next;
					patches_dist5km_ptr->patch = patch_family[0].patches[i];
					patches_dist5km_ptr->next = NULL;
					}
			} 
			else {
				WUI_ptr->patches_dist5km->patch = patch;
				WUI_ptr->patches_dist5km->next = NULL;
				patches_dist5km_ptr = WUI_ptr->patches_dist5km;
			 }
		}
		/* list exists so add additional patches */
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist5km_ptr = patches_dist5km_ptr->next;
					patches_dist5km_ptr->patch = patch_family[0].patches[i];
					patches_dist5km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family */
			patches_dist5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_dist5km_ptr = patches_dist5km_ptr->next;
			patches_dist5km_ptr->patch = patch;
			patches_dist5km_ptr->next = NULL;
			}
			}
	}


	if (wui_dist == 10) {

		/* first in this list so allocate room */
		if (WUI_ptr->patches_dist10km == NULL) {
			WUI_ptr->patches_dist10km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			if (command_line[0].multiscale_flag == 1) {
				WUI_ptr->patches_dist10km->patch = patch_family[0].patches[0];;
				patches_dist10km_ptr = WUI_ptr->patches_dist10km;
				/* add all the patches in the family */
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist10km_ptr = patches_dist10km_ptr->next;
					patches_dist10km_ptr->patch = patch_family[0].patches[i];
					patches_dist10km_ptr->next = NULL;
					}
			} 
			else {
				WUI_ptr->patches_dist10km->patch = patch;
				WUI_ptr->patches_dist10km->next = NULL;
				patches_dist10km_ptr = WUI_ptr->patches_dist10km;
			 }
		}
		/* list exists so add additional patches */
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_dist10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist10km_ptr = patches_dist10km_ptr->next;
					patches_dist10km_ptr->patch = patch_family[0].patches[i];
					patches_dist10km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family */
			patches_dist10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_dist10km_ptr = patches_dist10km_ptr->next;
			patches_dist10km_ptr->patch = patch;
			patches_dist10km_ptr->next = NULL;
			}
			}
	}
		
}


/* echo back */

WUI_ptr = WUI_list;
while(WUI_ptr != NULL) {
	printf("\n For WUI %d", WUI_ptr->ID);
	patches_dist2km_ptr = WUI_ptr->patches_dist2km;
	while(patches_dist2km_ptr != NULL) {
		printf("\n	we have at 2km %d",patches_dist2km_ptr->patch[0].ID);
		patches_dist2km_ptr = patches_dist2km_ptr->next;
		}
	patches_dist5km_ptr = WUI_ptr->patches_dist5km;
	while(patches_dist5km_ptr != NULL) {
		printf("\n	we have at 5km %d",patches_dist5km_ptr->patch[0].ID);
		patches_dist5km_ptr = patches_dist5km_ptr->next;
		}
	patches_dist10km_ptr = WUI_ptr->patches_dist10km;
	while(patches_dist10km_ptr != NULL) {
		printf("\n	we have at 10km %d",patches_dist10km_ptr->patch[0].ID);
		patches_dist10km_ptr = patches_dist10km_ptr->next;
		}
	WUI_ptr = WUI_ptr->next;
	}



	return(WUI_list);
} /*end construct_WUI_list.c*/

