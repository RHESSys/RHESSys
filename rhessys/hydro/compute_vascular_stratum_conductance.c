/*--------------------------------------------------------------*/
/* 								*/
/*			compute_vascular_stratum_conductance	*/
/*								*/
/*	compute_vascular_stratum_conductance - computes conductance to H20	*/
/*				of a single mean vascular stratum		*/
/*								*/
/*	NAME							*/
/*	compute_vascular_stratum_conductance 				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_vascular_stratum_conductance(			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This is essential a Jarvis model of multiplicative	*/
/*	factors.  Note that the multipliers are assumed to 	*/
/*	scale linearly with leaf area so we just scale by	*/
/*	LAI * stomatal_fraction to get stratum conductance.	*/
/*	However, the multipliers themselves should then be	*/
/*	designed to apply to the MEAN leaf in the canopy, we	*/
/*	do just this for the leaf_conductance_APAR_curve	*/
/*	By assuming that the irradiance distribution follows 	*/
/*	a negative exponential decay in the canopy.		*/
/*								*/
/*	AT the moment, the negative exponential decay is 	*/
/*	supplied from the calling routine as an extinction 	*/
/*	coefficient.  It is up to the user to ensure that the	*/
/*	extinction coefficient is appropriate (i.e. if the	*/
/*	irradiance is direct only it may be fine, for diffuse	*/
/*	irradiance it may have to be adjusted).			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Sep 2 1997 RAF									*/
/*	New code based on compute_stratum_conductance but now */
/*	labelled so it applies only to vascular strata.			*/
/*								*/
/*	Sep 3 1997 RAF						*/
/*	Made sure stratum conductance was non zero.		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_vascular_stratum_conductance(
											 int	verbose_flag,
											 int	LWP_curve,
											 double	APAR_coef,
											 double 	cuticular_cond,
											 double	stomatal_fraction,
											 double	LWP_min_spring,
											 double	LWP_stom_closure,
											 double	LWP_threshold,
											 double	LWP_slp,
											 double	LWP_intercpt,
											 double	stomatal_conductance_max,
											 double	topt,     
											 double	tcoef,
											 double	tmax,
											 double	vpd_open,
											 double	vpd_closed,
											 double	APAR,
											 double	LAI,
											 double	LWP_predawn,
											 double	Tair_min,
											 double	Tair_mean_day,
											 double	vpd,
											 double CO2,
											 double coef_CO2,
											 int ID,
											 struct	canopy_strata_object *stratum, 
											 struct patch_object *patch)
											 
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	double	leaf_conductance_APAR_curve(	double,
		double);

	double	leaf_conductance_CO2_curve(double, double);

	double	leaf_conductance_LWP_curve(int,	double,
		double,
		double,
		double,
		double,
		double);
	double	leaf_conductance_tavg_curve(	double,
		double,
		double,
		double);
	double	leaf_conductance_tmin_curve( 	double	);
	double	leaf_conductance_vpd_curve( 	double,
		double,
		double);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	stratum_conductance;
	double	m_CO2;
	double	m_APAR;
	double	m_final;
	double	m_LWP;
	double	m_tavg;
	double	m_tmin;
	double	m_vpd;
	double	stomatal_conductance, wilting_point;
	
	wilting_point = 0.0;
	
	/* wilting point as volumetric water content, so needs to be multiplied by rz depth to get water depth */
	wilting_point = exp(-1.0*log(-1.0*100.0*LWP_stom_closure/patch[0].soil_defaults[0][0].psi_air_entry) 
						* patch[0].soil_defaults[0][0].pore_size_index);
	wilting_point *= (min(patch[0].sat_deficit, patch[0].rootzone.potential_sat));
		
	/*--------------------------------------------------------------*/
	/*	incident PAR multiplier					*/
	/*--------------------------------------------------------------*/
	m_APAR = leaf_conductance_APAR_curve( 	APAR,	APAR_coef);
	/*--------------------------------------------------------------*/
	/*	CO2 multiplier						*/
	/*--------------------------------------------------------------*/
	m_CO2 = leaf_conductance_CO2_curve(CO2, coef_CO2); 
	/*--------------------------------------------------------------*/
	/*	soil-LWP multiplier					*/
	/*--------------------------------------------------------------*/
	if (LWP_predawn !=  9999.0) {
		m_LWP = leaf_conductance_LWP_curve(LWP_curve, LWP_predawn,
			LWP_min_spring,LWP_stom_closure,LWP_threshold, LWP_slp, LWP_intercpt );

		if ((patch[0].rz_storage > ZERO) && (patch[0].sat_deficit > patch[0].rootzone.potential_sat)) {

			/* Adjust check to make proportional to stratum-specific rooting depth */
			if ( (patch[0].rz_storage * stratum[0].rootzone.depth / patch[0].rootzone.depth ) < wilting_point) {
				m_LWP = 0.0;
					}
				}

			}
	else m_LWP=1.0;


	/*--------------------------------------------------------------*/
	/*	mean daytime temperature multiplier.			*/
	/*--------------------------------------------------------------*/
	m_tavg = leaf_conductance_tavg_curve(Tair_mean_day, topt, tmax, tcoef );
	/*--------------------------------------------------------------*/
	/*	freezing night minimum temperature multiplier		*/
	/*--------------------------------------------------------------*/
	m_tmin	= leaf_conductance_tmin_curve( 	Tair_min );
	/*--------------------------------------------------------------*/
	/*	vapour pressure deficit multiplier, vpd in Pa		*/
	/*--------------------------------------------------------------*/
	m_vpd	= leaf_conductance_vpd_curve(vpd, vpd_open, vpd_closed);
	/*--------------------------------------------------------------*/
	/*	Apply multipliers to max stomatal conductance		*/
	/*--------------------------------------------------------------*/
	m_final = m_APAR * m_tavg * m_LWP * m_CO2 * m_tmin * m_vpd;
	stomatal_conductance = stomatal_conductance_max * m_final;

	/*--------------------------------------------------------------*/
	/*	Limit stomoatal conductance to cuticular conductance	*/
	/*--------------------------------------------------------------*/
	stratum_conductance =  stomatal_conductance*LAI*stomatal_fraction;


	/*--------------------------------------------------------------*/
	/*	Ensure that streatum conductance is non-zero.		*/
	/*--------------------------------------------------------------*/
	stratum_conductance = max(stratum_conductance,0.0000001);


	stratum[0].mult_conductance.APAR = m_APAR;			
	stratum[0].mult_conductance.tavg = m_tavg;			
	stratum[0].mult_conductance.LWP = m_LWP;			
	stratum[0].mult_conductance.CO2 = m_CO2;			
	stratum[0].mult_conductance.tmin = m_tmin;			
	stratum[0].mult_conductance.vpd = m_vpd;
	
	if ( verbose_flag == -5 ){
		printf("\nVAS STRAT CONDUCT END: m_APAR=%lf m_tavg=%lf m_LWP=%lf m_CO2=%lf m_tmin=%lf m_vpd=%lf m_final=%lf lai=%lf frac=%lf glmax=%lf gl=%lf pchWP=%lf stratWP=%lf",
			   m_APAR,
			   m_tavg, 
			   m_LWP,
			   m_CO2,
			   m_tmin,
			   m_vpd,
			   m_final,
			   LAI,
			   stomatal_fraction,
			   stomatal_conductance_max,
			   stratum_conductance,
			   patch[0].wilting_point,
			   wilting_point);
	}
	
	
	return(stratum_conductance);
} /*end compute_vascular_stratum_conductance*/
