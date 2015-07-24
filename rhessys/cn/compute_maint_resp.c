/*--------------------------------------------------------------*/
/* 								*/
/*			compute_maint_resp.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_maint_resp - 					*/
/*		computes maintenance respiration		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_maint_resp( 				*/
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

int	compute_maint_resp(	double q10,
					   double mrpern,
					   struct	cstate_struct *cs,
					   struct	nstate_struct *ns,
					   struct epconst_struct *epc,
					   struct	metvar_struct *metv,
					   struct cdayflux_struct *cdf)
{
/*----------------------------------------------------------------------
maintenance respiration routine
Uses reference values at 20 deg C and an empirical relationship between
tissue N content and respiration rate given in:

	Ryan, M.G., 1991. Effects of climate change on plant respiration.
	Ecological Applications, 1(2):157-167.
	
	 Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	 coarse and fine roots.
	 
	  From Ryan's figures and regressions equations, the maintenance respiration
	  in kgC/day per kg of tissue N is:
	  mrpern = 0.218 (kgC/kgN/d)
	  
	   change this to an input default variable,
	   
		Leaf maintenance respiration is calculated separately for day and
		night, since the PSN routine needs the daylight value.
		
		 Leaf and fine root respiration are dependent on phenology.
	----------------------------------------------------------------------*/
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/* temperature acclimation of Q10 is taken from Tjoelker et al., GCB 2001, 7, 223-230 */
	/*------------------------------------------------------*/
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;
	double t1;
	double exponent;

	if (epc->Tacclim == 1)
		q10 = epc->Tacclim_intercpt - epc->Tacclim_slp * cs->Tacc;

	/* leaf day and night maintenance respiration when leaves on */
	if (cs->leafc > ZERO){
		t1 = ns->leafn * mrpern;
		/* leaf, day */
		exponent = (metv->tday - 20.0) / 10.0;
		cdf->leaf_day_mr = t1 * pow(q10, exponent) * metv->dayl / 86400.0;
		/* leaf, night */
		exponent = (metv->tnight - 20.0) / 10.0;
		cdf->leaf_night_mr = t1 * pow(q10, exponent)*(86400.0-metv->dayl)/86400.0;
	}
	else /* no leaves on */
		cdf->leaf_day_mr = cdf->leaf_night_mr = 0.0;
	/* fine root maintenance respiration when fine roots on */
	/* ammended to consider only the specified n concentration,
	to avoid excessive MR with n-loading to fine roots */
	if (cs->frootc > ZERO){
		exponent = (metv->tsoil - 20.0) / 10.0;
		t1 = pow(q10, exponent);
		cdf->froot_mr = ns->frootn * mrpern * t1;
	}
	else /* no fine roots on */
		cdf->froot_mr = 0.0;
	/* TREE-specific fluxes */
	if (epc->veg_type == TREE){
		/* live stem maintenance respiration */
		exponent = (metv->tavg - 20.0) / 10.0;
		t1 = pow(q10, exponent);
		cdf->livestem_mr = ns->live_stemn * mrpern * t1;
		/* live coarse root maintenance respiration */
		exponent = (metv->tsoil - 20.0) / 10.0;
		t1 = pow(q10, exponent);
		cdf->livecroot_mr = ns->live_crootn * mrpern * t1;
	}
	cdf->total_mr = cdf->livecroot_mr + cdf->livestem_mr + cdf->froot_mr
		+ cdf->leaf_day_mr + cdf->leaf_night_mr;

  return (!ok);
} /*compute_maint_resp*/
