/*--------------------------------------------------------------*/
/* 								*/
/*			compute_froot_litfall.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_froot_litfall - computes fine root litter fall	*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_froot_litfall( 				*/
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

int	compute_froot_litfall(
						  struct epconst_struct epc,
						  double litfallc,
						  double cover_fraction,
						  struct cstate_struct *cs,
						  struct nstate_struct *ns,
						  struct litter_c_object *cs_litr,
						  struct litter_n_object *ns_litr,
						  struct cdayflux_patch_struct *cdf,
						  struct ndayflux_patch_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double c1,c2,c3,c4;
	double n1,n2,n3,n4, nloss;
	double avg_cn;
	avg_cn = cs->frootc/ns->frootn;


	/*------------------------------------------------------*/
	/*	Don't allow more leaves to fall than exist	*/
	/*------------------------------------------------------*/
	litfallc = min(cs->frootc, litfallc);
	/*------------------------------------------------------*/
	/*	determine carbon and nitrgoen to labile, cellulose and lignan pools */
	/*------------------------------------------------------*/

	c1 = litfallc * epc.frootlitr_flab;
	c2 = litfallc * epc.frootlitr_fucel;
	n2 = litfallc * epc.frootlitr_fucel/CEL_CN;
	c3 = litfallc * epc.frootlitr_fscel;
	n3 = litfallc * epc.frootlitr_fscel/CEL_CN;
	c4 = litfallc * epc.frootlitr_flig;
	n4 = litfallc * epc.frootlitr_flig/LIG_CN;
	n1 = ((c1+c2+c3+c4)/avg_cn)-n2-n3-n4;
	/* test for negative or zero labile litter N */
	if (n1 <= -0.000000000001){
		printf("Error in froot_litfall()\n");
		printf("Initial froot litter labile N <= 0.0 (%lf)\n",n1);
		printf("Either lower the froot litter C:N, or increase the\n");
		printf("fractions of lignin and/or cellulose in the froot litter.\n");
		printf("\n c1 %lf c2 %lf c3 %lf c4 %lf, n1 %lf n2 %lf n3 %lf n4 %lf, avg_cn %lf, croot %lf, froot %lf",
			c1,c2,c3,c4,n1,n2,n3,n4,avg_cn,cs->frootc, ns->frootn);
		ok=0;
	}
	if (ok){
		/* set fluxes in daily flux structure */
		cdf->frootc_to_litr1c = c1 * cover_fraction;
		cdf->frootc_to_litr2c = c2 * cover_fraction;
		cdf->frootc_to_litr3c = c3 * cover_fraction;
		cdf->frootc_to_litr4c = c4 * cover_fraction;
		ndf->frootn_to_litr1n = n1 * cover_fraction;
		ndf->frootn_to_litr2n = n2 * cover_fraction;
		ndf->frootn_to_litr3n = n3 * cover_fraction;
		ndf->frootn_to_litr4n = n4 * cover_fraction;
		/* update state variables */
		cs->frootc -= litfallc;
		if (epc.allocation_flag == STATIC)
			cs->frootc_store += litfallc;
		cs_litr->litr1c += c1 * cover_fraction;
		cs_litr->litr2c += c2 * cover_fraction;
		cs_litr->litr3c += c3 * cover_fraction;
		cs_litr->litr4c += c4 * cover_fraction;
		nloss =  n1+n2+n3+n4;
		ns->frootn -= nloss;
		if (epc.allocation_flag == STATIC)
			ns->frootn_store += nloss;
		ns_litr->litr1n += n1 * cover_fraction;
		ns_litr->litr2n += n2 * cover_fraction;
		ns_litr->litr3n += n3 * cover_fraction;
		ns_litr->litr4n += n4 * cover_fraction;
	}
	return(0);
} /*compute_froot_litfall*/ 
