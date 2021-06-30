/*--------------------------------------------------------------*/
/* 																*/
/*					update_salience					*/
/*																*/
/*	update_salience.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	update_salience.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object update_salience( 		*/
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

void update_salience(struct WUI_object *WUI_list)
													  
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
	/* cycle patches in each distance  */
	/*--------------------------------------------------------------*/

	/* 2km list */
	patches_dist2km_ptr = WUI_ptr->patches_dist2km;
	while( (patches_dist2km_ptr != NULL) ) {
		patch = patches_dist2km_ptr->patch;

		if (WUI_ptr->fire_occurence[D2KM] >= patch[0].landuse_defaults[0][0].salience_fire_area) 
			patch[0].fuel_treatment.salience_prob[D2KM][D2KM] = patch[0].landuse_defaults[0][0].salience_2km2km_prob;
		if (WUI_ptr->fire_occurence[D5KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D2KM][D5KM] = patch[0].landuse_defaults[0][0].salience_2km5km_prob;
		if (WUI_ptr->fire_occurence[D10KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D2KM][D10KM] = patch[0].landuse_defaults[0][0].salience_2km10km_prob;
		patches_dist2km_ptr = patches_dist2km_ptr->next;
		}

	/* 5km list */
	patches_dist5km_ptr = WUI_ptr->patches_dist5km;
	while( (patches_dist5km_ptr != NULL) ) {
		patch = patches_dist5km_ptr->patch;

		if (WUI_ptr->fire_occurence[D2KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D5KM][D2KM] = patch[0].landuse_defaults[0][0].salience_2km2km_prob;
		if (WUI_ptr->fire_occurence[D5KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D5KM][D5KM] = patch[0].landuse_defaults[0][0].salience_2km5km_prob;
		if (WUI_ptr->fire_occurence[D10KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D5KM][D10KM] = patch[0].landuse_defaults[0][0].salience_2km10km_prob;
		patches_dist5km_ptr = patches_dist5km_ptr->next;
		}

	/* 10km list */
	patches_dist10km_ptr = WUI_ptr->patches_dist10km;
	while( (patches_dist10km_ptr != NULL) ) {
		patch = patches_dist10km_ptr->patch;

		if (WUI_ptr->fire_occurence[D2KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D10KM][D2KM] = patch[0].landuse_defaults[0][0].salience_2km2km_prob;
		if (WUI_ptr->fire_occurence[D5KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D10KM][D5KM] = patch[0].landuse_defaults[0][0].salience_2km5km_prob;
		if (WUI_ptr->fire_occurence[D10KM] >= patch[0].landuse_defaults[0][0].salience_fire_area)
			patch[0].fuel_treatment.salience_prob[D10KM][D10KM] = patch[0].landuse_defaults[0][0].salience_2km10km_prob;

		patches_dist10km_ptr = patches_dist10km_ptr->next;
		}



	WUI_ptr = WUI_ptr->next;
	}



	return;
} /*end update_salience.c*/
