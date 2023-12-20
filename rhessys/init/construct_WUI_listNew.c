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
printf("in construct_wui\n");
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
	int		patch_ID, WUI_ID, wui_dist,trt_ord2,trt_ord5,trt_ord10;
	int stop_flag;
	double dist;

        struct basin_object *basin;
        struct zone_object *zone;
        struct hillslope_object *hillslope;
        struct patch_object *patch;
        struct patch_family_object *patch_family;
printf("loc 1\n");
	struct WUI_object *WUI_list;	
	struct WUI_object *WUI_ptr;	
	struct wui_dist_list *prev_wui_dist_list_ptr;
	struct wui_dist_list *next_wui_dist_list_ptr;
	FILE *WUI_file;
	struct patch_object_list *patches_trt2km_ptr;
	struct patch_object_list *patches_trt5km_ptr;
	struct patch_object_list *patches_trt10km_ptr;
	/*--------------------------------------------------------------*/
        /*  Try to open the WUI file in read mode.                    */
        /*--------------------------------------------------------------*/
        if ( (WUI_file = fopen(WUI_filename,"r")) == NULL ){
                fprintf(stderr,"FATAL ERROR:  Cannot open WUI file %s\n",
                                WUI_filename);
                exit(0);
        } /*end if*/
	


	n_WUI = 0;
printf("nWUI %d\n",n_WUI);
	/* read through file line by line */
	while (!feof(WUI_file)) {
	trt_ord2=-1;
	trt_ord5=-1;
	trt_ord10=-1; // initialize treatment order as -1
printf("reading in file\n");
	fscanf(WUI_file, "%d %d %d %d %d %d %d",
			&line_n,
			&patch_ID,
			&WUI_ID,
			&wui_dist,
			&trt_ord2,
			&trt_ord5,
			&trt_ord10);
sleep(5);
printf("\nIn read WUI file %d %d %d %d %d %d %d \n",
                        line_n,
                        patch_ID,
                        WUI_ID,
                        wui_dist,
                        trt_ord2,
                        trt_ord5,
                        trt_ord10);
	/* have we created any WUI's yet?, if not create one */
	if (n_WUI == 0) {
printf("nWUI %d\n",n_WUI);
	WUI_list = (struct WUI_object *) malloc(sizeof(struct WUI_object));
	WUI_list->ID = WUI_ID;
	n_WUI += 1;
	WUI_ptr = WUI_list;
	WUI_ptr->patches_trt2km = NULL;
	WUI_ptr->patches_trt5km = NULL;
	WUI_ptr->patches_trt10km = NULL;
	}
	else {
	/* try to find an existing WUI  */
	printf("loc1a\n");
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
	
printf("loc1b ");

	/* couldn't find an existing WUI so make a new one */
	if ((WUI_ptr->ID != WUI_ID)) {
		WUI_ptr->next = (struct WUI_object *)malloc(sizeof(struct WUI_object));
		WUI_ptr = WUI_ptr->next;
		WUI_ptr->ID = WUI_ID;
		WUI_ptr->patches_trt2km = NULL;
		WUI_ptr->patches_trt5km = NULL;
		WUI_ptr->patches_trt10km = NULL;
		n_WUI +=1;
		} 
	
	}

	h=0; z=0; pf=0; p=0; b=0;
	fnd = 0;
	/* find the patch that this WUI is refering too */
// this is actually looking for the patch family. we're going to orient the LUT to an
// individual patch within the family, so the patchID is appropriate. I think we can use find_patch
printf("loc3 ");

         while ( (fnd == 0) && (b >= 0) && (b < world[0].num_basin_files)) { 
		basin = world[0].basins[b];		
        printf("WUI1 %d\n", WUI_ptr->ID);
 
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
printf("patch family %d",patch_family[0].family_ID);


	/* now add this to the appropriate patch list for the current WUI */
	if (trt_ord2 > 0) {//if available for treatment for a 2 km salience event for this WUI
		/* first in this list so allocate room */
		if (WUI_ptr->patches_trt2km == NULL) {

			WUI_ptr->patches_trt2km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
patch=patch_family[0].patches[0];
			if (command_line[0].multiscale_flag == 1) {
printf("wui: %d, patch: %d",WUI_ptr->ID,patch[0].ID);
				WUI_ptr->patches_trt2km->patch = patch_family[0].patches[0];
sleep(5);
printf("loc4\n");

				patches_trt2km_ptr = WUI_ptr->patches_trt2km;
				/* add all the patches in the family */
printf("loc4a ");
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt2km_ptr = patches_trt2km_ptr->next;
					patches_trt2km_ptr->patch = patch_family[0].patches[i];
					patches_trt2km_ptr->next = NULL;
					}
printf("loc4b ");
			} 
			else {
				WUI_ptr->patches_trt2km->patch = patch;
				WUI_ptr->patches_trt2km->next = NULL;
				patches_trt2km_ptr = WUI_ptr->patches_trt2km;
			 }
		}
		/* list exists so add additional patches */
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt2km_ptr = patches_trt2km_ptr->next;
					patches_trt2km_ptr->patch = patch_family[0].patches[i];
					patches_trt2km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family */
			patches_trt2km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_trt2km_ptr = patches_trt2km_ptr->next;
			patches_trt2km_ptr->patch = patch;
			patches_trt2km_ptr->next = NULL;
			}
			}
	}
