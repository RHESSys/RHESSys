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

void hydro_routing( struct command_line_object * command_line,
                    double                       extstep,   /*  external time step  */
                    struct	date	             current_date,
                    struct basin_object        * basin );

#endif
