/*--------------------------------------------------------------*/
/* 								                                */
/*			update_deadroot_decay.c	                    	*/
/*							                                	*/
/*						                                		*/
/*	NAME				                            			*/
/*	compute_dead_root_decay -                         			*/
/*		computes the beetle caused dead root pool decay to the  */
/*		litter pool, the dead root pool has a delay             */
/*                  and half time of 2 years, this can  		*/
/*		also be a input for the defs parameters			        */
/*	SYNOPSIS					                            	*/
/*	double	update_deadroot_decay( 	                		*/
/*					);			                                */
/*							                                	*/
/*							                                	*/
/*	OPTIONS					                            		*/
/*							                                	*/
/*	DESCRIPTION			                            			*/
/*	base on the paper Da Luo et al., 2017                       */
/* MDPI forest decomposition of leaves and fine roots in three	*/
/*	subtropical plantations in china				            */
/* Solly et al 2014 plant soil. factor controlling decomposition*/
/* the litter decay rate is slower than leaf decay rate         */
/*	09/11/2019 N. REN. WSU                                   	*/
/*	PROGRAMMER NOTES		                        			*/
/*							                                	*/
/*							                                	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
#include <math.h>

void	update_deadroot_decay(
					  struct epconst_struct epc,
					  //double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct cdayflux_patch_struct *cdf, //daily carbon flux
					  struct ndayflux_patch_struct *ndf,  // daily nitrogen flux
					  int half_life)

{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/



	double y_half ;  // snag pool decay rate half life is 10 years
	double c_loss;
	double n_loss;
	double ratio;

    double deadrootc_to_litr1c;
	double deadrootc_to_litr2c;
	double deadrootc_to_litr3c;
	double deadrootc_to_litr4c;

    double deadrootn_to_litr1n;
	double deadrootn_to_litr2n;
	double deadrootn_to_litr3n;
	double deadrootn_to_litr4n;


    //due to it is inside of daily time step loop, so there is no need to include time differeneces
    //just simply calculate the daily loss rate, and substract the amount from the deadpool.

	y_half = half_life*365.256; //convert the year to daily


    ratio = (1-pow(0.5, (1/y_half)));

	c_loss = max(0,cs->dead_rootc_beetle*ratio);
	n_loss =max(0, ns->dead_rootn_beetle *ratio);

   // 	printf("\n the decay ratio are %lf, the c_loss is %lf, the current dead root pool are %lf", ratio, c_loss, cs->dead_root_beetle);


    cs->dead_rootc_beetle =cs->dead_rootc_beetle - c_loss;
	ns->dead_rootn_beetle =ns->dead_rootn_beetle - n_loss;

  /* the following use the dead root  _loss to move the  carbon and nitrogen to the litter pool */
  /* carbon */
	deadrootc_to_litr1c = c_loss * epc.leaflitr_flab;
	deadrootc_to_litr2c = c_loss * epc.leaflitr_fucel;
	deadrootc_to_litr3c = c_loss * epc.leaflitr_fscel;
	deadrootc_to_litr4c = c_loss * epc.leaflitr_flig; // all the epc fraction ration add together is 1


   /*nitrogen */

    deadrootn_to_litr2n = deadrootc_to_litr2c / CEL_CN; // should the flux have the fix ratio or the storage have the fix ratio for the litter pool
    deadrootn_to_litr3n = deadrootc_to_litr3c / CEL_CN;
    deadrootn_to_litr4n = deadrootc_to_litr4c / LIG_CN;
    deadrootn_to_litr1n = n_loss - (deadrootn_to_litr2n + deadrootn_to_litr3n + deadrootn_to_litr4n);
    deadrootn_to_litr1n = max(deadrootn_to_litr1n, 0.0);



	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/
    cs_litr->litr1c += deadrootc_to_litr1c;
    cs_litr->litr2c += deadrootc_to_litr2c;
    cs_litr->litr3c += deadrootc_to_litr3c;
    cs_litr->litr4c += deadrootc_to_litr4c;





	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/
   ns_litr->litr1n += deadrootn_to_litr1n;
   ns_litr->litr2n += deadrootn_to_litr2n;
   ns_litr->litr3n += deadrootn_to_litr3n;
   ns_litr->litr4n += deadrootn_to_litr4n;




} /*end compute_cwd_decay*/



