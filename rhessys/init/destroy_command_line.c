/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_command_line						*/
/*																*/
/*	destroy_command_line.c - destroys command line object		*/
/*																*/
/*	NAME														*/
/*	destroy_command_line.c - destroy command line object		*/
/*																*/
/*	SYNOPSIS													*/
/*	void destroy_command_line(command_line)						*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, March 15, 1996.								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void destroy_command_line(struct command_line_object	*command_line)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Close the tec file if it is present.						*/
	/*	(FOR NOW WE HAVE A DUMMY ARGUEMENT.			*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].tec_flag == 1){
		command_line[0].tec_flag = 1;
	}
	return;
} /*end destroy_command_line*/
