/*--------------------------------------------------------------*/
/* 								*/
/*		compute_snow_stored				*/
/*								*/
/*	NAME							*/
/*	compute_snow_stored - snow intercepted by stratum and 	*/
/*				updates snow falling through.	*/ 	
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_snow_stored( 	double	*,		*/
/*			struct canopy_strata_object	*);	*/
/*								*/
/*	OPTIONS							*/
/*	*snow (m) - amount of snow falling on stratum.		*/
/*	*stratum  - canopy stratum object			*/
/*								*/
/*	returns:						*/
/*	snow_storage (m) - snow in canopy at end of day		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine computes the change in snow storage on the	*/
/*	stratum by sublimation or melt  and snowfall.  It	*/
/*	assumes that potential sublimation/melt is only limited */
/*	available Kstar and actual sublimation/melt is limited 	*/
/*	potnetial sublimation and available snow from storage 	*/
/*	and snowfall.						*/
/*								*/
/*	It assumes that if mean daily air temp > 0 the 		*/
/*	snow melts rather than sublimates and this melt is 	*/
/*	then considered throughfall of rain!! Note that for	*/
/*	convenience we have set the snowpack density to	that	*/
/*	of water.  Thgis could be fed in later.			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	May 1, 1997 RAF						*/
/*	Added checks to make sure that if no part of an 	*/
/*	irradiance stream was used the stream is not updated.	*/
/*	May 16, 1997 C.Tague					*/
/*	Updates of stratum radiation moved to after calc. of	*/
/*	radiation fractions   					*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_snow_stored(
							int	verbose_flag,
							double	Tair_mean_day,
							double  vp_air,
							double  gasnow,
							double  pa,
							double	snow_density,
							double	*snow,
							double  *rain,
							struct	canopy_strata_object	*stratum)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration.				*/
	/*--------------------------------------------------------------*/
	double	compute_potential_snow_interception(
		int,
		double,
		double,
		struct  canopy_strata_object    *);
	double  compute_snow_sublimation(int, 
									 double,
									 double,
									 double,
									 double,
									 double,
									 double,
									 double,
									 double,
									 double *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	fraction_direct_K_used;
	double	fraction_diffuse_K_used;
	double	fraction_direct_APAR_used;
	double	fraction_diffuse_APAR_used;
	double fraction_L_used;
	double	latent_heat;
	double	potential_interception;
	double	potential_sublimation;
	double	snow_storage;
	double	storage_sublimated;
	double	potential_interception_sublimated;
	double	Rnet_used;
	double	APAR_used;
	double	throughfall;
	double unload;
	double tmp, tmp2, tmp3, tmp4, tmp5;
	double radsubl;
	
	tmp = 0.0;
	tmp2 = 0.0;
	tmp3 = 0.0;
	tmp4 = 0.0;
	tmp5 = 0.0;
	unload = 0.0;
	radsubl = 0.0;
	
	fraction_L_used = 0.0;
	fraction_direct_K_used = 0.0;
	fraction_diffuse_K_used = 0.0;
	fraction_direct_APAR_used = 0.0;
	fraction_diffuse_APAR_used = 0.0;
	
	APAR_used = 0.0;
	Rnet_used = 0.0;
	
	if( verbose_flag > 2)
		printf("%8.6f %8.6f ",*snow,  stratum[0].snow_stored);
	/*--------------------------------------------------------------*/
	/*	Determine if we are sublimating or melting this snow	*/
	/*	based on the daytime average temperature.		*/
	/*--------------------------------------------------------------*/
	if ( Tair_mean_day > 0 ){
		latent_heat = 334.0;
		}
	else {
		/*latent_heat = 2845.0;*/
		/* Eq. 3.19, Bras 1990 with conversion to KJ/kg */
		latent_heat = (677.0 - 0.07*Tair_mean_day) * 4.1868;
		}
	
	/* First, unload percent of stored snow in mass unloading */
	/* Set to 0.1 per day per Mahat 2011 (UEBVeg) hourly unloading rate */
	/* based on Hedstrom & Pomeroy 1998 obs. */
	unload = 0.1 * stratum[0].snow_stored;
	unload = 0.0;
	stratum[0].snow_stored -= unload;
	
	/*--------------------------------------------------------------*/
	/*	Compute amount potentially intercepted.			*/
	/*								*/
	/*	Defined as the minimum of the amount the stratum can cat*/
	/*	and the amount that fell from the stratum (or sky) above*/
	/*--------------------------------------------------------------*/
	potential_interception = compute_potential_snow_interception(
		verbose_flag,
		*snow,
		Tair_mean_day,
		stratum);
	/*--------------------------------------------------------------*/
	/*	Sublimate the snow already stored and the potential intercep*/
	/*	tion.  Note that we made use of yesterday's albedo for this	*/
	/*	computations radiation input so there may be small problems */
	/*	with the first day of snowfall.				*/
	/*								*/
	/*	mm snow / day = (Kj/m2*day + Kj/m2*day)/(Kj/kg snow)	*/
	/*					* (m3 snow/Kgsnow)	*/
	/*								*/
	/*	Note we assume 1 m3 snow = snow_density kg snow		*/
	/*--------------------------------------------------------------*/
	/*potential_sublimation = ((stratum[0].Kstar_direct + stratum[0].Kstar_diffuse)
		/ latent_heat ) * (1/snow_density);*/
	/*---------------------------------------------------------------*/
	/* Now calculating sublimation using compute_sublimation routine */
	/*---------------------------------------------------------------*/
	
if (Tair_mean_day > 0) {
		/* this is really melt */
		potential_sublimation = ((stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar) / latent_heat ) * (1/snow_density);
		potential_sublimation = max(potential_sublimation, 0.0);
		storage_sublimated = min(potential_sublimation,stratum[0].snow_stored);
	}
	else {
		potential_sublimation = compute_snow_sublimation(
			verbose_flag,
			Tair_mean_day,
			min(Tair_mean_day,0.0),
			vp_air,
			100000000.0, /* no capacity limit, restricting later */
			gasnow,
			stratum[0].epv.height,
			stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar,
			pa,
			&(radsubl));
		potential_sublimation = max(potential_sublimation, 0.0);
		/* Per Lundberg 1994, evap rates reduced for unsaturated canopies, approximated by power function */
		/*storage_sublimated = min(potential_sublimation * pow(stratum[0].snow_stored/(stratum[0].epv.all_pai * 
									stratum[0].defaults[0][0].specific_snow_capacity),2), stratum[0].snow_stored);*/
		storage_sublimated = min(max(potential_sublimation,0.0),stratum[0].snow_stored);
	}
	if (verbose_flag == -5) {
		printf("\n          SNOW STORED:Tair=%lf vp=%lf snow=%lf stor=%lf maxstor=%lf potint=%lf gasnow=%lf\n               ht=%lf K=%lf L=%lf pa=%lf subl_pot=%lf pai=%lf snowcap=%lf",
		   Tair_mean_day,
		   vp_air,
		   *snow,
		   stratum[0].snow_stored,
		   (stratum[0].epv.all_pai * stratum[0].defaults[0][0].specific_snow_capacity),
		   potential_interception,
		   gasnow,
		   stratum[0].epv.height,
		   (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse)/86.4,
		   stratum[0].Lstar/86.4,
		   pa,
		   potential_sublimation,
			   stratum[0].epv.all_pai,
			   stratum[0].defaults[0][0].specific_snow_capacity);
	}
	/*--------------------------------------------------------------*/
	/*	Compute amount of storage sublimated.			*/
	/*--------------------------------------------------------------*/
	/*storage_sublimated = min(potential_sublimation,stratum[0].snow_stored);*/

	/*--------------------------------------------------------------*/
	/*	Update amount of snow in storage after sublimation.	*/
	/*--------------------------------------------------------------*/
	stratum[0].snow_stored  -= storage_sublimated;
	if( verbose_flag > 2)
		printf("%8.6f %8.6f %8.6f %8.6f ",
		potential_interception,potential_sublimation,storage_sublimated,
		stratum[0].snow_stored);
	/*--------------------------------------------------------------*/
	/*	Update potentail sublimation.				*/
	/*--------------------------------------------------------------*/
	tmp5 = potential_sublimation;
	potential_sublimation -= storage_sublimated;
	/*--------------------------------------------------------------*/
	/*	Remove snow potentially intercepted from throughfall.	*/
	/*								*/
	/*	We will later add back what could not be actually intercepte*/
	/*--------------------------------------------------------------*/
	throughfall  = *snow - potential_interception;
	/*--------------------------------------------------------------*/
	/*	Compute amount of potential interception sublimated.	*/
	/*--------------------------------------------------------------*/
	potential_interception_sublimated  =
		min( potential_sublimation, potential_interception);
	/*--------------------------------------------------------------*/
	/*	Update the potential interception after sublimation.	*/
	/*--------------------------------------------------------------*/
	potential_interception -= potential_interception_sublimated;
	if( verbose_flag > 2)
		printf("%8.6f %8.6f %8.6f", throughfall,potential_interception_sublimated,
		potential_interception);
	
	/*--------------------------------------------------------------*/
	/*	Update snow throughfall.				*/
	/*								*/
	/*	Update snow storage					*/
	/*--------------------------------------------------------------*/
	if (stratum[0].defaults[0][0].epc.veg_type != NON_VEG) {
		snow_storage = min( stratum[0].snow_stored
			+ potential_interception, stratum[0].epv.all_pai
			* stratum[0].defaults[0][0].specific_snow_capacity );
		throughfall += max(potential_interception - (snow_storage - stratum[0].snow_stored),0.0);
		*snow = throughfall + unload;
		}
	else {
		snow_storage = min( stratum[0].snow_stored
			+ potential_interception, stratum[0].defaults[0][0].specific_snow_capacity );
		throughfall += max(potential_interception - (snow_storage - stratum[0].snow_stored),0.0);
		*snow = throughfall + unload;
		}
	/* Removing small stores so that canopy albedo is not affected */
	if (snow_storage < 0.000001) {
		*snow += snow_storage;
		snow_storage = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	Compute amount of sublimation that happened.		*/
	/*--------------------------------------------------------------*/
	stratum[0].sublimation =  storage_sublimated +
		potential_interception_sublimated;
	
	if (verbose_flag == -5) {
		printf("\n               subl_act=%lf radsubl=%lf",stratum[0].sublimation,radsubl);
	}
	/*--------------------------------------------------------------*/
	/*	If in fact the sublimation was melt add the amount	*/
	/*	melted to the rain throughfall.				*/
	/*--------------------------------------------------------------*/
	if ( Tair_mean_day > 0 ){
		*rain += stratum[0].sublimation;
		stratum[0].canopy_drip = stratum[0].sublimation;
		stratum[0].sublimation = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	Update amount of energy available for this stratum.	*/
	/*	We first get rid of longwave.				*/
	/*--------------------------------------------------------------*/
	/* NEW ADJUSTMENT FOR SUBLIM DUE TO RADIATION */
	/*Rnet_used =  stratum[0].sublimation * radsubl * latent_heat * snow_density;*/
	Rnet_used =  stratum[0].sublimation * latent_heat * snow_density;
		
	if (Rnet_used > 0.0) {
		if ( (stratum[0].Lstar > 0) && (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar > 0) ){
			fraction_L_used = stratum[0].Lstar 
							/ (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar);
			fraction_direct_K_used = stratum[0].Kstar_direct 
							/ (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar);
			fraction_diffuse_K_used = stratum[0].Kstar_diffuse 
							/ (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse + stratum[0].Lstar);
			}
		else if ( stratum[0].Kstar_direct + stratum[0].Kstar_diffuse > 0.0 ) {
			fraction_direct_K_used = stratum[0].Kstar_direct 
							/ (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse);
			fraction_diffuse_K_used = stratum[0].Kstar_diffuse 
							/ (stratum[0].Kstar_direct + stratum[0].Kstar_diffuse);
		}
	
		
		if ( stratum[0].APAR_direct > 0 ){
			fraction_direct_APAR_used = 1.0
							/ ( 1 + stratum[0].APAR_diffuse/stratum[0].APAR_direct);
			}
		else
			fraction_direct_APAR_used = 0.0;
	
		if (stratum[0].APAR_diffuse > 0 ){
			fraction_diffuse_APAR_used = 1.0
							/ ( 1 + stratum[0].APAR_direct/stratum[0].APAR_diffuse);
			}
		else
			fraction_diffuse_APAR_used = 0.0;
	

		APAR_used = ( ( (fraction_direct_K_used * Rnet_used) + (fraction_diffuse_K_used * Rnet_used) )
						/ ( stratum[0].Kstar_direct + stratum[0].Kstar_diffuse ) )
						* (stratum[0].APAR_direct + stratum[0].APAR_diffuse);
	
	
		tmp = stratum[0].APAR_direct;
		tmp2 = stratum[0].APAR_direct + stratum[0].APAR_diffuse;
		tmp3 = stratum[0].Kstar_direct;
		tmp4 = stratum[0].Kstar_direct + stratum[0].Kstar_diffuse;
	
		stratum[0].Lstar -= Rnet_used * fraction_L_used;
		stratum[0].Kstar_direct -= Rnet_used * fraction_direct_K_used;
		stratum[0].Kstar_diffuse -= Rnet_used * fraction_diffuse_K_used;
		stratum[0].APAR_direct -= APAR_used * fraction_direct_APAR_used;
		stratum[0].APAR_diffuse -= APAR_used * fraction_diffuse_APAR_used;
		
		/* Zero out negative APARs. K and Lstars ok since we are converting to temp change in canopy daily F */
		stratum[0].APAR_direct = max(stratum[0].APAR_direct,0.0);
		stratum[0].APAR_diffuse = max(stratum[0].APAR_diffuse,0.0);
	
		} /* end if rnet_used > 0 */
	

	
	if( verbose_flag > 2)
		printf("%8.6f %8.6f %8.6f %8.6f ", *snow,  stratum[0].sublimation,
		snow_storage, Rnet_used);
	
	if (verbose_flag == -5) {
		printf("\n                         STOREND:snow=%lf rain=%lf stratstor1000=%lf storage1000=%lf potint1000=%lf stratsubl1000=%lf Kdirused=%lf Kdifused=%lf Lused=%lf",
		   *snow,
		   *rain,
		   stratum[0].snow_stored*1000,
		   snow_storage*1000,
		   potential_interception*1000,
		   stratum[0].sublimation*1000,
		   (Rnet_used * fraction_direct_K_used)/86.4,
		   (Rnet_used * fraction_diffuse_K_used)/86.4,
		   (Rnet_used * fraction_L_used)/86.4);
	}
	
	if ( stratum[0].APAR_direct < -1 ) {
		printf("Tair=%lf pot_subl=%lf subl=%lf radsubl=%lf APARused=%lf APARdir=%lf APAR=%lf Rnet_used=%lf Kstardir=%lf Kstar=%lf Lstar=%lf \n", 
			   Tair_mean_day, 
			   tmp5, 
			   stratum[0].sublimation, 
			   radsubl, 
			   APAR_used, 
			   stratum[0].APAR_direct,
			   (stratum[0].APAR_direct+stratum[0].APAR_diffuse)/86.4,
			   Rnet_used/86.4, 
			   stratum[0].Kstar_direct/86.4,
			   (stratum[0].Kstar_direct+stratum[0].Kstar_diffuse)/86.4,
			   stratum[0].Lstar/86.4);
	}
	
	
	return( snow_storage);
} /*end compute_snow_stored*/
