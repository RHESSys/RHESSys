/*--------------------------------------------------------------*/
/*                                                              */
/*		ew_horizon									    */
/*                                                              */
/*  NAME                                                        */
/*		 ew_horizon										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 ew_horizon( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*  revision:  6.0 29 April, 2005                               */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"

void ew_horizon(dem, ehr, whr, maxr, maxc, cell, scale_dem)
	int *dem;
	float *ehr, *whr;
	double cell, scale_dem;
	int maxr, maxc;

    {

 	/* local variable declarations */
	int inx, cr, hr_inx;
	int r,c,pch;

	double x, y;
	double curr_ehr, curr_whr;
	double new_ehr, new_whr;
	double curr_elev, elev_diff;
	double maxe_elev, maxw_elev;

 	/* local function definitions */

	curr_ehr = 0;
	curr_whr = 0;

	printf("\n Starting EW horizon \n");

	for (r=0; r< maxr; r++) 
		{
		for (c=0; c< maxc; c++)
			{
			inx = r*maxc+c;
			curr_ehr = 0.0;
			curr_whr = 0.0;
			maxe_elev = 0.0;
			maxw_elev = 0.0;
			curr_elev = dem[inx]*scale_dem;

			for (cr = 0; cr < maxc; cr++)	{
				hr_inx = r*maxc+cr;	
				elev_diff = dem[hr_inx]*scale_dem-curr_elev;
				if (cr < c) {
					if (elev_diff > 0.001) {
						y = (c-cr)*cell;
						x = elev_diff;
				 		new_whr = x / sqrt(x*x + y*y );
					}
					else new_whr = 0.0;
					if (new_whr > curr_whr) { curr_whr = new_whr;
						maxw_elev = dem[hr_inx]; }
					}
				if (cr > c) {
					if (elev_diff > 0.001) {
						y = (cr-c)*cell;
						x = elev_diff;
				 		new_ehr = x / sqrt(x*x + y*y );
						}
					else new_ehr = 0.0;
					if (new_ehr > curr_ehr) {curr_ehr = new_ehr;
						maxe_elev = dem[hr_inx]; }
					}
	
				} /* end column scan */
				ehr[inx] = curr_ehr;	
				whr[inx] = curr_whr;	
			}	/* end col */	
			
		} /* end row */


	return;


    }



