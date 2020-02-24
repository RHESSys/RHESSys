/*--------------------------------------------------------------*/
/* 																*/
/*					reset_fire_in_WUI					*/
/*																*/
/*	reset_fire_in_WUI.c - resets cumulative WUI fire area		*/
/*																*/
/*	NAME														*/
/*	reset_fire_in_WUI.c - resets cumulative WUI fire area		*/
/*																*/
/*	SYNOPSIS													*/
/*	resets cumulative WUI fire area, each year		*/
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
/*	PROGRAMMER NOTES
MK Feb 23, 2020*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rhessys.h"

void reset_fire_in_WUI(struct WUI_object *WUI_list)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void *alloc(size_t, char *, char *);
	
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct WUI_object *WUI_ptr;	

	/*--------------------------------------------------------------*/
	/* cycle through WUIs  */
	/*--------------------------------------------------------------*/

	WUI_ptr = WUI_list;
	while(WUI_ptr != NULL) {

	/*--------------------------------------------------------------*/
	/* cycle through patches in each distance list to see if there was a fire */
	/*--------------------------------------------------------------*/

	/* 2km list */
	WUI_ptr->fire_occurence[D2KM] =0;
	/* 5km list */
	WUI_ptr->fire_occurence[D5KM] =0;
	/* 10km list */
	WUI_ptr->fire_occurence[D10KM] =0;
	}
	return;
} /*end reset_fire_in_WUI.c*/
