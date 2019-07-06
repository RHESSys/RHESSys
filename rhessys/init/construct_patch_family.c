/*--------------------------------------------------------------*/
/*						                                		*/
/*		    construct_patch_family  		                   	*/
/*							                                   	*/
/*	construct_patch_family.c - creates a patch family 	    */
/*							                                	*/
/*	NAME					                            		*/
/*	construct_patch_family.c - routes within patch family   	*/
/*							                                	*/
/*	SYNOPSIS					                            	*/
/*	 construct_patch_family( ) 	*/
/*                                                              */
/*					                                            */
/*				                                            	*/
/*                                                              */
/*										                       	*/
/* 										                       	*/
/*										                       	*/
/*	OPTIONS								                   		*/
/*										                       	*/
/*										                       	*/
/*	DESCRIPTION								                   	*/
/*	Create patch family data object  							*/
/*										                       	*/
/*										                       	*/
/*										                       	*/
/*	PROGRAMMER NOTES							               	*/
/*										                       	*/
/*	July, 2019 William Burke						           	*/
/*	                     	                                	*/
/*											                    */
/*--------------------------------------------------------------*/

// includes
#include <stdio.h>

struct patch_family *construct_patch_family(
    struct	command_line_object	*command_line,
	FILE	*world_file) 
{


/*--------------------------------------------------------------*/
/*	Local function definition.				                    */
/*--------------------------------------------------------------*/

// maybe nothing here?

/*--------------------------------------------------------------*/
/*	Local variable definition.			                    	*/
/*--------------------------------------------------------------*/

int num_patch_family;
int patch_families;
struct patch_family_object *patch_family;


/*--------------------------------------------------------------*/
/*  Allocate a patch family object.                             */
/*--------------------------------------------------------------*/

// FIX FOR PATCH FAMILY NOT PATCH
//patch_family = (struct patch_family_object *) alloc( 1 * sizeof( struct patch_family_object ),"patch","construct_patch");

/*--------------------------------------------------------------*/
/*	Loop thru patches            	                    	*/
/*--------------------------------------------------------------*/

// loop through patches
// IF - family ID is in array, add pointer to patch ID to array in patch family struct
// IF not in array, add ID to patch_family array as well as patch ID pointer

//patch_family[0].family_ID = XXXXXXXXX ;

/*--------------------------------------------------------------*/
/*	Find patches in patch family        	*/
/*--------------------------------------------------------------*/




//patch_family[0].patches =  XXXXXXXXX ;



}
