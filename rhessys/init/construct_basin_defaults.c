/*--------------------------------------------------------------*/
/* 								*/
/*	construct_basin_defaults				*/
/*								*/
/*	construct_basin_defaults.c - makes basin default	*/
/*			objects.				*/
/*								*/
/*	NAME							*/
/*	construct_basin_defaults.c - makes basin default	*/
/*			objects.				*/
/*								*/
/*	SYNOPSIS						*/
/*	struct basin_default *construct_basin_defaults(     	*/
/*			num_default_files,			*/
/*			default_files,				*/
/*			grow_flag,				*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Original code, January 15, 1996.			*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct basin_default *construct_basin_defaults(
	int	num_default_files,
	char	**default_files,
	struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	void	*alloc( size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int 	i;
	FILE	*default_file;
	char	record[MAXSTR];
	struct	basin_default	*default_object_list;
        param *paramPtr = NULL;
        int paramCnt = 0;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.			*/
	/*--------------------------------------------------------------*/
	default_object_list   = (struct basin_default *)
		alloc(num_default_files *
		sizeof(struct basin_default),"default_object_list",
		"construct_basin_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*	read the ith default file into the ith object.		*/
		/*--------------------------------------------------------------*/
		printf("Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "basin_default_ID", "%d", 1, 1);
		/*--------------------------------------------------------------*/
		/*	if grow flag allocate a grow default structure for the	*/
		/*	ith object.						*/
		/*--------------------------------------------------------------*/
		if ( command_line[0].grow_flag ){
			default_object_list[i].grow_defaults = (struct basin_grow_default *)
				alloc( 1 * sizeof(struct basin_grow_default),
				"grow_defaults","construct_basin_defaults");
			/*--------------------------------------------------------------*/
			/*		    NOTE: PLACE ANY GROW READING HERE.		*/
			/*--------------------------------------------------------------*/
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		Close the ith default file.			*/
		/*--------------------------------------------------------------*/

	} /*end for*/

	return(default_object_list);
} /*end construct_basin_defaults*/
