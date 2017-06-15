/*--------------------------------------------------------------*/
/* 								*/
/*			compute_cwd_decay.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_cwd_decay - 					*/
/*		computes physical fragmentation of coarse	*/
/*		woody debris into litter			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_cwd_decay( 				*/
/*					);			*/	
/*								*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	source from Peter Thornton, 1d_bgc, 1997		*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

int	compute_cwd_decay(
					  struct epconst_struct *epc,
					  double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct cdayflux_patch_struct *cdf,
					  struct ndayflux_patch_struct *ndf,
					  struct ndayflux_struct *ndf_stratum)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double cwdc_loss, kfrag;
	double rate_scalar;
	/*--------------------------------------------------------------*/
	/* calculate the flux from CWD to litter lignin and cellulose   */
	/*						 compartments, due to physical fragmentation */
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	for now use temperature and water scaleris calculated	*/
	/*	the previous day for soil decomposition limitations	*/
	/*--------------------------------------------------------------*/
	rate_scalar = cs_litr->w_scalar * cs_litr->t_scalar;
	rate_scalar = max(rate_scalar, 0.0);
	/* physical fragmentation of coarse woody debris */
	kfrag = epc->kfrag_base * rate_scalar;
	cwdc_loss = kfrag * cs->cwdc;
	cdf->cwdc_to_litr2c = cwdc_loss * epc->deadwood_fucel * cover_fraction;
	cdf->cwdc_to_litr3c = cwdc_loss * epc->deadwood_fscel * cover_fraction;
	cdf->cwdc_to_litr4c = cwdc_loss * epc->deadwood_flig * cover_fraction;
	ndf->cwdn_to_litr2n = cdf->cwdc_to_litr2c/CEL_CN;
	ndf->cwdn_to_litr3n = cdf->cwdc_to_litr3c/CEL_CN;
	ndf->cwdn_to_litr4n = cdf->cwdc_to_litr4c/LIG_CN;
	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/
	cs->cwdc -= (cdf->cwdc_to_litr2c + cdf->cwdc_to_litr3c
		+ cdf->cwdc_to_litr4c);
	cs_litr->litr2c += (cdf->cwdc_to_litr2c * cover_fraction);
	cs_litr->litr3c += (cdf->cwdc_to_litr3c * cover_fraction);
	cs_litr->litr4c += (cdf->cwdc_to_litr4c * cover_fraction);
	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/
	ns->cwdn -= (ndf->cwdn_to_litr2n + ndf->cwdn_to_litr3n
		+ ndf->cwdn_to_litr4n);
	ns_litr->litr2n += (ndf->cwdn_to_litr2n * cover_fraction);
	ns_litr->litr3n += (ndf->cwdn_to_litr3n * cover_fraction);
	ns_litr->litr4n += (ndf->cwdn_to_litr4n * cover_fraction);
	return(!ok);
} /*end compute_cwd_decay*/
