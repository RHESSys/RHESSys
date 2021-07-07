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
printf("just entered construct wui list\n");
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void *alloc(size_t, char *, char *);

	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i, h,z,pf,p,b;
	int fnd, n_WUI,stop_flag;
	int		line_n;
	int		family_ID,patch_ID, WUI_ID, wui_dist,trt_ord2,trt_ord5,trt_ord10,trt_ord100;
	int iter2km=0,iter5km=0,iter10km=0;
	double dist;

        struct basin_object *basin;
        struct zone_object *zone;
        struct hillslope_object *hillslope;
        struct patch_object *patch;
        struct patch_family_object *patch_family;

	struct WUI_object *WUI_list;	
	struct WUI_object *WUI_ptr;	
	struct WUI_object *new_WUI_ptr;
	FILE *WUI_file;

	struct wui_dist_list *prev_wui_dist_list_ptr;
	struct wui_dist_list *next_wui_dist_list_ptr;
 
	struct patch_object_list *patches_dist2km_ptr;
	struct patch_object_list *patches_dist5km_ptr;
	struct patch_object_list *patches_dist10km_ptr;
        struct patch_object_list *patches_dist100km_ptr;


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
		fscanf(WUI_file, "%d %d %d %d %d %d %d %d %d\n",
			&line_n,
			&family_ID,
			&patch_ID,
			&WUI_ID,
			&wui_dist,
			&trt_ord2,
			&trt_ord5,
			&trt_ord10,
			&trt_ord100);
printf("\nIn read WUI file %d %d %d %d %d %d %d %d %d \n",
                        line_n,
			family_ID,
                        patch_ID,
                        WUI_ID,
                        wui_dist,
                        trt_ord2,
                        trt_ord5,
                        trt_ord10,trt_ord100);


	/* have we created any WUI's yet?, if not create one */
	// create WUI 0, then this WUI
	if (n_WUI == 0) {
	        WUI_list = (struct WUI_object *) malloc(sizeof(struct WUI_object));
        	WUI_list->ID = 0;// WUI 0 for background treatment order
	        n_WUI += 1;
	        WUI_ptr = WUI_list;
	        WUI_ptr->patches_dist2km = NULL;
	        WUI_ptr->patches_dist5km = NULL;
	        WUI_ptr->patches_dist10km = NULL;
	        WUI_ptr->fire_occurence=100;// initialize salience event as 100, replace with new salience distance as triggered
	        WUI_ptr->ntrt[0]=0;// tally of triggered salience events for this WUI, array of 3 (one for each dist). Initialize all with zero
	        WUI_ptr->ntrt[1]=0;
	        WUI_ptr->ntrt[2]=0;
	        WUI_ptr->next=(struct WUI_object *) malloc(sizeof(struct WUI_object));// now the first official WUI
		WUI_ptr->prev=NULL;
		
		WUI_ptr= WUI_ptr->next;
		WUI_ptr->ID = WUI_ID;
		n_WUI += 1;
		WUI_ptr->patches_dist2km = NULL;
		WUI_ptr->patches_dist5km = NULL;
		WUI_ptr->patches_dist10km = NULL;
		WUI_ptr->fire_occurence=100;// initialize salience event as 100, replace with new salience distance as triggered
	        WUI_ptr->ntrt[0]=0;// tally of triggered salience events for this WUI, array of 3 (one for each dist). Initialize all with zero
	        WUI_ptr->ntrt[1]=0;
	        WUI_ptr->ntrt[2]=0;
		WUI_ptr->next=NULL;
		WUI_ptr->prev=WUI_list; // point it back to the beginning of the list

	}
	else {
	/* try to find an existing WUI  */
	fnd = 0;
	i = 0;
	WUI_ptr = WUI_list;// start at the beginning of the list
	while ((fnd==0) && (i < n_WUI)) {
		if (WUI_ptr->ID == WUI_ID)
		{
			fnd=1;// stop and add to this WUI
			printf("found an existing WUI, stay here! ID %d\n",WUI_ptr->ID);
		}
		else {
			if (WUI_ptr->next==NULL)// stop because we have reached the end of the list
				i = n_WUI;
			else {// otherwise keep looking
				if(WUI_ptr->ID>WUI_ID) // the WUI should be entered before this one
				{
					fnd=1;// stop and add previous to this WUI
				}
				else{
					i=i+1;
					WUI_ptr = WUI_ptr->next;
					}
				}
		}
	}
//	}

	/* couldn't find an existing WUI so make a new one. otherwise WUI_ptr is the correct WUI */
	if ((WUI_ptr->ID != WUI_ID)) {
		if(WUI_ID>WUI_ptr->ID)// then add it to the end of the list
		{
			WUI_ptr->next = (struct WUI_object *)malloc(sizeof(struct WUI_object));
			new_WUI_ptr = WUI_ptr->next;
			new_WUI_ptr->next=NULL;
			new_WUI_ptr->prev=WUI_ptr;
		}
		else// then add it before this one
		{	
			new_WUI_ptr=(struct WUI_object *)malloc(sizeof(struct WUI_object));
			new_WUI_ptr->next=WUI_ptr;
			WUI_ptr->prev->next=new_WUI_ptr;
		}		

		WUI_ptr=new_WUI_ptr;
		WUI_ptr->ID = WUI_ID;
		WUI_ptr->patches_dist2km = NULL;
		WUI_ptr->patches_dist5km = NULL;
		WUI_ptr->patches_dist10km = NULL;
		WUI_ptr->fire_occurence=100;// initialize salience event as 100, replace with new salience distance as triggered
      		WUI_ptr->ntrt[0]=0;// tally of triggered salience events for this WUI, array of 3 (one for each dist). Initialize all with zero
		WUI_ptr->ntrt[1]=0;
		WUI_ptr->ntrt[2]=0;
		
		n_WUI +=1;
		} 
	}		
	h=0; z=0; pf=0; p=0; b=0;
	fnd = 0;

