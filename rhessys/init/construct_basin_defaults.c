/*--------------------------------------------------------------*/
/* 																*/
/*					construct_basin_defaults				*/
/*																*/
/*	construct_basin_defaults.c - makes basin default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_basin_defaults.c - makes basin default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct basin_default *construct_basin_defaults(     */
/*								  num_default_files,			*/
/*								  default_files,				*/
/*								  grow_flag,					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
struct basin_default *construct_basin_defaults(
											   int	num_default_files,
											   char	**default_files,
											   int grow_flag)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc( size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int 	i;
	FILE	*default_file;
	char	record[MAXSTR];
	struct	basin_default	*default_object_list;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct basin_default *)
		alloc(num_default_files *
		sizeof(struct basin_default),"default_object_list",
		"construct_basin_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct basine defaults,unable to open defaults file %s.\n",default_files[i]);
			exit(EXIT_FAILURE);
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);
		/*--------------------------------------------------------------*/
		/*		if grow flag allocate a grow default structure for the	*/
		/*		ith object.												*/
		/*--------------------------------------------------------------*/
		if ( grow_flag ){
			default_object_list[i].grow_defaults = (struct basin_grow_default *)
				alloc( 1 * sizeof(struct basin_grow_default),
				"grow_defaults","construct_basin_defaults");
			/*--------------------------------------------------------------*/
			/*		    NOTE: PLACE ANY GROW READING HERE.					*/
			/*--------------------------------------------------------------*/
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_basin_defaults*/
