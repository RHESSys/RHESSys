#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

//#include <cassandra.h>

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


//void patchdb_print_error(CassFuture* future, const char* query);
//CassError patchdb_prepare_statement(CassSession* session, const char* query,
//		const CassPrepared **stmt);
//CassError patchdb_execute_query(CassSession* session, const char* query);
//
//void init_patchdb(char* hostname,
//		 	 	  char* keyspace_name,
//				  CassCluster** cluster,
//				  CassSession** session);
//
//void destroy_patchdb(CassCluster* cluster,
//					 CassSession* session);

#endif
