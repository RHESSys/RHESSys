/*--------------------------------------------------------------*/
/* 																*/
/*					update_fire_in_WUI					*/
/*																*/
/*	update_fire_in_WUI.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	update_fire_in_WUI.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object update_fire_in_WUI( 		*/
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

void update_fire_in_WUI(struct WUI_object *WUI_list)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void *alloc(size_t, char *, char *);
	
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct WUI_object *WUI_ptr;	
	double fire_area_occur;
 
	struct patch_object_list *patches_dist2km_ptr;
	struct patch_object_list *patches_dist5km_ptr;
	struct patch_object_list *patches_dist10km_ptr;

	struct patch_object *patch;
	/*--------------------------------------------------------------*/
	/* cycle through WUIs  */
	/*--------------------------------------------------------------*/

	WUI_ptr = WUI_list;
	while(WUI_ptr != NULL) {

	/*--------------------------------------------------------------*/
	/* cycle through patches in each distance list to see if there was a fire */
	/*--------------------------------------------------------------*/

	/* 2km list */
	patches_dist2km_ptr = WUI_ptr->patches_dist2km;
	fire_area_occur=0;
	while( (patches_dist2km_ptr != NULL) ) {
		patch = patches_dist2km_ptr->patch;
		if (patch[0].fire.severity >= patch[0].landuse_defaults[0][0].salience_fire_level)
			fire_area_occur += patch[0].area;
		patches_dist2km_ptr = patches_dist2km_ptr->next;
		}
	WUI_ptr->fire_occurence[D2KM] = fire_area_occur;

	/* 5km list */
	patches_dist5km_ptr = WUI_ptr->patches_dist5km;
	fire_area_occur=0;
	while( (patches_dist5km_ptr != NULL) ) {
		patch = patches_dist5km_ptr->patch;
		if (patch[0].fire.severity >= patch[0].landuse_defaults[0][0].salience_fire_level)
			fire_area_occur += patch[0].area;
		patches_dist5km_ptr = patches_dist5km_ptr->next;
		}
	WUI_ptr->fire_occurence[D5KM] = fire_area_occur;

	/* 10km list */
	patches_dist10km_ptr = WUI_ptr->patches_dist10km;
	while( (patches_dist10km_ptr != NULL) ) {
		patch = patches_dist10km_ptr->patch;
		if (patch[0].fire.severity >= patch[0].landuse_defaults[0][0].salience_fire_level)
			fire_area_occur += patch[0].area;
		patches_dist10km_ptr = patches_dist10km_ptr->next;
		}
	WUI_ptr->fire_occurence[D10KM] = fire_area_occur;


	WUI_ptr = WUI_ptr->next;
	}



	return;
} /*end update_fire_in_WUI.c*/
