/*--------------------------------------------------------------*/
/* 								*/
/*		update_dissolved_organic_losses					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_dissolved_organic_losses -  					*/
/*		performs decomposition and updates soil/litter	*/
/*		carbon and nitrogen stores			*/
/*								*/
/*	SYNOPSIS						*/
/*	int update_dissolved_organic_losses(					*/
/*			double,					*/
/*			double,					*/
/*			double,					*/
/*			double,					*/
/*			struct	soil_c_object	*		*/
/*			struct	soil_n_object	*		*/
/*			struct	litter_c_object	*		*/
/*			struct	litter_n_object	*		*/
/*			struct	cdayflux_patch_object *		*/
/*			struct	ndayflux_patch_object *		*/
/*				)				*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	assume DON at 3% mineralization (Vitousek et al,2000	*/
/*	Within-system element cycles, Input-output budgets      */
/*	and nutrient limitation)				*/
/*	DOC losses then follow DON				*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_dissolved_organic_losses(
				  struct	date	current_date,
				  double 	DON_production_rate,
				  struct  soil_c_object   *cs_soil,
				  struct  soil_n_object   *ns_soil,
				  struct  litter_c_object *cs_litr,
				  struct  litter_n_object *ns_litr,
				  struct cdayflux_patch_struct *cdf,
				  struct ndayflux_patch_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double cn_l1,cn_l2,cn_l3,cn_l4,cn_s1,cn_s2,cn_s3,cn_s4;
	int ok;

	ok=1;
	/* calculate litter and soil compartment C:N ratios */
	if (ns_litr->litr1n > ZERO) cn_l1 = cs_litr->litr1c/ns_litr->litr1n;
		else cn_l1 = LIVELAB_CN;
	if (ns_litr->litr2n > ZERO) cn_l2 = cs_litr->litr2c/ns_litr->litr2n;
		else cn_l2 = CEL_CN;
	if (ns_litr->litr3n > ZERO) cn_l3 = cs_litr->litr3c/ns_litr->litr3n;
		else cn_l3 = CEL_CN;
	if (ns_litr->litr4n > ZERO) cn_l4 = cs_litr->litr4c/ns_litr->litr4n;
		else cn_l4 = LIG_CN;

	cn_s1 = SOIL1_CN;
	cn_s2 = SOIL2_CN;
	cn_s3 = SOIL3_CN;
	cn_s4 = SOIL4_CN;

	/* labile litter fluxes */
	if (-1.0*ndf->pmnf_l1s1 > ZERO) {
		ndf->do_litr1n_loss = DON_production_rate * -1.0*ndf->pmnf_l1s1;
		cdf->do_litr1c_loss = ndf->do_litr1n_loss * cn_l1;
		}
	/* cellulose litter fluxes */
	if (-1.0*ndf->pmnf_l2s2 > ZERO) {
		ndf->do_litr2n_loss = DON_production_rate * -1.0*ndf->pmnf_l2s2;
		cdf->do_litr2c_loss = ndf->do_litr2n_loss * cn_l2;
		}

	/* shielded cellulose litter fluxes */
	/* note these are based on lignan decay */
	if (-1.0*ndf->pmnf_l4s3 > ZERO) {
		ndf->do_litr3n_loss = DON_production_rate * -1.0*ndf->pmnf_l4s3;
		cdf->do_litr3c_loss = ndf->do_litr3n_loss * cn_l3;
		}

	/* lignan litter fluxes  */
	if (-1.0*ndf->pmnf_l4s3 > ZERO) {
		ndf->do_litr4n_loss = DON_production_rate * -1.0*ndf->pmnf_l4s3;
		cdf->do_litr4c_loss = ndf->do_litr4n_loss * cn_l4;
		}
	
	/* fast microbial recycling pool */
	if (-1.0*ndf->pmnf_s1s2 > ZERO) {
		ndf->do_soil1n_loss = DON_production_rate * -1.0*ndf->pmnf_s1s2;
		cdf->do_soil1c_loss = ndf->do_soil1n_loss * cn_s1;
		}

	/* medium microbial recycling pool */
	if (-1.0*ndf->pmnf_s2s3 > ZERO) {
		ndf->do_soil2n_loss = DON_production_rate * -1.0*ndf->pmnf_s2s3;
		cdf->do_soil2c_loss = ndf->do_soil2n_loss * cn_s2;
		}

	/* slow microbial recycling pool */
	if (-1.0*ndf->pmnf_s3s4 > ZERO) {
		ndf->do_soil3n_loss = DON_production_rate * -1.0*ndf->pmnf_s3s4;
		cdf->do_soil3c_loss = ndf->do_soil3n_loss * cn_s3;
		}

	/* recalcitrant SOM pool (rf = 1.0, always mineralizing) */
	if (-1.0*ndf->soil4n_to_sminn > ZERO) {
		ndf->do_soil4n_loss = DON_production_rate * -1.0*ndf->soil4n_to_sminn;
		cdf->do_soil4c_loss = ndf->do_soil4n_loss * cn_s4;
		}

	/* update soild and litter stores */
	cs_litr->litr1c       -= cdf->do_litr1c_loss;
	cs_litr->litr2c       -= cdf->do_litr2c_loss;
	cs_litr->litr4c       -= cdf->do_litr4c_loss;
	cs_soil->soil1c       -= cdf->do_soil1c_loss;
	cs_soil->soil2c       -= cdf->do_soil2c_loss;
	cs_soil->soil3c       -= cdf->do_soil3c_loss;
	cs_soil->soil4c       -= cdf->do_soil4c_loss;

	ns_litr->litr1n       -= ndf->do_litr1n_loss;
	ns_litr->litr2n       -= ndf->do_litr2n_loss;
	ns_litr->litr4n       -= ndf->do_litr4n_loss;
	ns_soil->soil1n       -= ndf->do_soil1n_loss;
	ns_soil->soil2n       -= ndf->do_soil2n_loss;
	ns_soil->soil3n       -= ndf->do_soil3n_loss;
	ns_soil->soil4n       -= ndf->do_soil4n_loss;

	cs_soil->DOC +=  (cdf->do_soil1c_loss + cdf->do_soil2c_loss + cdf->do_soil4c_loss);
	ns_soil->DON  +=  ( ndf->do_soil1n_loss + ndf->do_soil2n_loss + ndf->do_soil4n_loss);

	

	return (!ok);
} /* end update_dissolved_organic_losses.c */
