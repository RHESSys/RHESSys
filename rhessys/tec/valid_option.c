/*--------------------------------------------------------------*/
/* 																*/
/*						validate_option							*/
/*																*/
/*	validate_option.c - makes sure a option is OK				*/
/*																*/
/*	NAME														*/
/*	validate_option.c - makes sure a option is OK				*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	The routine performs as follows:							*/
/*																*/
/*	June 3, 1997 C.Tague					*/
/*	- changed from multipl. to if statement			*/
/*	 to check for valid (zero) option			*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

int	 valid_option( char *command_line){
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	/*--------------------------------------------------------------*/
	/*	Set i to zero if it matches and valid options.				*/
	/*--------------------------------------------------------------*/
	i = 1;
	if (( strcmp(command_line,"bgc") == 0) ||
		(strcmp(command_line,"-v")  == 0) ||
		(strcmp(command_line,"-e")  == 0) ||
		(strcmp(command_line,"-b")  == 0) ||
		(strcmp(command_line,"-h")  == 0) ||
		(strcmp(command_line,"-p")  == 0) ||
		(strcmp(command_line,"-g")  == 0) ||
		(strcmp(command_line,"-c")  == 0) ||
		(strcmp(command_line,"-o")  == 0) ||
		(strcmp(command_line,"-w")  == 0) ||
		(strcmp(command_line,"-r")  == 0) ||
		(strcmp(command_line,"-t")  == 0) ||
		(strcmp(command_line,"-s")  == 0) ||
		(strcmp(command_line,"-z")  == 0) ||
		(strcmp(command_line,"-sv")  == 0) ||
		(strcmp(command_line,"-st")  == 0) ||
		(strcmp(command_line,"-th")  == 0) ||
		(strcmp(command_line,"-ed")  == 0) ||
		(strcmp(command_line,"-tmp")  == 0) ||
		(strcmp(command_line,"-gw")  == 0) ||
		(strcmp(command_line,"-tchange")  == 0) ||
		(strcmp(command_line,"-pre") == 0) ||
		(strcmp(command_line,"-rddn")  == 0) ||
		(strcmp(command_line,"-stdev") == 0) ||
		(strcmp(command_line,"-dor") == 0) ||
		(strcmp(command_line,"-csv") == 0) ||
		(strcmp(command_line,"-vgsen") == 0) ||
		(strcmp(command_line,"-noredist") == 0) ||
		(strcmp(command_line,"-vmort") == 0) ||
		(strcmp(command_line,"-svalt") == 0) ||
		(strcmp(command_line,"-precip") == 0) ||
		(strcmp(command_line,"-gwtoriparian") == 0) ||
		(strcmp(command_line,"-surfaceenergy") == 0) ||
		(strcmp(command_line,"-firespread") == 0) ||
		(strcmp(command_line,"-snowdistb") == 0) ||
		(strcmp(command_line,"-whdr") == 0) ||
		(strcmp(command_line,"-netcdf") == 0) ||
		(strcmp(command_line,"-climrepeat") == 0) ||

		(strcmp(command_line,"-template") == 0) ||
		(strcmp(command_line,"-fs") == 0) ||

		(strcmp(command_line,"-vegspinup") == 0) ||
		(strcmp(command_line,"-template") == 0))

		i = 0;
	if ( i == 0 ){
		return(1);
	}
	else{
		return(0);
	}
} /*end validate_option.c*/
