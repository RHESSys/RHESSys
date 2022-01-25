/*--------------------------------------------------------------*/
/* 																*/
/*					execute_fueltreatment_event								*/
/*																*/
/*	execute_fueltreatment_event.c - identifies patches to be treated and calls treatment function					*/
/*																*/
/*	NAME														*/
/*	execute_fueltreatment_event.c
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

void execute_fueltreatment_event(
									 struct	world_object *world,
									 struct	command_line_object	*command_line,
									 struct date	current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


	void *alloc(size_t, char *, char *);

// define treatment function here


	void treat_patch(struct patch_object *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct WUI_object *WUI_ptr;//pointers to navigate salience lists
	struct patch_object_list *patches_ptr;
	struct patch_object *patch;
 

	struct wui_dist_list *wui_dist_ptr;
//	struct wui_dist_list *sal_wui_dist_list_ptr;
	int salience_event,stop_flag;

	
	
	
// add code here to define understory et and pet to calculate understory deficit. The definition of understory here
// will be the same as that for fire effects below

// default value to determine if you are in the understory, compare layer height to stratum-level height threshold (canopy_strata_upper[0].defaults[0][0].understory_height_thresh,
// compared to patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])].epv.height

	/*--------------------------------------------------------------*/
	/* Loop through WUI list and determine if salience event is triggered			*/
	/* If it is, determine which patches should be treated			*/
	/* if it isn't, then determine which non-salience patches should be treated */
	/*--------------------------------------------------------------*/
	salience_event=0;
	if (command_line[0].salience_flag == 1)// enter loop if salience is flagged
	{
		WUI_ptr=world[0].WUI_list->next; // skip the first in the list
		while(WUI_ptr!=NULL) // loop through the WUIs
		{
			printf("Current wui: %d\n",WUI_ptr->ID);
			if(WUI_ptr->next!=NULL)
				printf("Next wui: %d\n",WUI_ptr->next->ID);
			patches_ptr=NULL; // initialize the patch list
			if(WUI_ptr->fire_occurence<100) // then a salience event was triggered for this WUI
			{
				salience_event=1; //flag the salience event
				if(WUI_ptr->fire_occurence==2) // is this a 2 km salience event?
				{
					patches_ptr=WUI_ptr->patches_dist2km; // loop through the 2 km patch list
					WUI_ptr->ntrt[1]+=1; //update number of times this has been treated at this salience level, to match to trt_ord, and regardless if a patch is actually treated
					while(patches_ptr!=NULL)//navigate the 2km list
					{
						patch=patches_ptr->patch;
						wui_dist_ptr=patch->wui_dist;
						stop_flag=0;
						if(wui_dist_ptr->wui_id==WUI_ptr->ID)
							stop_flag=1;
						while(stop_flag==0) // make sure we're at the correct wui in the patch.wui_dist_list
						{
							wui_dist_ptr=wui_dist_ptr->next;
							if(wui_dist_ptr!=NULL)
							{
								if(wui_dist_ptr->wui_id==WUI_ptr->ID)
									stop_flag=1;
							}
							else
								stop_flag=2;
						}
						if(stop_flag==1)
						{
							if(wui_dist_ptr->trt_ord2==WUI_ptr->ntrt[1]) // is this the correct treatment order? for the correct wui?
							{
								treat_patch(patch); // Update with final function
								printf("Treated patch ID, dist2: %d, ntrt: %d\n",patch[0].ID,WUI_ptr->ntrt[1]);
							}
						}
						patches_ptr=patches_ptr->next;
					}
					
					if(WUI_ptr->next!=NULL)
	                        	        printf("Next wui fire_occurence2: %d\n",WUI_ptr->next->ID);				
				}
				else // as above but for 5 and 10 km salience events
				{
					if(WUI_ptr->fire_occurence==5)
					{
						patches_ptr=WUI_ptr->patches_dist5km;
						WUI_ptr->ntrt[2]+=1;
						while(patches_ptr!=NULL)
						{
							patch=patches_ptr->patch;
							wui_dist_ptr=patch->wui_dist;
							stop_flag=0;
                                                	if(wui_dist_ptr->wui_id==WUI_ptr->ID)
                                                        	stop_flag=1;
	                                                while(stop_flag==0) // make sure we're at the correct wui in the patch.wui_dist_list
        	                                        {
                	                                        wui_dist_ptr=wui_dist_ptr->next;
                        	                                if(wui_dist_ptr!=NULL)
                                	                        {
                                        	                        if(wui_dist_ptr->wui_id==WUI_ptr->ID)
                                                	                        stop_flag=1;
                                                        	}
	                                                        else
        	                                                        stop_flag=2;
                	                                }
							if(stop_flag==1)
                                                	{
								if(wui_dist_ptr->trt_ord5==WUI_ptr->ntrt[2])
								{
									treat_patch(patch); // Update with final function
									printf("Treated patch ID, dist5: %d, ntrt: %d\n",patch[0].ID,WUI_ptr->ntrt[2]);
								}
							}
							patches_ptr=patches_ptr->next;
						}
						if(WUI_ptr->next!=NULL)
	                                                printf("Next wui fire_occurence5: %d\n",WUI_ptr->next->ID);
					}
					else
					{
						patches_ptr=WUI_ptr->patches_dist10km;
						if(WUI_ptr->next!=NULL)
                                                        printf("Next wui fire_occurence10a: %d\n",WUI_ptr->next->ID);

						WUI_ptr->ntrt[3]+=1;
						if(WUI_ptr->next!=NULL)
                                                        printf("Next wui fire_occurence10b: %d\n",WUI_ptr->next->ID);
	                                        while(patches_ptr!=NULL)
                                                {
                                                        patch=patches_ptr->patch;
							wui_dist_ptr=patch->wui_dist;
                        	                        stop_flag=0;
                	                                if(wui_dist_ptr->wui_id==WUI_ptr->ID)
 		                                                stop_flag=1;
                                                        while(stop_flag==0) // make sure we're at the correct wui in the patch.wui_dist_list
                                                        {
                                                                wui_dist_ptr=wui_dist_ptr->next;
                                                                if(wui_dist_ptr!=NULL)
                                                                {
                                                                        if(wui_dist_ptr->wui_id==WUI_ptr->ID)
                                                                                stop_flag=1;
                                                                }
                                                                else
                                                                        stop_flag=2;
                                                        }
							if(stop_flag==1)
	                                                { 
								if(wui_dist_ptr->trt_ord10==WUI_ptr->ntrt[3])
								{
									treat_patch(patch); // Update with final function
									printf("Treated patch ID, dist10: %d, ntrt: %d\n",patch[0].ID,WUI_ptr->ntrt[3]);
								}
							}
							patches_ptr=patches_ptr->next;
						}
						if(WUI_ptr->next!=NULL)
	                                                printf("Next wui fire_occurence10: %d\n",WUI_ptr->next->ID);
					}
				}
				WUI_ptr->fire_occurence=100; // reset the flag for next year
				
			}
			printf("Current wui place 1a: %d\n",WUI_ptr->ID);
			WUI_ptr=WUI_ptr->next;
			if(WUI_ptr!=NULL)
				printf("Current wui place 2: %d\n",WUI_ptr->ID);
			
		}
		if(salience_event==0)// then test for baseline treatment
		{
			//printf("Current wui Non-salience 1: %d\n",WUI_ptr->ID);
			WUI_ptr=world[0].WUI_list; // go back to the first in the list
			printf("Current wui Non-salience 2: %d\n",WUI_ptr->ID);
			patches_ptr=WUI_ptr->patches_dist100km;
			WUI_ptr->ntrt[0]+=1;
			while(patches_ptr!=NULL) // navigate this linked list
			{
				patch=patches_ptr->patch;
				if(patch->wui_dist->trt_ord100==WUI_ptr->ntrt[0])
				{
					treat_patch(patch); // Update with final function
					printf("Treated patch ID, baseline: %d, ntrt: %d\n",patch[0].ID,WUI_ptr->ntrt[0]);
				}
				patches_ptr=patches_ptr->next;
			}
		}
	}
		
	return;
} /*end execute_fueltreatment_event.c*/

