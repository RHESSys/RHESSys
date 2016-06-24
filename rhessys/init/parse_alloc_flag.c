/*--------------------------------------------------------------*/
/*                                                              */ 
/*		parse_alloc_flag					*/
/*                                                              */
/*  NAME                                                        */
/*		parse_alloc_flag					*/
/*   	                                                        */
/*                                                              */
/*  SYNOPSIS                                                    */
/* int parse_alloc_flag( char *)					*/
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

int	parse_alloc_flag( char *input_string)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int alloc_flag;
	if ((strncmp(input_string,"STATIC",6) == 0) ||
		(strncmp(input_string, "static",6) == 0) )
		alloc_flag = CONSTANT;
	else if ((strncmp(input_string,"DYNAMIC",7) == 0) ||
		(strncmp(input_string, "dynamic",7) == 0) )
		alloc_flag = DYNAMIC;
	else if ((strncmp(input_string,"CONSTANT",8) == 0) ||
		(strncmp(input_string, "constant",8) == 0) )
		alloc_flag = CONSTANT;
	else if ((strncmp(input_string,"WARING",6) == 0) ||
		(strncmp(input_string, "waring",6) == 0) )
		alloc_flag = WARING;
	else if ((strncmp(input_string,"DICKENSON",9) == 0) ||
		(strncmp(input_string, "dickenson",9) == 0) )
		alloc_flag = DICKENSON;
	else if ((strncmp(input_string,"COMBINED",8) == 0) ||
		(strncmp(input_string, "combined",8) == 0) )
		alloc_flag = COMBINED;
	else {
		fprintf(stderr,"\n FATAL ERROR - parse_alloc_flag");
		fprintf(stderr,"\n Flag must be static or dynamic and is %s" , input_string);
		exit(EXIT_FAILURE);
	}
	free(input_string);
	return(alloc_flag);
}/*end parse_alloc_flag.c*/
