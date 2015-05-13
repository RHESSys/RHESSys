
#include <stdio.h>
#include <stdlib.h>

#include <cassandra.h>

#include "rhessys.h"

void patchdb_print_error(CassFuture* future, const char* query) {
	const char* message;
	size_t message_length;
	cass_future_error_message(future, &message, &message_length);
	fprintf(stderr, "\n\nError: %s\n\n", message);
	if (query != NULL) {
		fprintf(stderr, "Query was: %s", query);
	}
}

CassError patchdb_prepare_statement(CassSession* session, const char* query,
		const CassPrepared **stmt) {
	CassFuture* future = NULL;
	CassError rc = CASS_OK;

	future = cass_session_prepare(session, query);
	cass_future_wait(future);
	rc = cass_future_error_code(future);
	if (rc != CASS_OK) {
		patchdb_print_error(future, query);
	} else {
		*stmt = cass_future_get_prepared(future);
	}
	cass_future_free(future);
	return rc;
}

CassError patchdb_execute_query(CassSession* session, const char* query) {
	CassError rc = CASS_OK;
	CassFuture* future = NULL;
	CassStatement* statement = cass_statement_new(query, 0);

	future = cass_session_execute(session, statement);
	cass_future_wait(future);

	rc = cass_future_error_code(future);
	if (rc != CASS_OK) {
		patchdb_print_error(future, query);
		exit(EXIT_FAILURE);
	}

	cass_future_free(future);
	cass_statement_free(statement);

	return rc;
}

void init_patchdb(char* hostname,
				  char* keyspace_name,
				  CassCluster** cluster,
				  CassSession** session) {

	printf("patchdb: Connecting to Cassandra cluster at %s ...\n", hostname);

	*cluster = cass_cluster_new();
	*session = cass_session_new();
	cass_cluster_set_contact_points(*cluster, hostname);
	CassFuture* connect_future = cass_session_connect(*session, *cluster);

	if (cass_future_error_code(connect_future) != CASS_OK) {
		patchdb_print_error(connect_future, NULL);
		exit(EXIT_FAILURE);
	}

	printf("patchdb: Creating keyspace %s ... ", keyspace_name);

	char query[MAXSTR];
	// TODO: remove "IF NOT EXISTS" for production use
	snprintf(query, MAXSTR, "CREATE KEYSPACE IF NOT EXISTS %s WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': '1' };", keyspace_name);
	patchdb_execute_query(*session, query);

	// Use the newly created keyspace
	snprintf(query, MAXSTR, "USE %s;", keyspace_name);
	patchdb_execute_query(*session, query);

	printf("done\n");

	cass_future_free(connect_future);
}

void destroy_patchdb(CassCluster* cluster,
					 CassSession* session) {
	CassFuture* close_future = cass_session_close(session);

	printf("patchdb: Disconnecting from cluster ... ");

	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(cluster);
	cass_session_free(session);

	printf("done\n");
}