printf("Test1: %d\n", WUI_ptr->ID);
	/* find the patch that this  line in the file is refering to */
         while ( (fnd == 0) && (b >= 0) && (b < world[0].num_basin_files)) { 
		basin = world[0].basins[b];		
         	while ( (fnd == 0) && (h >= 0) && (h < basin[0].num_hillslopes)) { 
                hillslope =  basin[0].hillslopes[h];
//printf("looking for hillslope\n");
                while ( (fnd == 0) && (z >= 0) && (z < hillslope[0].num_zones)) {
                zone = hillslope[0].zones[z];
                
        	/* are we looking for patch families or patches */
//                if (command_line[0].multiscale_flag == 1) {
//printf("looking for patch family\n");
                while ( (fnd == 0) && (pf >= 0) && (pf < zone[0].num_patch_families)) {
                patch_family = zone[0].patch_families[pf];
//printf("patch_family ID: %d\n",patch_family[0].family_ID);
                if (family_ID == patch_family[0].family_ID) {
		p=0; //start at the first patch in this family
                while ( (fnd == 0) && (p >= 0) && (p < patch_family[0].num_patches_in_fam)) {
                patch = patch_family[0].patches[p];
		if(patch[0].ID==patch_ID){
			fnd = 1;// found it! now just use this patch below
		}
		else { p+=1; };
//			printf("found patch family\n");
			}
		}
		else { pf += 1; }
                }
		z += 1;
        }
		h += 1;
        }
		b += 1;
	}
	if(fnd==0)
	{
		printf("can't find patch %d in patch family %d\n",patch_ID,family_ID);
		exit(0);
	}
	/* now add this to the appropriate patch list for the current WUI */
	if (trt_ord2 >0) {

		/* first in this list so allocate room and assign the patch*/
		if (WUI_ptr->patches_dist2km == NULL) {
			WUI_ptr->patches_dist2km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_dist2km_ptr = WUI_ptr->patches_dist2km;//
			patches_dist2km_ptr->patch=patch;
			patches_dist2km_ptr->next=NULL;
		}
                 /*       fnd=0;
                        p=0;
			if (command_line[0].multiscale_flag == 1) {
                                while((fnd==0)&&(p>=0)&&(p<patch_family[0].num_patches_in_fam))
                                {
					if(patch_family[0].patches[p].ID==patch_ID)// is this the target patch within this family?
					{
						WUI_ptr->patches_dist2km->patch = patch_family[0].patches[p];
						patches_dist2km_ptr = WUI_ptr->patches_dist2km;// this orients the pointer for this WUI, but doesn't switch back if WUI out of 
						patches_dist2km_ptr->next=NULL;
						fnd=1;
					}
					p+=1;
				}	
			} */
		/* list exists so add additional patches */
		else {
			// first make sure you are at the end of the correct list
			patches_dist2km_ptr = WUI_ptr->patches_dist2km;
			while(patches_dist2km_ptr->next!=NULL)
			{
				patches_dist2km_ptr=patches_dist2km_ptr->next;
			}
                        patches_dist2km_ptr->next= (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
                        patches_dist2km_ptr = patches_dist2km_ptr->next;
 
                        patches_dist2km_ptr->patch=patch;
                        patches_dist2km_ptr->next=NULL;
		}		
	/*if (command_line[0].multiscale_flag == 1) {// and now  find the correct one and add it to the end of the list
	                     fnd=0;
 	                     p=0;
	                     while((fnd==0)&&(p>=0)&&(p<patch_family[0].num_patches_in_fam))
                             {
	                             if(patch_family[0].patches[p].ID==patch_ID)// is this the target patch within this family?
                                     {
					patches_dist2km_ptr->next= (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
					patches_dist2km_ptr = patches_dist2km_ptr->next;
					patches_dist2km_ptr->patch = patch_family[0].patches[p];
					patches_dist2km_ptr->next = NULL;
					printf("Added patch %d to WUI %d at trt_ord2\n",patches_dist2km_ptr->patch[0].ID,WUI_ptr->ID);
					fnd=1;
					}
					p+=1;
				}
			}*/
	}

	if (trt_ord5 >0) {/* first in this list so allocate room */
                if (WUI_ptr->patches_dist5km == NULL) {
 
                        WUI_ptr->patches_dist5km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			WUI_ptr->patches_dist5km->patch = patch;
                        patches_dist5km_ptr = WUI_ptr->patches_dist5km;// this orients the pointer for this WUI, but doesn't switch back if WUI out of
                        patches_dist5km_ptr->next=NULL;
		}
		else
		{
                       patches_dist5km_ptr = WUI_ptr->patches_dist5km;
                        while(patches_dist5km_ptr->next!=NULL)
                        {
                                patches_dist5km_ptr=patches_dist5km_ptr->next;
                        }
                        patches_dist5km_ptr->next= (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
                        patches_dist5km_ptr = patches_dist5km_ptr->next;
                        patches_dist5km_ptr->patch=patch;
                        patches_dist5km_ptr->next=NULL;

		}
	}


	if (trt_ord10 >0) {

		/* first in this list so allocate room */
		if (WUI_ptr->patches_dist10km == NULL) {
			WUI_ptr->patches_dist10km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
			patches_dist10km_ptr = WUI_ptr->patches_dist10km;
			 patches_dist10km_ptr ->patch=patch;
			patches_dist10km_ptr->next=NULL;
		}
		else {
                        patches_dist10km_ptr = WUI_ptr->patches_dist10km;
                        while(patches_dist10km_ptr->next!=NULL)
                        {
                                patches_dist10km_ptr=patches_dist10km_ptr->next;
                        }
			 patches_dist10km_ptr->next= (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
                         patches_dist10km_ptr = patches_dist10km_ptr->next;
                         patches_dist10km_ptr->patch = patch;
                         patches_dist10km_ptr->next = NULL;
                         printf("Added patch %d to WUI %d at trt_ord10\n",patches_dist10km_ptr->patch[0].ID,WUI_ptr->ID);
			}
	}
	if(trt_ord100>0) {// then add this to the first WUI0
		if(WUI_list->patches_dist100km == NULL) {
                        WUI_list->patches_dist100km = (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
                        patches_dist100km_ptr = WUI_list->patches_dist100km;
                         patches_dist100km_ptr->patch=patch;
                        patches_dist100km_ptr->next=NULL;
			 printf("Added patch %d to WUI %d at trt_ord100\n",patches_dist100km_ptr->patch[0].ID,WUI_ptr->ID);
                }
                else {
                        patches_dist100km_ptr = WUI_list->patches_dist100km;
                        while(patches_dist100km_ptr->next!=NULL&&patches_dist100km_ptr->patch[0].ID!=patch_ID)//Find end of list or if this patch already in list, don't add
                        {
                                patches_dist100km_ptr=patches_dist100km_ptr->next;
                        }
			if(patches_dist100km_ptr->patch[0].ID!=patch_ID)
			{
	                         patches_dist100km_ptr->next= (struct patch_object_list *) malloc(sizeof(struct patch_object_list));
        	                 patches_dist100km_ptr = patches_dist100km_ptr->next;
                	         patches_dist100km_ptr->patch = patch;
				patches_dist100km_ptr->next = NULL;
			// and assign its first wui_dist to this trt_ord

                         	printf("Added patch %d to WUI %d at trt_ord100\n",patches_dist100km_ptr->patch[0].ID,WUI_ptr->ID);
			}
                }
	}
	/* and now update the wuiDist for this patch for this wui*/
//	if(patch_family[0].patches[0]->wui_dist==NULL)// first wui for this patch family
	if(patch[0].wui_dist==NULL)
	{
//		patch_family[0].patches[0]->wui_dist=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
		patch[0].wui_dist=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
                patch[0].wui_dist->dist=100;
                patch[0].wui_dist->wui_id=0;
                patch[0].wui_dist->trt_ord2=-1;
                patch[0].wui_dist->trt_ord5=-1;
                patch[0].wui_dist->trt_ord10=-1;
		patch[0].wui_dist->trt_ord100=trt_ord100;
		patch[0].wui_dist->prev=NULL;
		patch[0].wui_dist->next=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));
		

//		prev_wui_dist_list_ptr= patch_family[0].patches[0]->wui_dist;
		prev_wui_dist_list_ptr= patch[0].wui_dist->next;
		prev_wui_dist_list_ptr->dist=wui_dist;
		 prev_wui_dist_list_ptr->wui_id=WUI_ID;
		prev_wui_dist_list_ptr->trt_ord2=trt_ord2;
                prev_wui_dist_list_ptr->trt_ord5=trt_ord5;
                prev_wui_dist_list_ptr->trt_ord10=trt_ord10;

		prev_wui_dist_list_ptr->next=NULL;
		prev_wui_dist_list_ptr->prev=patch[0].wui_dist;
	}
//Check	}
	else // find the correct place in the list and make a new entry. Make sure it's in the right order
	{

		
		stop_flag=1; // initialize as 1, which is the flag for end of the list
//		if(prev_wui_dist_list_ptr->next!=NULL)//it is not the end of the list
//		{
			if(prev_wui_dist_list_ptr->wui_id<=WUI_ID) // the first in the list has a lower wuiID or the same, so this one should be placed after
			{
				stop_flag=0; // then find its place in the listi
			}
			else// if the first has a higher wuiID and the new one should go before it
			{
				stop_flag=2; // then we have to insert the new one before this one in the list, so keep
				printf("first WUI is higher!\n");
			}
//		}
		while(stop_flag==0)// if the first one has a lower or equal WUI id, find the one with a higher ID or the end of the list 
		{
			printf("Shouldn't be here\n");
			if(prev_wui_dist_list_ptr->next==NULL)
			{
				stop_flag=1;//Then it is at the end of the list and we can add the new one here
			}
			else
			{
				prev_wui_dist_list_ptr=prev_wui_dist_list_ptr->next;// then advance 
				if(prev_wui_dist_list_ptr->wui_id>WUI_ID)// and if this one has a bigger id, then stop and place the new one before it
				{
					stop_flag=2;
				}
			}
		}
		next_wui_dist_list_ptr=(struct wui_dist_list *) malloc(sizeof(struct wui_dist_list));// make a entry
                next_wui_dist_list_ptr->dist=wui_dist;//fill it in
                next_wui_dist_list_ptr->wui_id=WUI_ID;
		next_wui_dist_list_ptr->trt_ord2=trt_ord2;
                next_wui_dist_list_ptr->trt_ord5=trt_ord5;
                next_wui_dist_list_ptr->trt_ord10=trt_ord10;
		// find where it should be located based on stop_flag;

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
				patch[0].wui_dist=next_wui_dist_list_ptr; // so this is the first entry, pointed at by wui_dist
				 next_wui_dist_list_ptr->prev=NULL;// and this first entry has no previous entry
			}
			 prev_wui_dist_list_ptr->prev= next_wui_dist_list_ptr;// regardless, this one goes before the prev one and don't change the next
			 next_wui_dist_list_ptr->next= prev_wui_dist_list_ptr;// and the new one points to the previous one
		}
	}

