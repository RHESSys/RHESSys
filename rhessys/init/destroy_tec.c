/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_tec									*/
/*																*/
/* 	destroy_tec - destroys the tec file (closes it)				*/
/*																*/
/*	NAME														*/
/* 	destroy_tec - destroys the tec file (closes it)				*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Closes the tec file.	 									*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	destroy_tec(
					struct	tec_object *tec)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Close the tec file.											*/
	/*--------------------------------------------------------------*/
	fclose( tec[0].tfile );
	return;
} /*end destroy tec file*/
