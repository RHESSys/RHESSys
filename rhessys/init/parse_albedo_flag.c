/*--------------------------------------------------------------*/
/*                                                              */ 
/*		parse__flag					*/
/*                                                              */
/*  NAME                                                        */
/*		parse_albedo_flag					*/
/*   	                                                        */
/*                                                              */
/*  SYNOPSIS                                                    */
/* int parse_albedo_flag( char *)					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*	returns an integer value from albedo model type		*/
/*	character string					*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rhessys.h"

int	parse_albedo_flag( char *input_string)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int albedo_flag;
	if ((strncmp(input_string,"age",3) == 0) ||
		(strncmp(input_string, "AGE",3) == 0) )
		albedo_flag = 1;
	else if ((strncmp(input_string,"bats",4) == 0) ||
		(strncmp(input_string, "BATS",4) == 0) )
		albedo_flag = 2;
	else {
		fprintf(stderr,"\n FATAL ERROR - parse_albedo_flag");
		fprintf(stderr,"\n Flag must be age or bats and is %s" , input_string);
		exit(EXIT_FAILURE);
	}

	free(input_string);
	
	return(albedo_flag);
}/*end parse_albedo_flag.c*/
