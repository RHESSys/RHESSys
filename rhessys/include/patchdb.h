/*
 * patchdb.h
 *
 *  Created on: Jun 18, 2015
 *      Author: miles
 */

#ifndef UTIL_PATCHDBMQ_INCLUDE_PATCHDB_H_
#define UTIL_PATCHDBMQ_INCLUDE_PATCHDB_H_

#include <cassandra.h>

#define MAXSTR	1024

#define PATCHDB_SOCKET_PATH "inproc://patchdb"

void patchdb_print_error(CassFuture* future, const char* query);
CassError patchdb_prepare_statement(CassSession* session, const char* query,
		const CassPrepared **stmt);
CassError patchdb_execute_query(CassSession* session, const char* query);

void init_patchdb(char* hostname,
		 	 	  char* keyspace_name,
				  CassCluster** cluster,
				  CassSession** session);

void destroy_patchdb(CassCluster* cluster,
					 CassSession* session);


typedef struct patchdb_args {
	void *context;
	char *cass_hostname;
	char *cass_keyspace;
	const char *socket_path;
} PatchDBArgs;

void *patchdbserver(void *args);

#endif /* UTIL_PATCHDBMQ_INCLUDE_PATCHDB_H_ */
