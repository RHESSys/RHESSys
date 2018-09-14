
/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_litter_soil_mortality			*/
/*                                                              */
/*  NAME                                                        */
/*		update_litter_soil_mortality			*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*								*/
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*	calculates soil and litter losses following a fire	*/
/*								*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*								*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
void update_litter_soil_mortality(
					  struct cdayflux_patch_struct *cdf_patch,
					  struct ndayflux_patch_struct *ndf_patch,
					  struct soil_c_object *cs_soil,
					  struct soil_n_object *ns_soil,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct fire_litter_soil_loss_struct fire_loss)
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
	double m_litr1n_to_atmos;
	double m_litr2n_to_atmos;
	double m_litr3n_to_atmos;
	double m_litr4n_to_atmos;
	double m_soil1n_to_atmos;
	double m_soil2n_to_atmos;
	double m_soil3n_to_atmos;
	double m_soil4n_to_atmos;

	/* litter and soil carbon fluxes due to fire loss */

	m_litr1c_to_atmos = fire_loss.loss_litr1c * cs_litr->litr1c;
	m_litr2c_to_atmos = fire_loss.loss_litr2c * cs_litr->litr2c;
	m_litr3c_to_atmos = fire_loss.loss_litr3c * cs_litr->litr3c;
	m_litr4c_to_atmos = fire_loss.loss_litr4c * cs_litr->litr4c;
	m_soil1c_to_atmos = fire_loss.loss_soil1c * cs_soil->soil1c;
	m_soil2c_to_atmos = fire_loss.loss_soil2c * cs_soil->soil2c;
	m_soil3c_to_atmos = fire_loss.loss_soil3c * cs_soil->soil3c;
	m_soil4c_to_atmos = fire_loss.loss_soil4c * cs_soil->soil4c;

	/* litter and soil nitrogen fluxes due to fire loss */

	m_litr1n_to_atmos = fire_loss.loss_litr1n * ns_litr->litr1n;
	m_litr2n_to_atmos = fire_loss.loss_litr2n * ns_litr->litr2n;
	m_litr3n_to_atmos = fire_loss.loss_litr3n * ns_litr->litr3n;
	m_litr4n_to_atmos = fire_loss.loss_litr4n * ns_litr->litr4n;
	m_soil1n_to_atmos = fire_loss.loss_soil1n * ns_soil->soil1n;
	m_soil2n_to_atmos = fire_loss.loss_soil2n * ns_soil->soil2n;
	m_soil3n_to_atmos = fire_loss.loss_soil3n * ns_soil->soil3n;
	m_soil4n_to_atmos = fire_loss.loss_soil4n * ns_soil->soil4n;

	/* update state variables */

	cs_litr->litr1c -= m_litr1c_to_atmos;
	cs_litr->litr2c -= m_litr2c_to_atmos;
	cs_litr->litr3c -= m_litr3c_to_atmos;
	cs_litr->litr4c -= m_litr4c_to_atmos;
	cs_soil->soil1c -= m_soil1c_to_atmos;
	cs_soil->soil2c -= m_soil2c_to_atmos;
	cs_soil->soil3c -= m_soil3c_to_atmos;
	cs_soil->soil4c -= m_soil4c_to_atmos;

	ns_litr->litr1n -= m_litr1n_to_atmos;
	ns_litr->litr2n -= m_litr2n_to_atmos;
	ns_litr->litr3n -= m_litr3n_to_atmos;
	ns_litr->litr4n -= m_litr4n_to_atmos;
	ns_soil->soil1n -= m_soil1n_to_atmos;
	ns_soil->soil2n -= m_soil2n_to_atmos;
	ns_soil->soil3n -= m_soil3n_to_atmos;
	ns_soil->soil4n -= m_soil4n_to_atmos;
	
	return;
}/*end update_litter_soil_mortality*/

