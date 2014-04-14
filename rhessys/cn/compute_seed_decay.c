/*--------------------------------------------------------------*/
/* 								*/
/*			compute_seed_decay.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_seed_decay - 					*/
/*		computes physical fragmentation of 	*/
/*		seed into litter			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_seed_decay( 				*/
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

int	compute_seed_decay(
					  struct epconst_struct *epc,
					  double cover_fraction,
					  struct cstate_struct *cs,
					  struct nstate_struct *ns,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct cdayflux_patch_struct *cdf,
					  struct ndayflux_patch_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double seedc_loss, kfrag;
	double rate_scalar;
	/*--------------------------------------------------------------*/
	/* calculate the flux from seed to litter lignin and cellulose   */
	/*						 compartments, due to physical fragmentation */
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	for now use temperature and water scaleris calculated	*/
	/*	the previous day for soil decomposition limitations	*/
	/*--------------------------------------------------------------*/
	rate_scalar = cs_litr->w_scalar * cs_litr->t_scalar;
	rate_scalar = max(rate_scalar, 0.0);
	/* physical fragmentation of coarse woody debris */
	kfrag = epc->kfrag_seed * rate_scalar;
	seedc_loss = kfrag * cs->seedc;
	cdf->seedc_to_litr2c = seedc_loss * epc->seed_fucel * cover_fraction;
	cdf->seedc_to_litr3c = seedc_loss * epc->seed_fscel * cover_fraction;
	cdf->seedc_to_litr4c = seedc_loss * epc->seed_flig * cover_fraction;
	ndf->seedn_to_litr2n = cdf->seedc_to_litr2c/CEL_CN;
	ndf->seedn_to_litr3n = cdf->seedc_to_litr3c/CEL_CN;
	ndf->seedn_to_litr4n = cdf->seedc_to_litr4c/LIG_CN;
	/*--------------------------------------------------------------*/
	/*	update carbon state variables				*/
	/*--------------------------------------------------------------*/
	cs->seedc -= (cdf->seedc_to_litr2c + cdf->seedc_to_litr3c
		+ cdf->seedc_to_litr4c);
	cs_litr->litr2c += (cdf->seedc_to_litr2c * cover_fraction);
	cs_litr->litr3c += (cdf->seedc_to_litr3c * cover_fraction);
	cs_litr->litr4c += (cdf->seedc_to_litr4c * cover_fraction);
	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables				*/
	/*--------------------------------------------------------------*/
	ns->seedn -= (ndf->seedn_to_litr2n + ndf->seedn_to_litr3n
		+ ndf->seedn_to_litr4n);
	ns_litr->litr2n += (ndf->seedn_to_litr2n * cover_fraction);
	ns_litr->litr3n += (ndf->seedn_to_litr3n * cover_fraction);
	ns_litr->litr4n += (ndf->seedn_to_litr4n * cover_fraction);
	return(!ok);
} /*end compute_seed_decay*/
