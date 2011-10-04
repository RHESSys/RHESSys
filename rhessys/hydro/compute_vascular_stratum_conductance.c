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
											 double	APAR_coef,
											 double 	cuticular_cond,
											 double	stomatal_fraction,
											 double	LWP_min_spring,
											 double	LWP_stom_closure,
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

	double	leaf_conductance_LWP_curve(	double,
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
		m_LWP = leaf_conductance_LWP_curve(LWP_predawn,
			LWP_min_spring,LWP_stom_closure);

		if ((patch[0].rz_storage > ZERO) && (patch[0].sat_deficit > stratum[0].rootzone.potential_sat)) {
			wilting_point = exp(-1.0*log(-1.0*LWP_stom_closure/patch[0].soil_defaults[0][0].psi_air_entry) 
			* patch[0].soil_defaults[0][0].pore_size_index) * patch[0].soil_defaults[0][0].porosity_0;
			if (patch[0].rz_storage/(min(patch[0].sat_deficit, patch[0].rootzone.potential_sat)) 
			 < wilting_point) m_LWP  = 0.0;
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
	
	return(stratum_conductance);
} /*end compute_vascular_stratum_conductance*/