printf("loc5 ");

//	if (trt_ord5 > 0) { //if available for treatment for a 10 km salience event for this WUI

		/* first in this list so allocate room *
		if (WUI_ptr->patches_trt5km == NULL) {
			WUI_ptr->patches_trt5km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			if (command_line[0].multiscale_flag == 1) {
				WUI_ptr->patches_trt5km->patch = patch_family[0].patches[0];;
				patches_trt5km_ptr = WUI_ptr->patches_trt5km;
				/* add all the patches in the family *
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt5km_ptr = patches_trt5km_ptr->next;
					patches_trt5km_ptr->patch = patch_family[0].patches[i];
					patches_trt5km_ptr->next = NULL;
					}
			} 
			else {
				WUI_ptr->patches_trt5km->patch = patch;
				WUI_ptr->patches_trt5km->next = NULL;
				patches_trt5km_ptr = WUI_ptr->patches_trt5km;
			 }
		}
		/* list exists so add additional patches *
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt5km_ptr = patches_trt5km_ptr->next;
					patches_trt5km_ptr->patch = patch_family[0].patches[i];
					patches_trt5km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family *
			patches_trt5km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_trt5km_ptr = patches_trt5km_ptr->next;
			patches_trt5km_ptr->patch = patch;
			patches_trt5km_ptr->next = NULL;
			}
			}
	}
printf("loc6\n");

	if (trt_ord10 > 0) { //if available for treatment for a 10 km salience event for this WUI

		/* first in this list so allocate room *
		if (WUI_ptr->patches_trt10km == NULL) {
			WUI_ptr->patches_trt10km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			if (command_line[0].multiscale_flag == 1) {
				WUI_ptr->patches_trt10km->patch = patch_family[0].patches[0];;
				patches_trt10km_ptr = WUI_ptr->patches_trt10km;
				/* add all the patches in the family *
				for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt10km_ptr = patches_trt10km_ptr->next;
					patches_trt10km_ptr->patch = patch_family[0].patches[i];
					patches_trt10km_ptr->next = NULL;
					}
			} 
			else {
				WUI_ptr->patches_trt10km->patch = patch;
				WUI_ptr->patches_trt10km->next = NULL;
				patches_trt10km_ptr = WUI_ptr->patches_trt10km;
			 }
		}
		/* list exists so add additional patches *
		else {
			if (command_line[0].multiscale_flag == 1) {
				for (i=0; i < patch_family[0].num_patches_in_fam; i++) {
					patches_trt10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_trt10km_ptr = patches_trt10km_ptr->next;
					patches_trt10km_ptr->patch = patch_family[0].patches[i];
					patches_trt10km_ptr->next = NULL;
					}
			}
			else {
			/* add all the patches in the family *
			patches_trt10km_ptr->next = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_trt10km_ptr = patches_trt10km_ptr->next;
			patches_trt10km_ptr->patch = patch;
			patches_trt10km_ptr->next = NULL;
			}
		}
		}
		
//	}
printf("loc7\n");
	/* and now update the wuiDist for this patch for this wui*
	if(patch_family[0].patches[0]->wui_dist==NULL)// first wui for this patch family
	{
		patch_family[0].patches[0]->wui_dist=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
		patch_family[0].patches[0]->wui_dist->dist=wui_dist;
		 patch_family[0].patches[0]->wui_dist->wui_id=WUI_ID;
		patch_family[0].patches[0]->wui_dist->next=NULL;
		 patch_family[0].patches[0]->wui_dist->prev=NULL;
		for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
		        patch_family[0].patches[i]->wui_dist=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
        	        patch_family[0].patches[i]->wui_dist->dist=wui_dist;
                 	patch_family[0].patches[i]->wui_dist->wui_id=WUI_ID;
 	                patch_family[0].patches[i]->wui_dist->next=NULL;
			patch_family[0].patches[i]->wui_dist->prev=NULL;

		}

	}
	else // find the end of the list and make a new entry. Make sure it's in the right order
	{
		prev_wui_dist_list_ptr=patch_family[0].patches[0]->wui_dist;
		stop_flag=1; // initialize as 1
		if(prev_wui_dist_list_ptr->next!=NULL)
		{
			if(prev_wui_dist_list_ptr->wui_id<WUI_ID)
			{
				stop_flag=0; // then find its place in the list
			}
			else
			{
				stop_flag=2; // then we have to insert the new one before this one in the list
			}
		}

//		while(prev_wui_dist_list_ptr->next!=NULL)
		while(stop_flag==0)
		{
			if(prev_wui_dist_list_ptr->next==NULL)
			{
				stop_flag=1;
			}
			else
			{
				prev_wui_dist_list_ptr=prev_wui_dist_list_ptr->next;
				if(prev_wui_dist_list_ptr->wui_id>WUI_ID)
				{
					stop_flag=2;
				}
			}
		}
		next_wui_dist_list_ptr=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
                next_wui_dist_list_ptr->dist=wui_dist;
                next_wui_dist_list_ptr->wui_id=WUI_ID;
     // Now are we putting it at the end of the list (stop_flag=1), or do we have to insert it?
     		if(stop_flag==1) // put it at the end of the list
		{
			next_wui_dist_list_ptr->next=NULL;
			next_wui_dist_list_ptr->prev= prev_wui_dist_list_ptr;
			prev_wui_dist_list_ptr->next= next_wui_dist_list_ptr;
		}
		else // insert it between the prev-->prev and the prev
		{
			if(prev_wui_dist_list_ptr->prev!=NULL) //not the first in the list
			{
				 prev_wui_dist_list_ptr->prev->next= next_wui_dist_list_ptr; // then point its previous one to this one
				 next_wui_dist_list_ptr->prev= prev_wui_dist_list_ptr->prev; // and point this one to it backwards
			}
			else // prev is first in list, so make this one first
			{
				 next_wui_dist_list_ptr->prev=NULL;
			}
			 prev_wui_dist_list_ptr->prev= next_wui_dist_list_ptr;// regardless, this one goes before the prev one and don't change the next
			 next_wui_dist_list_ptr->next= prev_wui_dist_list_ptr;
		}
                for (i=1; i < patch_family[0].num_patches_in_fam; i++) {
	                prev_wui_dist_list_ptr=patch_family[0].patches[i]->wui_dist;
	                stop_flag=1; // initialize as 1
        	        if(prev_wui_dist_list_ptr->next!=NULL)
                	{
                        	if(prev_wui_dist_list_ptr->wui_id<WUI_ID)
	                        {
        	                        stop_flag=0; // then find its place in the list
                	        }
	                        else
        	                {
                	                stop_flag=2; // then we have to insert the new one before this one in the list
                        	}
	                }
	                while(stop_flag==0)
        	        {
                	        if(prev_wui_dist_list_ptr->next==NULL)
                       		{
                                	stop_flag=1;
	                        }
        	                else
                	        {
                        	        prev_wui_dist_list_ptr=prev_wui_dist_list_ptr->next;
	                                if(prev_wui_dist_list_ptr->wui_id>WUI_ID)
        	                        {
                	                        stop_flag=2;
                        	        }
	                        }
        	        }
	                next_wui_dist_list_ptr=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
        	        next_wui_dist_list_ptr->dist=wui_dist;
                	next_wui_dist_list_ptr->wui_id=WUI_ID;

 	              if(stop_flag==1) // put it at the end of the list
        	        {
	                        next_wui_dist_list_ptr->next=NULL;
        	                next_wui_dist_list_ptr->prev= prev_wui_dist_list_ptr;
                	        prev_wui_dist_list_ptr->next= next_wui_dist_list_ptr;
	                }
        	        else // insert it between the prev-->prev and the prev
                	{
	                        if(prev_wui_dist_list_ptr->prev!=NULL) //not the first in the list
        	                {
                	                 prev_wui_dist_list_ptr->prev->next= next_wui_dist_list_ptr; // then point its previous one to this one
                        	         next_wui_dist_list_ptr->prev= prev_wui_dist_list_ptr->prev; // and point this one to it backwards
	                        }
        	                else // prev is first in list, so make this one first
                	        {
                        	         next_wui_dist_list_ptr->prev=NULL;
	                        }
        	                 prev_wui_dist_list_ptr->prev= next_wui_dist_list_ptr;// regardless, this one goes before the prev one and don't change the next
                	         next_wui_dist_list_ptr->next= prev_wui_dist_list_ptr;
	                }
 


	            /*    while(prev_wui_dist_list_ptr->next!=NULL)
        	        {
                	        prev_wui_dist_list_ptr=wui_dist_list_ptr->next;
	                }
			
        	        next_wui_dist_list_ptr=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
			next_wui_dist_list_ptr->dist=wui_dist;
			next_wui_dist_list_ptr->wui_id=WUI_ID; //wui_ID to ensure the order is correct
                	next_wui_dist_list_ptr->next=NULL;
			next_wui_dist_list_ptr->prev= prev_wui_dist_list_ptr;*/
			
