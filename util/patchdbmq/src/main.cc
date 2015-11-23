#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <queue>

#include <zmq.h>
#include <cassandra.h>

#include "patch.pb.h"
#include "patchdb.h"

#define NUM_CONCURRENT_REQUESTS 10000

using namespace std;

//const size_t MAX_STATEMENTS = 64000;
//size_t num_statements = 0;

FILE *debug;
void quit(int sig);

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

static CassCluster* cass_cluster;
static CassSession* cass_session;
static const CassPrepared *var_by_date_patch_stmt;
static const CassPrepared *patch_by_var_date_stmt;
//static CassBatch* patchdb_batch;

static std::queue<CassFuture *> insert_future_queue;

void _cass_init(char *cass_hostname, char *cass_keyspace) {
	init_patchdb(cass_hostname, cass_keyspace,
				 &cass_cluster, &cass_session);
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

void _cass_destroy() {
	// Free prepared statements
	cass_prepared_free(var_by_date_patch_stmt);
	cass_prepared_free(patch_by_var_date_stmt);
}

void _cass_prep_stmt() {
	/*
	 * Make prepared statements for this day
	 */
//	char datestr[16];
//	snprintf(datestr, 16, "%d-%02d-%02d",
//			year, month, day);
	// Make prepared statements for this day
	CassError rc = CASS_OK;
	char query[128];
	snprintf(query, 128, "INSERT INTO variables_by_date_patch "
			"(variable,date,patchid,value) "
			"VALUES (?,?,?,?);");
	//printf(query);
	rc = patchdb_prepare_statement(cass_session,
			(const char*)&query, &var_by_date_patch_stmt);
	if (rc != CASS_OK) {
		exit(EXIT_FAILURE);
	}

	snprintf(query, 128, "INSERT INTO patches_by_variable_date "
			"(patchid,variable,date,value) "
			"VALUES (?,?,?,?);");
	//printf(query);
	rc = patchdb_prepare_statement(cass_session,
			(const char*)&query, &patch_by_var_date_stmt);
	if (rc != CASS_OK) {
		exit(EXIT_FAILURE);
	}

	// Make batch for batch execution
//	patchdb_batch = cass_batch_new(CASS_BATCH_TYPE_UNLOGGED);
}

//void _cass_write_data(void) {
//
//	if (num_statements > 0) {
//		printf("Writing data to Cassandra...\n");
//		CassError cass_rc = CASS_OK;
//		CassFuture* future = NULL;
//		future = cass_session_execute_batch(cass_session,
//					patchdb_batch);
//		cass_future_wait(future);
//
//		cass_rc = cass_future_error_code(future);
//		if (cass_rc != CASS_OK) {
//			patchdb_print_error(future, "Batch execution of output_patch");
//			exit(EXIT_FAILURE);
//		}
//
//		cass_future_free(future);
//		// Free batch
//		cass_batch_free(patchdb_batch);
//
//		// Free prepared statements
//		cass_prepared_free(var_by_date_patch_stmt);
//		cass_prepared_free(patch_by_var_date_stmt);
//	}
//
//	num_statements = 0;
//}

void _bind_to_stmts_and_write(const char* patchid,
		const char* date, const char* var, cass_double_t value) {
	CassError rc = CASS_OK;
	CassStatement* statement = NULL;
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
		}
		future = NULL;
	}

	statement = cass_prepared_bind(var_by_date_patch_stmt);
	cass_statement_bind_string(statement, 0, var);
	cass_statement_bind_string(statement, 1, date);
	cass_statement_bind_string(statement, 2, patchid);
	cass_statement_bind_double(statement, 3, value);
//	cass_batch_add_statement(patchdb_batch, statement);
	future = cass_session_execute(cass_session, statement);
	insert_future_queue.push(future);

//	cass_future_wait(future);
//	rc = cass_future_error_code(future);
//	if (rc != CASS_OK) {
//		patchdb_print_error(future, "Error writing to variables_by_date_patch");
//	}
//	cass_future_free(future);

	cass_statement_free(statement);

	statement = cass_prepared_bind(patch_by_var_date_stmt);
	cass_statement_bind_string(statement, 0, patchid);
	cass_statement_bind_string(statement, 1, var);
	cass_statement_bind_string(statement, 2, date);
	cass_statement_bind_double(statement, 3, value);
//	cass_batch_add_statement(patchdb_batch, statement);
	future = cass_session_execute(cass_session, statement);
	insert_future_queue.push(future);

//	cass_future_wait(future);
//	rc = cass_future_error_code(future);
//	if (rc != CASS_OK) {
//		patchdb_print_error(future,  "Error writing to patches_by_variable_date");
//	}
//	cass_future_free(future);

	cass_statement_free(statement);

//	num_statements += 2;
}

int main (int argc, char **argv) {
	(void)signal(SIGTERM, quit);

	if (argc != 4) {
		printf("patchdbmq: incorrect number of command line arguments\n");
	}
	char *cass_hostname = argv[1];
	char *cass_keyspace = argv[2];
	char *socket_path = argv[3];

	// Setup connection to Cassandra cluster
	_cass_init(cass_hostname, cass_keyspace);

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	printf("patchdbmq: Connecting Cassandra cluster: %s...\n",
			cass_hostname);
	printf("patchdbmq: Using keyspace: %s\n",
			cass_keyspace);

    //  Socket to talk to clients
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, socket_path);
    assert (rc == 0);

    debug = fopen("/tmp/patchdb.debug", "w");

    char patchid[64];
    char date[16];
