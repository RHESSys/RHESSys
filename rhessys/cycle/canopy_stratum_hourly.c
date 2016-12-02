/*--------------------------------------------------------------*/
/* 																*/
/*						canopy_stratum_hourly					*/
/*																*/
/*	NAME														*/
/*	canopy_statrum_hourly 										*/
/*			 - performs cycling and output of a canopy_stratum	*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void canopy_stratum_hourly( 								*/
/*						struct	world_object *,					*/	
/*						struct	basin_object *,					*/	
/*						struct	hillslope_object *,				*/	
/*						struct	zone_object *,					*/	
/*						struct	patch_object *,					*/	
/*						struct canopy_stata_object *,			*/
/*						struct command_line_object *,			*/
/*						struct tec_entry *,						*/
/*						struct date)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	canopy_stata in the canopy_statrum. 						*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include "rhessys.h"
void	canopy_stratum_hourly(
							  struct world_object *world,
							  struct basin_object *basin,
							  struct hillslope_object *hillslope,
							  struct zone_object *zone,
							  struct patch_object *patch,
							  struct canopy_strata_object *stratum,
							  struct command_line_object *command_line,
							  struct	tec_entry	*event,
							  struct 	date current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local Function Declaration				*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);

	double compute_hourly_rain_stored(
		int,
		double  *,
		struct  canopy_strata_object *);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double rain_throughfall;
	double NO3_throughfall;
	double NO3_stored;
	double tmp1;
	double tmp2;
	rain_throughfall = 0;
	NO3_stored = 0;
	NO3_throughfall=0;

	/*--------------------------------------------------------------*/
	/*	Create canopy stratum hourly object.						*/
	/*--------------------------------------------------------------*/
	stratum[0].hourly = (struct canopy_strata_hourly_object*)
		alloc(1 * sizeof(struct canopy_strata_hourly_object),
		"hourly","canopy_stratum_hourly");
	rain_throughfall = patch[0].hourly[0].rain_throughfall;
	if ((zone[0].hourly_rain_flag == 1) && ( rain_throughfall > 0.0)) {
		stratum[0].rain_stored = compute_hourly_rain_stored(
			command_line[0].verbose_flag,
			&(rain_throughfall),
			stratum);
		patch[0].rain_throughfall_final += rain_throughfall * stratum[0].cover_fraction;
	}
	/*--------------------------------------------------------------*/
	/* update the NO3 stored in canopy and NO3 throughfall		*/
	/*--------------------------------------------------------------*/


	NO3_stored = 0;
        NO3_throughfall = 0;

	if (stratum[0].rain_stored > 0){
	    NO3_stored = (stratum[0].rain_stored) / (stratum[0].rain_stored + rain_throughfall) 
		* (stratum[0].NO3_stored + patch[0].hourly[0].NO3_throughfall);
	    NO3_throughfall = (rain_throughfall) / (stratum[0].rain_stored + rain_throughfall) 
		* (stratum[0].NO3_stored + patch[0].hourly[0].NO3_throughfall);
	    tmp1 = (stratum[0].rain_stored) / (stratum[0].rain_stored + rain_throughfall) ;
	    tmp2 = (rain_throughfall) / (stratum[0].rain_stored + rain_throughfall) ;
		
	}
	else{//stratum[0].rain_stored == 0 
	    if (rain_throughfall > 0){
		NO3_stored = (stratum[0].rain_stored) 
	      	/ (stratum[0].rain_stored + rain_throughfall) 
		* (stratum[0].NO3_stored + patch[0].hourly[0].NO3_throughfall);

		NO3_throughfall =  (rain_throughfall)
		/ (stratum[0].rain_stored + rain_throughfall) 
		* (stratum[0].NO3_stored + patch[0].hourly[0].NO3_throughfall); 
	    }
	    else{// rain_throughfall == 0 
                NO3_stored = stratum[0].NO3_stored + patch[0].hourly[0].NO3_throughfall;
		NO3_throughfall = 0;
	    }
	}



	patch[0].hourly[0].NO3_throughfall_final += NO3_throughfall 
		* stratum[0].cover_fraction;
	stratum[0].NO3_stored = NO3_stored;

	
	/*--------------------------------------------------------------*/
	/*	Destroy the canopy stratum hourly object.					*/
	/*--------------------------------------------------------------*/
	free( stratum[0].hourly );
	return;
} /*end canopy_stratum_hourly.c*/
