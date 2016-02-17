
/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_litter_soil_mortality			*/
/*                                                              */
/*  NAME                                                        */
/*		update_litter_soil_mortality			*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_mortality( 
/*                      struct epconst_struct,			*/
/*                      struct phenology_struct *,		*/
/*                      struct cstate_struct *,			*/
/*                      struct cdayflux_struct *,		*/
/*                      struct cdayflux_patch_struct *,		*/
/*                      struct nstate_struct *,			*/
/*                      struct ndayflux_struct *,		*/
/*                      struct ndayflux_patch_struct *,		*/
/*                      struct litter_c_object *,		*/
/*                      struct litter_n_object *,		*/
/*                      double);				*/
/*								*/
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*	calculated daily mortality losses and updates 		*/
/*	carbon and nitrogen pools				*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	from P.Thornton (1997) version of 1d_bgc		*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
void update_litter_soil_mortality(
					  struct litter_object *litter,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct soil_c_object *cs_soil,
					  struct soil_n_object *ns_soil,
					  struct fire_litter_soil_loss_struct fire_loss,
					  struct cstate_struct *cs,
					  struct cdayflux_patch_struct *cdf_patch,
					  struct nstate_struct *ns,
					  struct ndayflux_patch_struct *ndf_patch)
{

	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/


	double m_litr1c_to_atmos;
	double m_litr2c_to_atmos;
	double m_litr3c_to_atmos;
	double m_litr4c_to_atmos;
	double m_soil1c_to_atmos;
	double m_soil2c_to_atmos;
	double m_soil3c_to_atmos;
	double m_soil4c_to_atmos;
	double m_cwdc_to_atmos;


	/* litter and soil carbon fluxes due to fire loss */

	m_litr1c_to_atmos = fire_loss.loss_litr1c * cs_litr->litr1c;
	m_litr2c_to_atmos = fire_loss.loss_litr2c * cs_litr->litr2c;
	m_litr3c_to_atmos = fire_loss.loss_litr3c * cs_litr->litr3c;
	m_litr4c_to_atmos = fire_loss.loss_litr4c * cs_litr->litr4c;

	m_soil1c_to_atmos = fire_loss.loss_soil1c * cs_soil->soil1c;
	m_soil2c_to_atmos = fire_loss.loss_soil2c * cs_soil->soil2c;
	m_soil3c_to_atmos = fire_loss.loss_soil3c * cs_soil->soil3c;
	m_soil4c_to_atmos = fire_loss.loss_soil4c * cs_soil->soil4c;

	m_cwdc_to_atmos = fire_loss.loss_cwdc * cs->cwdc;



	/* litter and soil nitrogen fluxes due to fire loss */





	/* update state variables */

	cs_litr->litr1c -= m_litr1c_to_atmos;
	cs_litr->litr2c -= m_litr2c_to_atmos;
	cs_litr->litr3c -= m_litr3c_to_atmos;
	cs_litr->litr4c -= m_litr4c_to_atmos;

	cs_soil->soil1c -= m_soil1c_to_atmos;
	cs_soil->soil2c -= m_soil2c_to_atmos;
	cs_soil->soil3c -= m_soil3c_to_atmos;
	cs_soil->soil4c -= m_soil4c_to_atmos;

	cs->cwdc -= m_cwdc_to_atmos;


	
	return;
}/*end update_litter_soil_mortality*/

