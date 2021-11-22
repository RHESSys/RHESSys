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
	if ((cs_litr->litr1c > ZERO) && (ns_litr->litr1n > ZERO)) cn_l1 = cs_litr->litr1c/ns_litr->litr1n;
		else cn_l1 = LIVELAB_CN;
	if ((cs_litr->litr2c > ZERO) && (ns_litr->litr2n > ZERO)) cn_l2 = cs_litr->litr2c/ns_litr->litr2n;
		else cn_l2 = CEL_CN;
	if ((cs_litr->litr3c > ZERO) && (ns_litr->litr3n > ZERO))  cn_l3 = cs_litr->litr3c/ns_litr->litr3n;
		else cn_l3 = LIG_CN;
	if ((cs_litr->litr4c > ZERO) && (ns_litr->litr4n > ZERO))  cn_l4 = cs_litr->litr4c/ns_litr->litr4n;
		else cn_l4 = LIG_CN;

	cn_s1 = SOIL1_CN;
	cn_s2 = SOIL2_CN;
	cn_s3 = SOIL3_CN;
	cn_s4 = SOIL4_CN;

	/* labile litter fluxes */
	if (-1.0*ndf->pmnf_l1s1 > ZERO && cn_l1 > ZERO) {
	  ndf->do_litr1n_loss = -1.0*(DON_production_rate * ndf->pmnf_l1s1); //mineralization (negative value)
	  cdf->do_litr1c_loss = max(0,min(ndf->do_litr1n_loss * cn_l1, cs_litr->litr1c)); // adding boundary mechanism here
	  ndf->do_litr1n_loss = cdf->do_litr1c_loss/cn_l1; //back calculate the ndf
	}else{
	  ndf->do_litr1n_loss = 0.0;
	  cdf->do_litr1c_loss = 0.0;
	}

	/* cellulose litter fluxes */
	if (-1.0*ndf->pmnf_l2s2 > ZERO && cn_l2 > ZERO) {
	  ndf->do_litr2n_loss = -1.0*(DON_production_rate * ndf->pmnf_l2s2);
	  cdf->do_litr2c_loss = max(0,min(ndf->do_litr2n_loss * cn_l2, cs_litr->litr2c));
	  ndf->do_litr2n_loss = cdf->do_litr2c_loss/cn_l2;
	}else{
	  ndf->do_litr2n_loss = 0.0;
	  cdf->do_litr2c_loss = 0.0;
	}

	/* shielded cellulose litter fluxes */
	/* note these are based on lignan decay */
	if (-1.0*ndf->pmnf_l4s3 > ZERO && cn_l3 > ZERO) {
	  ndf->do_litr3n_loss = -1.0*(DON_production_rate * ndf->pmnf_l3l2);
	  cdf->do_litr3c_loss = max(0,min(ndf->do_litr3n_loss * cn_l3, cs_litr->litr3c));
	  ndf->do_litr3n_loss = cdf->do_litr3c_loss/cn_l3;
	}else{
	  ndf->do_litr3n_loss = 0.0;
	  cdf->do_litr3c_loss = 0.0;
	}
	/* lignan litter fluxes  */
	if (-1.0*ndf->pmnf_l4s3 > ZERO && cn_l4 > ZERO) {
	  ndf->do_litr4n_loss = -1.0*(DON_production_rate * ndf->pmnf_l4s3);
	  cdf->do_litr4c_loss = max(0,min(ndf->do_litr4n_loss * cn_l4, cs_litr->litr4c));
	  ndf->do_litr4n_loss = cdf->do_litr4c_loss/cn_l4;
	}else{
	  ndf->do_litr4n_loss = 0.0;
	  cdf->do_litr4c_loss = 0.0;
	}

	/* fast microbial recycling pool */
	/*if (ndf->pmnf_s1s2 < ZERO && cn_s1 > ZERO) {
	  ndf->do_soil1n_loss = -1.0*(DON_production_rate * ndf->pmnf_s1s2);
	  cdf->do_soil1c_loss = max(0,min(ndf->do_soil1n_loss * cn_s1, cs_soil->soil1c));
	  ndf->do_soil1n_loss = cdf->do_soil1c_loss/cn_s1;
	}else{
	  ndf->do_soil1n_loss = 0.0;
	  cdf->do_soil1c_loss = 0.0;
	} */

	/* medium microbial recycling pool */
	/*if (ndf->pmnf_s2s3 < ZERO && cn_s2 > ZERO) {
	  ndf->do_soil2n_loss = -1.0*(DON_production_rate * ndf->pmnf_s2s3);
	  cdf->do_soil2c_loss = max(0,min(ndf->do_soil2n_loss * cn_s2, cs_soil->soil2c));
	  ndf->do_soil2n_loss = cdf->do_soil2c_loss/cn_s2;
	}else{
	  ndf->do_soil2n_loss = 0.0;
	  cdf->do_soil2c_loss = 0.0;
	} */

	/* slow microbial recycling pool */
	/*if (ndf->pmnf_s3s4 < ZERO && cn_s3 > ZERO) {
	  ndf->do_soil3n_loss = -1.0*(DON_production_rate * ndf->pmnf_s3s4);
	  cdf->do_soil3c_loss = max(0,min(ndf->do_soil3n_loss * cn_s3, cs_soil->soil3c));
	  ndf->do_soil3n_loss = cdf->do_soil3c_loss/cn_s3;
	}else{
	  ndf->do_soil3n_loss = 0.0;
	  cdf->do_soil3c_loss = 0.0;
	} */
	/* recalcitrant SOM pool (rf = 1.0, always mineralizing) */
	/*if (ndf->soil4n_to_sminn < ZERO && cn_s4 > ZERO) {
	  ndf->do_soil4n_loss = -1.0*(DON_production_rate * ndf->soil4n_to_sminn);
	  cdf->do_soil4c_loss = max(0,min(ndf->do_soil4n_loss * cn_s4, cs_soil->soil4c));
	  ndf->do_soil4n_loss = cdf->do_soil4c_loss/cn_s4;
	}else{
	  ndf->do_soil4n_loss = 0.0;
	  cdf->do_soil4c_loss = 0.0;
	} */

	/* update soild and litter stores, soil pool will be updated after underground litter decomposes */
	cs_litr->litr1c       -= cdf->do_litr1c_loss;
	cs_litr->litr3c       -= cdf->do_litr3c_loss;
	cs_litr->litr2c       -= cdf->do_litr2c_loss;
	cs_litr->litr4c       -= cdf->do_litr4c_loss;
	/*cs_soil->soil1c       -= cdf->do_soil1c_loss;
	cs_soil->soil2c       -= cdf->do_soil2c_loss;
	cs_soil->soil3c       -= cdf->do_soil3c_loss;
	cs_soil->soil4c       -= cdf->do_soil4c_loss; */

	ns_litr->litr1n       -= ndf->do_litr1n_loss;
	ns_litr->litr2n       -= ndf->do_litr2n_loss;
	ns_litr->litr3n       -= ndf->do_litr3n_loss;
	ns_litr->litr4n       -= ndf->do_litr4n_loss;
	/*ns_soil->soil1n       -= ndf->do_soil1n_loss;
	ns_soil->soil2n       -= ndf->do_soil2n_loss;
	ns_soil->soil3n       -= ndf->do_soil3n_loss;
	ns_soil->soil4n       -= ndf->do_soil4n_loss; */



	/*cs_soil->DOC +=  (cdf->do_soil1c_loss + cdf->do_soil2c_loss + cdf->do_soil3c_loss + cdf->do_soil4c_loss);
	ns_soil->DON  +=  ( ndf->do_soil1n_loss + ndf->do_soil3n_loss + ndf->do_soil2n_loss + ndf->do_soil4n_loss); */

    /* above is above ground litter decomposition and below is underground litter decomposition */

    	/* calculate litter and soil compartment C:N ratios */
	if (ns_litr->litr1n_bg > ZERO) cn_l1 = cs_litr->litr1c_bg/ns_litr->litr1n_bg;
		else cn_l1 = LIVELAB_CN;
	if (ns_litr->litr2n_bg > ZERO) cn_l2 = cs_litr->litr2c_bg/ns_litr->litr2n_bg;
		else cn_l2 = CEL_CN;
	if (ns_litr->litr3n_bg > ZERO) cn_l3 = cs_litr->litr3c_bg/ns_litr->litr3n_bg;
		else cn_l3 = LIG_CN;
	if (ns_litr->litr4n_bg > ZERO) cn_l4 = cs_litr->litr4c_bg/ns_litr->litr4n_bg;
		else cn_l4 = LIG_CN;

	cn_s1 = SOIL1_CN;
	cn_s2 = SOIL2_CN;
	cn_s3 = SOIL3_CN;
	cn_s4 = SOIL4_CN;

	/* labile litter fluxes */
	if (-1.0*ndf->pmnf_l1s1_bg > ZERO) {
		ndf->do_litr1n_loss_bg = DON_production_rate * -1.0*ndf->pmnf_l1s1_bg;
		cdf->do_litr1c_loss_bg = ndf->do_litr1n_loss_bg * cn_l1;
		}
	/* cellulose litter fluxes */
	if (-1.0*ndf->pmnf_l2s2_bg > ZERO) {
		ndf->do_litr2n_loss_bg = DON_production_rate * -1.0*ndf->pmnf_l2s2_bg;
		cdf->do_litr2c_loss_bg = ndf->do_litr2n_loss_bg * cn_l2;
		}

	/* shielded cellulose litter fluxes */
	/* note these are based on lignan decay */
	if (-1.0*ndf->pmnf_l4s3_bg > ZERO) {
		ndf->do_litr3n_loss_bg = DON_production_rate * -1.0*ndf->pmnf_l3l2_bg;
		cdf->do_litr3c_loss_bg = ndf->do_litr3n_loss_bg * cn_l3;
		}

	/* lignan litter fluxes  */
	if (-1.0*ndf->pmnf_l4s3_bg > ZERO) {
		ndf->do_litr4n_loss_bg = DON_production_rate * -1.0*ndf->pmnf_l4s3_bg;
		cdf->do_litr4c_loss_bg = ndf->do_litr4n_loss_bg * cn_l4;
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
	cs_litr->litr1c_bg       -= cdf->do_litr1c_loss_bg;
	cs_litr->litr3c_bg       -= cdf->do_litr3c_loss_bg;
	cs_litr->litr2c_bg       -= cdf->do_litr2c_loss_bg;
	cs_litr->litr4c_bg       -= cdf->do_litr4c_loss_bg;
	cs_soil->soil1c       -= cdf->do_soil1c_loss;
	cs_soil->soil2c       -= cdf->do_soil2c_loss;
	cs_soil->soil3c       -= cdf->do_soil3c_loss;
	cs_soil->soil4c       -= cdf->do_soil4c_loss;

	ns_litr->litr1n_bg       -= ndf->do_litr1n_loss_bg;
	ns_litr->litr2n_bg       -= ndf->do_litr2n_loss_bg;
	ns_litr->litr3n_bg       -= ndf->do_litr3n_loss_bg;
	ns_litr->litr4n_bg       -= ndf->do_litr4n_loss_bg;
	ns_soil->soil1n       -= ndf->do_soil1n_loss;
	ns_soil->soil2n       -= ndf->do_soil2n_loss;
	ns_soil->soil3n       -= ndf->do_soil3n_loss;
	ns_soil->soil4n       -= ndf->do_soil4n_loss;

	/*if((cdf->do_soil1c_loss + cdf->do_soil2c_loss + cdf->do_soil3c_loss + cdf->do_soil4c_loss) < ZERO){
	  printf("update_dissolved_organic negative: %lf\n", cdf->do_soil1c_loss + cdf->do_soil2c_loss + cdf->do_soil3c_loss + cdf->do_soil4c_loss );
	}

	if((cdf->do_litr1c_loss + cdf->do_litr2c_loss + cdf->do_litr3c_loss + cdf->do_litr4c_loss) < ZERO ){
	  printf("update_dissolved_organic negative: %lf\n", cdf->do_litr1c_loss + cdf->do_litr2c_loss + cdf->do_litr3c_loss + cdf->do_litr4c_loss );
	} */


    /* maybe add the DOC from below ground litter to soil pool */
	cs_soil->DOC +=  (cdf->do_soil1c_loss + cdf->do_soil2c_loss + cdf->do_soil3c_loss + cdf->do_soil4c_loss +
                      cdf->do_litr1c_loss_bg + cdf->do_litr2c_loss_bg + cdf->do_litr3c_loss_bg + cdf->do_litr4c_loss_bg);
	ns_soil->DON  +=  ( ndf->do_soil1n_loss + ndf->do_soil3n_loss + ndf->do_soil2n_loss + ndf->do_soil4n_loss +
                       ndf->do_litr1n_loss_bg + ndf->do_litr2n_loss_bg + ndf->do_litr3n_loss_bg + ndf->do_litr4n_loss_bg);



	return (!ok);
} /* end update_dissolved_organic_losses.c */
