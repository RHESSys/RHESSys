
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <queue>
#include <errno.h>
#include <pthread.h>

#include <zmq.h>
#include <cassandra.h>

#include "rhessys.h"
#include "patch.pb.h"
#include "patchdb.h"

#define NUM_CONCURRENT_REQUESTS 60000

using namespace std;


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

	//printf("patchdb_prepare_statement...");
	future = cass_session_prepare(session, query);
	cass_future_wait(future);
	rc = cass_future_error_code(future);
	if (rc != CASS_OK) {
		patchdb_print_error(future, query);
	} else {
		*stmt = cass_future_get_prepared(future);
	}
	cass_future_free(future);
	//printf("done\n");

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
		pthread_exit(NULL);
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

	cass_cluster_set_write_bytes_high_water_mark(*cluster, 128 * 1024 * 1024);
	cass_cluster_set_write_bytes_low_water_mark(*cluster, 64 * 1024 * 1024);
	cass_cluster_set_num_threads_io(*cluster, 4);
	cass_cluster_set_queue_size_io(*cluster, 65536);

//	cass_cluster_set_pending_requests_high_water_mark(*cluster, 1024);
//	cass_cluster_set_pending_requests_low_water_mark(*cluster, 512);
//	cass_cluster_set_core_connections_per_host(*cluster, 2);
//	cass_cluster_set_max_connections_per_host(*cluster, 8);

	CassFuture* connect_future = cass_session_connect(*session, *cluster);

	if (cass_future_error_code(connect_future) != CASS_OK) {
		patchdb_print_error(connect_future, NULL);
		pthread_exit(NULL);
	}

	printf("patchdb: Creating keyspace %s ... ", keyspace_name);

	char query[MAXSTR];
	// TODO: remove "IF NOT EXISTS" for production use
	snprintf(query, MAXSTR, "CREATE KEYSPACE IF NOT EXISTS %s WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': '3' };", keyspace_name);
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

int _ZmqToPb(zmq_msg_t *src, ::google::protobuf::Message *dest) {
	// Adapted from: http://stackoverflow.com/questions/16732774/whats-elegant-way-to-send-binary-data-serizlized-with-googles-protocol-buffers
    int rc = 0;
    try {
        rc = dest->ParseFromArray(zmq_msg_data(src), zmq_msg_size(src))?0:-1;
    }
    catch (google::protobuf::FatalException fe) {
        std::cout << "ZmqToPb " << fe.message() << std::endl;
    }
    return rc;
}

void _cass_init(char *cass_hostname, char *cass_keyspace,
				CassCluster* cass_cluster, CassSession* cass_session) {
	// Make tables
	char query[MAXSTR];
	printf("patchdb: Creating tables ... ");
	// Table partitioned by variable and clustered by date then patch ID
	snprintf(query, MAXSTR, "CREATE TABLE IF NOT EXISTS variables_by_date_patch ("
							"variable text,"
							"date text,"
							"patchid text,"
							"value double,"
							"PRIMARY KEY (variable, date, patchid));");
	patchdb_execute_query(cass_session, query);

	// Table partitioned by patch ID and clustered by variable then date
	snprintf(query, MAXSTR, "CREATE TABLE IF NOT EXISTS patches_by_variable_date ("
							"patchid text,"
							"variable text,"
							"date text,"
							"value double,"
							"PRIMARY KEY (patchid, variable, date));");
	patchdb_execute_query(cass_session, query);
}

void _cass_destroy(const CassPrepared *var_by_date_patch_stmt,
				   const CassPrepared *patch_by_var_date_stmt) {
	// Free prepared statements
	cass_prepared_free(var_by_date_patch_stmt);
	cass_prepared_free(patch_by_var_date_stmt);
}

void _cass_prep_stmt(CassSession* cass_session,
					 const CassPrepared **var_by_date_patch_stmt,
		             const CassPrepared **patch_by_var_date_stmt) {
	/*
	 * Make prepared statements
	 */
	CassError rc = CASS_OK;
	char *query = (char *)malloc(128);
	snprintf(query, 128, "INSERT INTO variables_by_date_patch "
			"(variable,date,patchid,value) "
			"VALUES (?,?,?,?);");
	rc = patchdb_prepare_statement(cass_session,
			(const char*)query, var_by_date_patch_stmt);
	if (rc != CASS_OK) {
		pthread_exit(NULL);
	}

	snprintf(query, 128, "INSERT INTO patches_by_variable_date "
			"(patchid,variable,date,value) "
			"VALUES (?,?,?,?);");
	rc = patchdb_prepare_statement(cass_session,
			(const char*)query, patch_by_var_date_stmt);
	if (rc != CASS_OK) {
		pthread_exit(NULL);
	}
	free(query);
}