//		}
//	}
	}// end file loop
fclose(WUI_file);
printf("Closed file\n");
/* echo back */

/*	WUI_ptr = WUI_list;
	while(WUI_ptr != NULL) {
	printf("\n For WUI %d", WUI_ptr->ID);
	patches_trt2km_ptr = WUI_ptr->patches_trt2km;
	while(patches_trt2km_ptr != NULL) {
		printf("\n	we have at 2km %d",patches_trt2km_ptr->patch[0].ID);
		patches_trt2km_ptr = patches_trt2km_ptr->next;
		}
	patches_trt5km_ptr = WUI_ptr->patches_trt5km;
	while(patches_trt5km_ptr != NULL) {
		printf("\n	we have at 5km %d",patches_trt5km_ptr->patch[0].ID);
		patches_trt5km_ptr = patches_trt5km_ptr->next;
		}
	patches_trt10km_ptr = WUI_ptr->patches_trt10km;
	while(patches_trt10km_ptr != NULL) {
		printf("\n	we have at 10km %d",patches_trt10km_ptr->patch[0].ID);
		patches_trt10km_ptr = patches_trt10km_ptr->next;
		}
	WUI_ptr = WUI_ptr->next;
	}

*/

	return(WUI_list);
} /*end construct_WUI_list.c*/
