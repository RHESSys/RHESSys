/*--------------------------------------------------------------*/
/* 								                                */
/*			compute_redneedle_decay.c	                    	*/
/*							                                	*/
/*						                                		*/
/*	NAME				                            			*/
/*	compute_snag_decay -                         				*/
/*		computes the dead leaf pool decay to the               	*/
/*		litter pool, the dead leaf pool has a delay             */
/*       of 3 years and half time of 2 years, this can  		*/
/*		also be a input for the defs parameters			        */
/*	SYNOPSIS					                            	*/
/*	double	compute_redneedle_decay( 	                		*/
/*					);			                                */
/*							                                	*/
/*							                                	*/
/*	OPTIONS					                            		*/
/*							                                	*/
/*	DESCRIPTION			                            			*/
/*	base on the paper Steven Edburg et al., 2011                */
/* Journal of geophysical research					    		*/
/*					                                			*/
/*	4/26/2-18 N. REN. WSU                                   	*/
/*	PROGRAMMER NOTES		                        			*/
/*							                                	*/
/*							                                	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
#include <math.h>

int	compute_redneedle_decay(
					  struct epconst_struct epc,
					  //double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct cdayflux_patch_struct *cdf, //daily carbon flux
					  struct ndayflux_patch_struct *ndf,  // daily nitrogen flux
					  int year_attack,  // the year that start have beetle attack impact
					  int year_delay,
					  int half_life,
					  struct date current_day) // current day that

{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	int ok=1;
	double y_delay ; //snag pool delay time is 5 years, in the future the user can change this paramter from defs files
	double y_half ;  // snag pool decay rate half life is 10 years
	double time_diff;
	double c_loss;
	double n_loss;
	struct date impact_day;
    double current_redneedlec;
    double current_redneedlen;

    double redneedlec_to_litr1c;
	double redneedlec_to_litr2c;
	double redneedlec_to_litr3c;
	double redneedlec_to_litr4c;

    double redneedlen_to_litr1n;
	double redneedlen_to_litr2n;
	double redneedlen_to_litr3n;
	double redneedlen_to_litr4n;



	y_delay = year_delay;
	y_half = half_life;



	/*--------------------------------------------------------------*/
	/* calculate the flux from CWD to litter lignin and cellulose   */
	/*						 compartments, due to physical fragmentation */
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	for now use temperature and water scaleris calculated	*/
	/*	the previous day for soil decomposition limitations	*/
	/*--------------------------------------------------------------*/

	/* build the decay function */
	/* the precondition is that the days are larger than y_delay and the current larger than the impact_Day)*/
	impact_day.year = year_attack;
	impact_day.month = 1;
	impact_day.day =2;



	time_diff = julday(current_day)-julday(impact_day)-y_delay*365.256; // convert year to day then calculate the superscribe
	if (time_diff<=0) {
	ok =2; //ok =2 means the period that snag not happen or
	return ok;
	}
	else {

	current_redneedlec = cs->redneedlec;
	current_redneedlen = ns->redneedlen;

	cs->redneedlec =cs->redneedlec *pow(0.5, 1/(365.256*y_half)); // half life is 2 years, due to the cs-needle is updating each time step so actually
	ns->redneedlen = ns->redneedlen * pow(0.5, 1/(365.256*y_half)); // should I here also consider the n decay the same or calculated based on the cn ratio?


	c_loss = current_redneedlec - cs->redneedlec;
	n_loss = current_redneedlen - ns->redneedlen;

	};

  /* the following use the red needle _loss to move the  carbon and nitrogen to the litter pool */
  /* carbon */
	redneedlec_to_litr1c = c_loss * epc.leaflitr_flab;
	redneedlec_to_litr2c = c_loss * epc.leaflitr_fucel;
	redneedlec_to_litr3c = c_loss * epc.leaflitr_fscel;
	redneedlec_to_litr4c = c_loss * epc.leaflitr_flig; // all the epc fraction ration add together is 1


   /*nitrogen */

    redneedlen_to_litr2n = redneedlec_to_litr2c / CEL_CN; // should the flux have the fix ratio or the storage have the fix ratio for the litter pool
    redneedlen_to_litr3n = redneedlec_to_litr3c / CEL_CN;
    redneedlen_to_litr4n = redneedlec_to_litr4c / LIG_CN;
    redneedlen_to_litr1n = n_loss - (redneedlen_to_litr2n + redneedlen_to_litr3n + redneedlen_to_litr4n);
    redneedlen_to_litr1n = max(redneedlen_to_litr1n, 0.0);



	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/
    cs_litr->litr1c += redneedlec_to_litr1c;
    cs_litr->litr2c += redneedlec_to_litr2c;
    cs_litr->litr3c += redneedlec_to_litr3c;
    cs_litr->litr4c += redneedlec_to_litr4c;





	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/
   ns_litr->litr1n += redneedlen_to_litr1n;
   ns_litr->litr2n += redneedlen_to_litr2n;
   ns_litr->litr3n += redneedlen_to_litr3n;
   ns_litr->litr4n += redneedlen_to_litr4n;


	return(!ok);
} /*end compute_cwd_decay*/


