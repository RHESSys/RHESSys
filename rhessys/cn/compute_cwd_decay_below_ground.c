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

int	compute_cwd_decay_bg(
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
	rate_scalar = cs_litr->w_scalar_bg * cs_litr->t_scalar_bg;
	rate_scalar = max(rate_scalar, 0.0);
	/* physical fragmentation of coarse woody debris */
	kfrag = epc->kfrag_base * rate_scalar;
	cwdc_loss = kfrag * cs->cwdc_bg;
	cdf->cwdc_to_litr2c_bg = cwdc_loss * epc->deadwood_fucel * cover_fraction;
	cdf->cwdc_to_litr3c_bg = cwdc_loss * epc->deadwood_fscel * cover_fraction;
	cdf->cwdc_to_litr4c_bg = cwdc_loss * epc->deadwood_flig * cover_fraction;
	ndf->cwdn_to_litr2n_bg = cdf->cwdc_to_litr2c_bg/CEL_CN;
	ndf->cwdn_to_litr3n_bg = cdf->cwdc_to_litr3c_bg/CEL_CN;
	ndf->cwdn_to_litr4n_bg = cdf->cwdc_to_litr4c_bg/LIG_CN;
	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/
	cs->cwdc_bg -= (cdf->cwdc_to_litr2c_bg + cdf->cwdc_to_litr3c_bg
		+ cdf->cwdc_to_litr4c_bg);
	cs_litr->litr2c_bg += (cdf->cwdc_to_litr2c_bg * cover_fraction);
	cs_litr->litr3c_bg += (cdf->cwdc_to_litr3c_bg * cover_fraction);
	cs_litr->litr4c_bg += (cdf->cwdc_to_litr4c_bg * cover_fraction);
	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/
	ns->cwdn_bg -= (ndf->cwdn_to_litr2n_bg + ndf->cwdn_to_litr3n_bg
		+ ndf->cwdn_to_litr4n_bg);
	ns_litr->litr2n_bg += (ndf->cwdn_to_litr2n_bg * cover_fraction);
	ns_litr->litr3n_bg += (ndf->cwdn_to_litr3n_bg * cover_fraction);
	ns_litr->litr4n_bg += (ndf->cwdn_to_litr4n_bg * cover_fraction);
	return(!ok);
} /*end compute_cwd_decay*/