void make_outstanding_writes(std::queue<CassFuture *>& insert_future_queue) {
	CassError rc = CASS_OK;
	CassFuture* future = NULL;

	size_t outstanding_inserts = insert_future_queue.size();
	if (outstanding_inserts >= NUM_CONCURRENT_REQUESTS) {
		for (int i = outstanding_inserts; i > 0; i--) {
			future = insert_future_queue.front();
			insert_future_queue.pop();
			cass_future_wait(future);
			rc = cass_future_error_code(future);
			if (rc != CASS_OK) {
				patchdb_print_error(future, "Error writing to patchdb");
			}
			cass_future_free(future);
		}
		future = NULL;
	}
}

void _bind_to_stmts_and_write(std::queue<CassFuture *>& insert_future_queue,
		CassSession* cass_session,
		const CassPrepared *var_by_date_patch_stmt,
		const CassPrepared *patch_by_var_date_stmt,
		const char* patchid,
		const char* date, const char* var, cass_double_t value) {
	CassError rc = CASS_OK;
	CassStatement* statement = NULL;
	CassFuture* future = NULL;

	make_outstanding_writes(insert_future_queue);

	statement = cass_prepared_bind(var_by_date_patch_stmt);
	cass_statement_bind_string(statement, 0, var);
	cass_statement_bind_string(statement, 1, date);
	cass_statement_bind_string(statement, 2, patchid);
	cass_statement_bind_double(statement, 3, value);
	cass_statement_set_consistency(statement, CASS_CONSISTENCY_ONE);
	future = cass_session_execute(cass_session, statement);
	insert_future_queue.push(future);

	cass_statement_free(statement);

	statement = cass_prepared_bind(patch_by_var_date_stmt);
	cass_statement_bind_string(statement, 0, patchid);
	cass_statement_bind_string(statement, 1, var);
	cass_statement_bind_string(statement, 2, date);
	cass_statement_bind_double(statement, 3, value);
	cass_statement_set_consistency(statement, CASS_CONSISTENCY_ONE);
	future = cass_session_execute(cass_session, statement);
	insert_future_queue.push(future);

	cass_statement_free(statement);

}

