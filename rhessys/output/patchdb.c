
#include <stdio.h>
#include <stdlib.h>

#include <cassandra.h>

void patchdb_print_error(CassFuture* future) {
	const char* message;
	size_t message_length;
	cass_future_error_message(future, &message, &message_length);
	fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassError patchdb_execute_query(CassSession* session, const char* query) {
	CassError rc = CASS_OK;
	CassFuture* future = NULL;
	CassStatement* statement = cass_statement_new(query, 0);

	future = cass_session_execute(session, statement);
	cass_future_wait(future);

	rc = cass_future_error_code(future);
	if (rc != CASS_OK) {
		patchdb_print_error(future);
		exit(EXIT_FAILURE);
	}

	cass_future_free(future);
	cass_statement_free(statement);

	return rc;
}

void init_patchdb(char* hostname,
				  char* keyspace_name,
				  CassCluster* cluster,
				  CassSession* session) {

	printf("Connecting to Cassandra cluster at %s ...\n", hostname);

	cluster = cass_cluster_new();
	session = cass_session_new();
	cass_cluster_set_contact_points(cluster, hostname);
	CassFuture* connect_future = cass_session_connect(session, cluster);

	if (cass_future_error_code(connect_future) != CASS_OK) {
		patchdb_print_error(connect_future);
		exit(EXIT_FAILURE);
	}

	printf("Creating keyspace %s ...", keyspace_name);

	patchdb_execute_query(session,
	              	  	  sprintf("CREATE KEYSPACE %s WITH replication = { \
	               'class': 'SimpleStrategy', 'replication_factor': '1' };", keyspace_name));

	printf("done\n");

	cass_future_free(connect_future);
}


void destroy_patchdb(CassCluster* cluster,
					 CassSession* session) {
	CassFuture* close_future = cass_session_close(session);
	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(cluster);
	cass_session_free(session);
}