//	if(patch_family[0].patches[0]->wui_dist->next!=NULL)
	//	printf("anything happen here? patchID %d, wui ptr IS:%d, patch dist WUI IS %d\n",patch[0].ID,WUI_ptr->ID,WUI_ptr->patches_dist2km->patch[0].wui_dist->wui_id);
		
}

fclose(WUI_file);
/* echo back */
//int iter2km=0,iter5km=0,iter10km=0;
WUI_ptr = WUI_list;
while(WUI_ptr != NULL) {
	printf("\n For WUI %d\n", WUI_ptr->ID);
	patches_dist2km_ptr = WUI_ptr->patches_dist2km;
	iter2km=0;
	iter5km=0;
	iter10km=0;
	while(patches_dist2km_ptr != NULL) {
		iter2km+=1;
		printf("\nwe have at 2km patch %d, iter: %d in WUI %d",patches_dist2km_ptr->patch[0].ID,iter2km,WUI_ptr->patches_dist2km->patch[0].wui_dist->wui_id);
		 prev_wui_dist_list_ptr=patches_dist2km_ptr->patch[0].wui_dist;
		printf("wui dist: %d",prev_wui_dist_list_ptr->dist);
		while(prev_wui_dist_list_ptr->next!=NULL)
		{
			prev_wui_dist_list_ptr=prev_wui_dist_list_ptr->next;
			printf(" patch WUI ID: %d wui dist: %d",prev_wui_dist_list_ptr->wui_id, prev_wui_dist_list_ptr->dist);
		}

		patches_dist2km_ptr = patches_dist2km_ptr->next;
		}

	patches_dist5km_ptr = WUI_ptr->patches_dist5km;
	while(patches_dist5km_ptr != NULL) {
		iter5km+=1;
		printf("\n	we have at 5km %d, iter: %d",patches_dist5km_ptr->patch[0].ID,iter5km);
		patches_dist5km_ptr = patches_dist5km_ptr->next;
		}
	patches_dist10km_ptr = WUI_ptr->patches_dist10km;

	while(patches_dist10km_ptr != NULL) {
		iter10km+=1;
		printf("\n	we have at 10km %d, iter: %d wui: %d patch WUI: %d",patches_dist10km_ptr->patch[0].ID,iter10km,WUI_ptr->ID,WUI_ptr->patches_dist10km->patch[0].wui_dist->wui_id);
		patches_dist10km_ptr = patches_dist10km_ptr->next;
		}
        patches_dist100km_ptr = WUI_ptr->patches_dist100km;
       while(patches_dist100km_ptr != NULL) {
                printf("\n      we have at 100-km %d, wui: %d trtOrd: %d",patches_dist100km_ptr->patch[0].ID,WUI_ptr->ID,patches_dist100km_ptr->patch[0].wui_dist->trt_ord100);
                patches_dist100km_ptr = patches_dist100km_ptr->next;
                }

	WUI_ptr = WUI_ptr->next;
	}
	return(WUI_list);
} /*end construct_WUI_list.c*/

