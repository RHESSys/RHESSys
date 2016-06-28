/*--------------------------------------------------------------*/
/*                                                              */ 
/*		parse_dyn_flag					*/
/*                                                              */
/*  NAME                                                        */
/*		parse_dyn_flag					*/
/*   	                                                        */
/*                                                              */
/*  SYNOPSIS                                                    */
/* int parse_dyn_flag( char *)					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*	returns an integer value from vegetation type		*/
/*	character string					*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rhessys.h"

int	parse_dyn_flag( char *input_string)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int dyn_flag;
	if ((strncmp(input_string,"STATIC",6) == 0) ||
		(strncmp(input_string, "static",6) == 0) )
		dyn_flag = STATIC;
	else if ((strncmp(input_string,"DYNAMIC",7) == 0) ||
		(strncmp(input_string, "dynamic",7) == 0) )
		dyn_flag = DYNAMIC;
	else if ((strncmp(input_string,"CONSTANT",8) == 0) ||
		(strncmp(input_string, "constant",8) == 0) )
		dyn_flag = STATIC;
	else {
		fprintf(stderr,"\n FATAL ERROR - parse_dyn_flag");
		fprintf(stderr,"\n Flag must be stati or dynamic and is %s" , input_string);
		exit(EXIT_FAILURE);
	}
	free(input_string);
	return(dyn_flag);
}/*end parse_dyn_flag.c*/
