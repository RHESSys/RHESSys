/*--------------------------------------------------------------*/
/*								*/
/*	cconstruct_filename_list - creates list of filenames	*/
/*								*/
/*	NAME							*/
/*	construct_filename_list - creates list of filenames	*/
/*								*/
/*	SYNOPSIS						*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*	Original code, March 15, 1996.				*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

char	**construct_filename_list( FILE *file, int num_filenames){
	/*--------------------------------------------------------------*/
	/*	Local Function Declaration				*/
	/*--------------------------------------------------------------*/
	void	*alloc(size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	i;
	char	 record[MAXSTR];
	char	**file_list;
	/*--------------------------------------------------------------*/
	/*	Allocate a list of filenames.				*/
	/*--------------------------------------------------------------*/
	file_list = (char **) alloc(num_filenames *
		sizeof(char *),"file_list","construct_filename_list" );
	/*--------------------------------------------------------------*/
	/*	Read in each file name.										*/
	/*--------------------------------------------------------------*/
	for (i = 0 ; i < num_filenames; i++ ){
		file_list[i] = (char *) alloc(256 *sizeof(char),
			"file_list","construct_fdilename_list");
		fscanf( file, "%s", file_list[i]);
		read_record(file, record);
	}
	return(file_list);
} /*end construct_filename_list*/
