#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "rhessys.h"

struct routing_list_object *construct_ddn_routing_topology(char *routing_filename,
		  struct basin_object *basin);

struct routing_list_object *construct_routing_topology(char *routing_filename,
		  struct basin_object *basin,
		  struct command_line_object *command_line,
		  bool surface);


double compute_prop_alloc_daily(
		double,
		struct cstate_struct *,
		struct epconst_struct);

void *alloc(size_t size, char *array_name, char *calling_function);

struct routing_list_object *construct_topmodel_patchlist(struct basin_object * const basin);

double	compute_potential_exfiltration(int 	verbose_flag,
									   double	S,
									   double 	sat_deficit_z,
									   double	Ksat_0,
									   double	m_z,
									   double	psi_air_entry,
									   double	pore_size_index,
									   double 	p,
									   double	p_0);

void	compute_Lstar(int	verbose_flag,
					  struct	basin_object	*basin,
					  struct	zone_object	*zone,
					  struct	patch_object	*patch);

void compute_Lstar_canopy(int	verbose_flag,
						  double KstarH,
						  double snow_stored,
						  struct	zone_object	*zone,
						  struct	patch_object	*patch,
						  struct	canopy_strata_object *stratum);

double compute_saturation_vapor_pressure(double temperature);

double compute_vapor_pressure_deficit(double saturation_vapor_pressure,
		double dewpoint_vapor_pressure);

#endif
