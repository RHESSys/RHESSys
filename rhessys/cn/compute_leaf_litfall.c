/*--------------------------------------------------------------*/
/* 								*/
/*			compute_leaf_litfall.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_leaf_litfall - computes leaf litfall and 	*/
/*				updates carbon stores		*/
/*				for patch.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_leaf_litfall( 				*/
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

int	compute_leaf_litfall(
						 struct epconst_struct epc,
						 double litfallc,
						 double cover_fraction,
						 struct cstate_struct *cs,
						 struct nstate_struct *ns,
						 struct litter_c_object *cs_litr,
						 struct litter_n_object *ns_litr,
						 struct cdayflux_patch_struct *cdf,
						 struct ndayflux_patch_struct *ndf,
						 struct cdayflux_struct *cdf_stratum,
						 struct ndayflux_struct *ndf_stratum,
						 int grow_flag)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;
	double c1,c2,c3,c4;
	double n1,n2,n3,n4;
	double nretrans, nloss;
	double avg_cn;
	
	avg_cn = cs->leafc/ns->leafn;

	/*------------------------------------------------------*/
	/*	Don't allow more leaves to fall than exist	*/
	/*------------------------------------------------------*/
	litfallc = min(cs->leafc, litfallc);
	/*------------------------------------------------------*/
	/*	determine carbon and nitrgoen to labile, cellulose and lignan pools */
	/*------------------------------------------------------*/
	c1 = litfallc * epc.leaflitr_flab;
	c2 = litfallc * epc.leaflitr_fucel;
	n2 = litfallc * epc.leaflitr_fucel/CEL_CN;
	c3 = litfallc * epc.leaflitr_fscel;
	n3 = litfallc * epc.leaflitr_fscel/CEL_CN;
	c4 = litfallc * epc.leaflitr_flig;
	n4 = litfallc * epc.leaflitr_flig/LIG_CN;
	n1 = ((c1+c2+c3+c4)/epc.leaflitr_cn)-n2-n3-n4;
	if (grow_flag > 0)
		nretrans = (litfallc/avg_cn) - (n1+n2+n3+n4);
	else
		nretrans = 0.0;
	/* test for negative or zero labile litter N */

	nretrans= max(nretrans, 0.0);
	n1 = max(n1, 0.0);

	if ((n1 < -ZERO) && (grow_flag > 0)){
		printf("Error in COMPUTE_LEAF_LITFALL - for litfallc %lf\n", litfallc);
		printf("Initial leaf litter labile N <= 0.0 (%lf)\n", n1);
		printf("Either lower the leaf litter C:N, or increase the\n");
		printf("fractions of lignin and/or cellulose in the leaf litter.\n");
		ok=0;
	}
	/* test for negative retranslocation */
	if (nretrans < -ZERO){
		printf("Error in leaf_litfall()\n");
		printf("leaf litter N retranslocation < 0.0 (%lf)\n",nretrans);
		printf("Raise leaf litter C:N, or check allocation or initial leaf\n");
		printf("C:N for excessively low value\n");
		nretrans = 0.0;
		ok=0;
	}
	if ((epc.veg_type == GRASS) || (epc.veg_type == C4GRASS)){
		cdf_stratum->leafc_to_deadleafc = litfallc;
		/* update state variables */
		cs->leafc -= cdf_stratum->leafc_to_deadleafc;
		cs->dead_leafc += cdf_stratum->leafc_to_deadleafc;
		if ((epc.allocation_flag == STATIC) || (grow_flag == 0) )
			cs->leafc_store += litfallc;
		/* nitrogen state variable updates */
		if (grow_flag > 0)
			nloss = n1+n2+n3+n4+nretrans;
		else
			nloss = litfallc/avg_cn;
		ns->retransn += nretrans;
		ns->leafn -= nloss;
		if ((epc.allocation_flag == STATIC) || (grow_flag == 0) )
			ns->leafn_store += nloss;
		ndf_stratum->leafn_to_deadleafn = nloss - nretrans ;
		ns->dead_leafn += ndf_stratum->leafn_to_deadleafn;
	}
	else{
		/* set fluxes in daily flux structure */
		if (grow_flag > 0) {
		cdf->leafc_to_litr1c += c1 * cover_fraction;
		cdf->leafc_to_litr2c += c2 * cover_fraction;
		cdf->leafc_to_litr3c += c3 * cover_fraction;
		cdf->leafc_to_litr4c += c4 * cover_fraction;
		ndf->leafn_to_litr1n += n1 * cover_fraction;
		ndf->leafn_to_litr2n += n2 * cover_fraction;
		ndf->leafn_to_litr3n += n3 * cover_fraction;
		ndf->leafn_to_litr4n += n4 * cover_fraction;
		}
		/* update state variables */
		cs->leafc -= litfallc;
		if ((epc.allocation_flag == STATIC) || (grow_flag == 0) )
			cs->leafc_store += litfallc;
		if (grow_flag > 0) {
		cs_litr->litr1c += c1 * cover_fraction;
		cs_litr->litr2c += c2 * cover_fraction;
		cs_litr->litr3c += c3 * cover_fraction;
		cs_litr->litr4c += c4 * cover_fraction;
		}
		/* nitrogen state variable updates */
		if (grow_flag > 0)
			nloss = n1+n2+n3+n4+nretrans;
		else
			nloss = litfallc/avg_cn;
		ns->retransn += nretrans;
		ns->leafn -= nloss;
		if ((epc.allocation_flag == STATIC) || (grow_flag == 0) )
			ns->leafn_store += nloss;
		if (grow_flag > 0) {
		ns_litr->litr1n += n1 * cover_fraction;
		ns_litr->litr2n += n2 * cover_fraction;
		ns_litr->litr3n += n3 * cover_fraction;
		ns_litr->litr4n += n4 * cover_fraction;
		}
	}
	return(0);
} /*compute_leaf_litfall*/ 