//    cass_uint32_t date;

    int year = -1;
    int month = -1;
    int day = -1;

    // Create prepared statements
    _cass_prep_stmt();

    bool loop = true;
    while (loop) {
    	zmq_msg_t msg;
    	int rc = zmq_msg_init(&msg);
    	assert(rc == 0);
    	/* Block until a message is available to be received from socket */
    	rc = zmq_recvmsg(responder, &msg, 0);
    	assert(rc != -1);

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
//				//printf("Need to make prepared statements...\n");
				year = p.year();
				month = p.month();
				day = p.day();
//				_cass_write_data();
//				_cass_prep_stmt();

				snprintf(date, 16, "%d-%02d-%02d",
						 year, month, day);
				fprintf(debug, "Date changed to: %s\n", date);
				fflush(debug);

//				date = cass_date_from_epoch(datestr);
			}

			snprintf(patchid, 64, "%d:%d:%d:%d", p.basin_id(),
					 p.hill_id(), p.zone_id(), p.patch_id());
			fprintf(debug, "\tPatch ID: %s\n", patchid);

			// Bind data to prepared statements
			fprintf(debug, "\tPre cassandra send\n");

			_bind_to_stmts_and_write(patchid, date, "rain_thr", (cass_double_t)p.rain_throughfall());
			_bind_to_stmts_and_write(patchid, date, "detention_store", (cass_double_t)p.detention_store());
			_bind_to_stmts_and_write(patchid, date, "sat_def_z", (cass_double_t)p.sat_deficit_z());
			_bind_to_stmts_and_write(patchid, date, "sat_def", (cass_double_t)p.sat_deficit());
			_bind_to_stmts_and_write(patchid, date, "rz_storage", (cass_double_t)p.rz_storage());
			_bind_to_stmts_and_write(patchid, date, "potential_sat", (cass_double_t)p.potential_sat());
			_bind_to_stmts_and_write(patchid, date, "rz_field_capacity", (cass_double_t)p.field_capacity());
			_bind_to_stmts_and_write(patchid, date, "rz_wilting_point", (cass_double_t)p.wilting_point());
			_bind_to_stmts_and_write(patchid, date, "unsat_stor", (cass_double_t)p.unsat_storage());
			_bind_to_stmts_and_write(patchid, date, "rz_drainage", (cass_double_t)p.rz_drainage());
			_bind_to_stmts_and_write(patchid, date, "unsat_drain", (cass_double_t)p.unsat_drainage());
			_bind_to_stmts_and_write(patchid, date, "sublimation", (cass_double_t)p.sublimation());
			_bind_to_stmts_and_write(patchid, date, "return", (cass_double_t)p.return_flow());
			_bind_to_stmts_and_write(patchid, date, "evap", (cass_double_t)p.evaporation());
			_bind_to_stmts_and_write(patchid, date, "evap_surface", (cass_double_t)p.evaporation_surf());
			_bind_to_stmts_and_write(patchid, date, "soil_evap", (cass_double_t)p.soil_evap());
			_bind_to_stmts_and_write(patchid, date, "snow", (cass_double_t)p.snow());
			_bind_to_stmts_and_write(patchid, date, "snow_melt", (cass_double_t)p.snow_melt());
			_bind_to_stmts_and_write(patchid, date, "trans_sat", (cass_double_t)p.trans_sat());
			_bind_to_stmts_and_write(patchid, date, "trans_unsat", (cass_double_t)p.trans_unsat());
			_bind_to_stmts_and_write(patchid, date, "Qin", (cass_double_t)p.q_in());
			_bind_to_stmts_and_write(patchid, date, "Qout", (cass_double_t)p.q_out());
			_bind_to_stmts_and_write(patchid, date, "psn", (cass_double_t)p.psn());
			_bind_to_stmts_and_write(patchid, date, "root_zone.S", (cass_double_t)p.rootzone_s());
			_bind_to_stmts_and_write(patchid, date, "root.depth", (cass_double_t)p.rootzone_depth());
			_bind_to_stmts_and_write(patchid, date, "litter.rain_stor", (cass_double_t)p.litter_rain_stored());
			_bind_to_stmts_and_write(patchid, date, "litter.S", (cass_double_t)p.litter_s());
			_bind_to_stmts_and_write(patchid, date, "area", (cass_double_t)p.area());
			_bind_to_stmts_and_write(patchid, date, "pet", (cass_double_t)p.pet());
			_bind_to_stmts_and_write(patchid, date, "lai", (cass_double_t)p.lai());
			_bind_to_stmts_and_write(patchid, date, "baseflow", (cass_double_t)p.baseflow());
			_bind_to_stmts_and_write(patchid, date, "streamflow", (cass_double_t)p.streamflow());
			_bind_to_stmts_and_write(patchid, date, "pcp", (cass_double_t)p.precip());
			_bind_to_stmts_and_write(patchid, date, "recharge", (cass_double_t)p.recharge());

			fprintf(debug, "\tPost cassandra send\n");

			zmq_send (responder, "A", 1, 0);

			fprintf(debug, "\tPost zmq send\n");

			break;
		}
    	case m.END_SIM:
			printf("patchdbmq: Recv. EndSim\n");
			fprintf(debug, "Received EndSim message\n");
			fclose(debug);

			// Write any outstanding data
			//_cass_write_data();

			// Shutdown Cassandra connection
			_cass_destroy();
			destroy_patchdb(cass_cluster, cass_session);
			zmq_send (responder, "A", 1, 0);
			sleep(2); // Sleep to make sure client receives response before we close
			loop = false;
			break;
    	default:
    		break;
    	}
    }
    return EXIT_SUCCESS;
}

void quit(int sig) {
	// TODO: Make sure outstanding data are written to the DB before quitting
	fprintf(debug, "Received SIGTERM\n");
	fclose(debug);

	// Shutdown Cassandra connection
	_cass_destroy();
	destroy_patchdb(cass_cluster, cass_session);

    exit(sig);
}
