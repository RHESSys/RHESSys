/*--------------------------------------------------------------*/
/*                                                              */
/*		read_allom_table									    */
/*                                                              */
/*  NAME                                                        */
/*		 read_allom_table										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 read_allom_table( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "blender.h"
#define DtoR 0.01745329 

void	read_allom_table(fac, nvegtype, allometric_table)
	struct allom_struct *allometric_table;
	int nvegtype;
	FILE *fac;

    {
	int i;



	for (i=0; i< nvegtype; i++) {

		fscanf(fac, "%d", &allometric_table[i].vegid);
		fscanf(fac, "%f", &allometric_table[i].sla);
		fscanf(fac, "%f", &allometric_table[i].lr);
		fscanf(fac, "%f", &allometric_table[i].ls);
		fscanf(fac, "%f", &allometric_table[i].sc);
		fscanf(fac, "%f", &allometric_table[i].lived);
		fscanf(fac, "%f", &allometric_table[i].cnl);
		fscanf(fac, "%f", &allometric_table[i].cnr);
		fscanf(fac, "%f", &allometric_table[i].cnlw);
		fscanf(fac, "%f", &allometric_table[i].cndw);
		}


	fclose(fac);

	return;

    }