void *patchdbserver(void *args) {

	PatchDBArgs *my_args = (PatchDBArgs *)args;
	void *context = my_args->context;
	char *cass_hostname = my_args->cass_hostname;
	char *cass_keyspace = my_args->cass_keyspace;
	const char *socket_path = my_args->socket_path;

	CassCluster* cass_cluster;
	CassSession* cass_session;
	const CassPrepared *var_by_date_patch_stmt;
	const CassPrepared *patch_by_var_date_stmt;

	std::queue<CassFuture *> insert_future_queue;

	// Setup connection to Cassandra cluster
	init_patchdb(cass_hostname, cass_keyspace,
			 	 &cass_cluster, &cass_session);
	_cass_init(cass_hostname, cass_keyspace, cass_cluster, cass_session);

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	printf("patchdbmq: Connecting Cassandra cluster: %s...\n",
			cass_hostname);
	printf("patchdbmq: Using keyspace: %s\n",
			cass_keyspace);

    //  Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_PAIR);
    int rc = zmq_bind(responder, socket_path);
    assert (rc == 0);

#ifdef DEBUG
    FILE *debug = fopen("/tmp/patchdb.debug", "w");
#endif

    char patchid[64];
    char date[16];

    int year = -1;
    int month = -1;
    int day = -1;

    // Create prepared statements
    _cass_prep_stmt(cass_session,
    		&var_by_date_patch_stmt, &patch_by_var_date_stmt);

    bool loop = true;
    while (loop) {
    	zmq_msg_t msg;
    	int rc = zmq_msg_init(&msg);
    	assert(rc == 0);
    	/* Block until a message is available to be received from socket */
    	rc = zmq_msg_recv(&msg, responder, 0);
    	if (rc == -1) {
    		switch(errno) {
    			case EAGAIN:
    				printf("Non-blocking mode was requested and no messages are available at the moment.");
    				break;
    			case ENOTSUP:
    				printf("The zmq_recv() operation is not supported by this socket type.");
    				break;
    			case EFSM:
    				printf("The zmq_recv() operation cannot be performed on this socket at the moment due to the socket not being in the appropriate state.");
    				break;
    			case ETERM:
    				printf("The 0MQ context associated with the specified socket was terminated.");
    				break;
    			case ENOTSOCK:
    				printf("The provided socket was invalid.");
    				break;
    			case EINTR:
    				printf("The operation was interrupted by delivery of a signal before a message was available.");
    				break;
    			case EFAULT:
    				printf("The message passed to the function was invalid.");
    				break;
    			default:
    				printf("errno was %d, exiting ...", errno);
    				break;
    		}
    		pthread_exit(NULL);
    	}

    	// De-serialize message Protocol Buffers message
    	rhessys::PatchDBMesg m;
    	_ZmqToPb(&msg, &m);
    	/* Release message */
    	zmq_msg_close(&msg);

    	//printf("Message type: %d\n", m.type());
    	switch (m.type()) {
    	case m.BEGIN_SIM:
			break;
    	case m.OUTPUT_PATCH:
		{
			rhessys::OutputPatch p = m.outputpatch();

			// Refresh date
			if (p.day() != day || p.month() != month || p.year() != year) {
				year = p.year();
				month = p.month();
				day = p.day();

				snprintf(date, 16, "%d-%02d-%02d",
						 year, month, day);
#ifdef DEBUG
				fprintf(debug, "Date changed to: %s\n", date);
				fflush(debug);
#endif
			}

			snprintf(patchid, 64, "%d:%d:%d:%d", p.basin_id(),
					 p.hill_id(), p.zone_id(), p.patch_id());

			zmq_send(responder, "A", 1, 0);

			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "rain_thr", (cass_double_t)p.rain_throughfall());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "detention_store", (cass_double_t)p.detention_store());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "sat_def_z", (cass_double_t)p.sat_deficit_z());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "sat_def", (cass_double_t)p.sat_deficit());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "rz_storage", (cass_double_t)p.rz_storage());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "potential_sat", (cass_double_t)p.potential_sat());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "rz_field_capacity", (cass_double_t)p.field_capacity());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "rz_wilting_point", (cass_double_t)p.wilting_point());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "unsat_stor", (cass_double_t)p.unsat_storage());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "rz_drainage", (cass_double_t)p.rz_drainage());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "unsat_drain", (cass_double_t)p.unsat_drainage());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "sublimation", (cass_double_t)p.sublimation());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "return", (cass_double_t)p.return_flow());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "evap", (cass_double_t)p.evaporation());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "evap_surface", (cass_double_t)p.evaporation_surf());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "soil_evap", (cass_double_t)p.soil_evap());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "snow", (cass_double_t)p.snow());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "snow_melt", (cass_double_t)p.snow_melt());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "trans_sat", (cass_double_t)p.trans_sat());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "trans_unsat", (cass_double_t)p.trans_unsat());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "Qin", (cass_double_t)p.q_in());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "Qout", (cass_double_t)p.q_out());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "psn", (cass_double_t)p.psn());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "root_zone.S", (cass_double_t)p.rootzone_s());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "root.depth", (cass_double_t)p.rootzone_depth());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "litter.rain_stor", (cass_double_t)p.litter_rain_stored());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "litter.S", (cass_double_t)p.litter_s());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "area", (cass_double_t)p.area());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "pet", (cass_double_t)p.pet());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "lai", (cass_double_t)p.lai());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "baseflow", (cass_double_t)p.baseflow());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "streamflow", (cass_double_t)p.streamflow());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "pcp", (cass_double_t)p.precip());
			_bind_to_stmts_and_write(insert_future_queue, cass_session, var_by_date_patch_stmt, patch_by_var_date_stmt, patchid, date, "recharge", (cass_double_t)p.recharge());

			break;
		}
    	case m.END_SIM:
			printf("patchdbmq: Recv. EndSim, committing outstanding writes\n");
    		make_outstanding_writes(insert_future_queue);

#ifdef DEBUG
			fprintf(debug, "Received EndSim message\n");
			fclose(debug);
#endif

			// Shutdown Cassandra connection
			_cass_destroy(var_by_date_patch_stmt, patch_by_var_date_stmt);
			destroy_patchdb(cass_cluster, cass_session);
			zmq_send(responder, "A", 1, 0);
			zmq_close(responder);
			loop = false;
			break;
    	default:
    		break;
    	}
    }
    printf("Patchdb thread returning");
    return NULL;
}

