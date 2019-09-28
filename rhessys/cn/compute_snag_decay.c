/*--------------------------------------------------------------*/
/* 								                                */
/*			compute_snag_decay.c	                    		*/
/*							                                	*/
/*						                                		*/
/*	NAME				                            			*/
/*	compute_snag_decay -                         				*/
/*		computes the snag pool decay to the                 	*/
/*		woody debris into cwd pool, the snag pool               */
/*        has a delay of 5 years and half time 10 years			*/
/*						                                		*/
/*	SYNOPSIS					                            	*/
/*	double	compute_snag_decay( 	                			*/
/*					);			                                */
/*							                                	*/
/*							                                	*/
/*	OPTIONS					                            		*/
/*							                                	*/
/*	DESCRIPTION			                            			*/
/*	base on the paper Steven Edburg et al., 2011                */
/* Journal of geophysical research					    		*/
/*					                                			*/
/*	4/12/2-18 WSU                                       		*/
/*	PROGRAMMER NOTES		                        			*/
/*							                                	*/
/*							                                	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
#include <math.h>

void	compute_snag_decay(
					  //struct epconst_struct *epc,
					  //double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  //struct litter_c_object *cs_litr,
					  //struct litter_n_object *ns_litr,
					  struct cdayflux_patch_struct *cdf, //daily carbon flux
					  struct ndayflux_patch_struct *ndf,  // daily nitrogen flux
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

	double y_delay ; //snag pool delay time is 5 years, in the future the user can change this paramter from defs files
	double y_half ;  // snag pool decay rate half life is 10 years
	//double decay_rate1; // the decay rate previous time step
	//double decay_rate2; // the decay rate current time step, decay_rate1- decay_rate2 is the loss current time step
	double time_diff;
	double c_loss;
	double n_loss;
	//struct date impact_day;
   // double current_snagc;
    // current_snagn;
    double ratio;  // the flux/snag

	y_delay = year_delay;
	y_half = half_life*365.256; //convert the year to daily


    /* based on the calculation the snag decay have half life H*/
    /* from calculation we know that the flux out of snag pool is*/
    /* a fixed ratio=flux/snagpool, which is (1-0.5^(1/H)), so no
    /* mater when the different snag come to this ready to decay pool
    /* there is always a fixed decay ratio */


	/* build the decay function */
	/* the precondition is that the days are larger than y_delay and the current larger than the impact_Day)*/
	//impact_day.year = year_attack;
	//impact_day.month = 1;
	//impact_day.day =2;


	// calculate the the ratio
	ratio = (1-pow(0.5, 1/y_half));

	c_loss =  max(0,cs->snagc *ratio);
	n_loss = max(0,ns->snagn *ratio);
	//printf("\n the decay ratio are %lf, the c_loss is %lf, the current snag pool are %lf", ratio, c_loss, cs->snagc);

	//update the snag pool first

	cs->snagc -=c_loss;
	ns->snagn -=n_loss;

		/*another opotion */
	/*
	n_loss_to_cwdn = c_loss/epc.deadwood_cn //
	n_loss_to_litter = (n_loss - n_loss_to_cwdn) */

  /* the following use the snag_loss to move the snag carbon and nitrogen to the CWD pool */

	cdf->snagc_to_cwdc = c_loss;  // why the cwdc decay not go to 2 3 4 pool
    ndf->snagn_to_cwdn = n_loss;

	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/

	cs->cwdc += cdf->snagc_to_cwdc;

	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/

   ns->cwdn += ndf->snagn_to_cwdn;

   /* update the carbon daily flux go to cwd pool NREN 20190926 */

   cdf->stemc_to_cwdc += cdf->snagc_to_cwdc;

} /*end compute_cwd_decay*/

