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

int	compute_snag_decay(
					  //struct epconst_struct *epc,
					  //double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  //struct litter_c_object *cs_litr,
					  //struct litter_n_object *ns_litr,
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
	//double decay_rate1; // the decay rate previous time step
	//double decay_rate2; // the decay rate current time step, decay_rate1- decay_rate2 is the loss current time step
	double time_diff;
	double c_loss;
	double n_loss;
	struct date impact_day;
    double current_snagc;
    double current_snagn;

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

	current_snagc = cs->snagc;
	current_snagn = ns->snagn;

	cs->snagc =cs->snagc *pow(0.5, 1/(365.256*y_half)); // half life is 10 years, due to the cs-snagc is updating each time step so actually
	ns->snagn = ns->snagn * pow(0.5, 1/(365.256*y_half)); //live stem, use the cn ratio to from cs->snagc to calculate the snagn loss does here need to be this way




	c_loss = current_snagc - cs->snagc;
	n_loss = current_snagn - ns->snagn;

		/*another opotion */
	/*
	n_loss_to_cwdn = c_loss/epc.deadwood_cn //
	n_loss_to_litter = (n_loss - n_loss_to_cwdn) */

	};

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


	return(!ok);
} /*end compute_cwd_decay*/

