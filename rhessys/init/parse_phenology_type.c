/*--------------------------------------------------------------*/
/*                                                              */ 
/*		parse_phenology_type					*/
/*                                                              */
/*  NAME                                                        */
/*		parse_phenology_type					*/
/*   	                                                        */
/*                                                              */
/*  SYNOPSIS                                                    */
/* int parse_phenology_type( char *)					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*	returns an integer value from vegetation type		*/
/*	character string					*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

int	parse_phenology_type( char *input_string)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int veg_type;
	if ((strncmp(input_string,"DECIDUOUS",5) == 0) ||
		 (strncmp(input_string,"deciduous",5) == 0) )
		veg_type = DECID;
	else if ((strncmp(input_string,"EVERGREEN",9) == 0) ||
			(strncmp(input_string,"evergreen",9) == 0) )
		veg_type = EVERGREEN;
	else {
		fprintf(stderr,"\n FATAL ERROR - parse_phenology_type");
		fprintf(stderr,"\n Unknown phenology type %s in stratum default file", input_string);
		fprintf(stderr,"\n Must be either DECID or EVERGREEN");
		exit(EXIT_FAILURE);
		}

	free(input_string);	
	return(veg_type);
}/*parse_phenology_type.c*/
