/*--------------------------------------------------------------*/
/* 								*/
/*		read_record.c					*/
/*								*/
/*	read_record.c -  read a record up to a new line or eof	*/
/*								*/
/*	NAME							*/
/*	read_record.c -  read a record up to a new line or eof	*/
/*								*/
/*	SYNOPSIS						*/
/*	void	*read_record( fp, record )			*/
/*								*/
/*	OPTIONS							*/
/*	FILE	fp	- pointer to input file			*/
/*	char 	*record	- character string of record itself	*/
/*								*/
/*	DESCRIPTION						*/
/*	this routine is used to get rid (read) comments		*/
/*	associated with input data file				*/
/*	it assumes the comment follows the data item		*/
/*	and will read until the end of the line			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*   the *len (for the length of record was removed 8/25/99 SEB*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

/*-----------------------------------------------
 ead a record upto a new line or eof
from axing's oksis.h
fp      - file pointer
record  - character string of the record itself
------------------------------------------------*/

int read_record(FILE *fp, char *record)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int  j;
	char ch;
	
	if (fp == NULL){
		return(0);
	}
	for (j = 0; ((ch = getc(fp))!='\n' && !feof(fp)); j++){
		record[j] = ch;
	}
	record[j]='\0';
	if (feof(fp)) return(-1);

	if (j >= MAXSTR) {
		printf("\n Comment in default file is too long\n");
		exit(EXIT_FAILURE);
		}
	return(0);
}
