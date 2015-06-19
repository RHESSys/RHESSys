#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <zmq.h>
#include <cassandra.h>

#include "patch.pb.h"
#include "patchdb.h"


using namespace std;

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
static CassBatch* patchdb_batch;

void _cass_init(char *cass_hostname, char *cass_keyspace) {
	init_patchdb(cass_hostname, cass_keyspace,
				 &cass_cluster, &cass_session);
	// Make tables
	char query[MAXSTR];
	printf("patchdb: Creating tables ... ");
	// Table partitioned by variable and clustered by date then patch ID
	snprintf(query, MAXSTR, "CREATE TABLE IF NOT EXISTS variables_by_date_patch ("
							"variable text,"
							"date timestamp,"
							"patchid text,"
							"value double,"
							"PRIMARY KEY (variable, date, patchid));");
	patchdb_execute_query(cass_session, query);

	// Table partitioned by patch ID and clustered by variable then date
	snprintf(query, MAXSTR, "CREATE TABLE IF NOT EXISTS patches_by_variable_date ("
							"patchid text,"
							"variable text,"
							"date timestamp,"
							"value double,"
							"PRIMARY KEY (patchid, variable, date));");
	patchdb_execute_query(cass_session, query);
}

void _cass_prep_stmt(int year, int month, int day) {
	/*
	 * Make prepared statements for this day
	 */
	char datestr[16];
	snprintf(datestr, 16, "%d-%02d-%02d",
			year, month, day);
	// Make prepared statements for this day
	CassError rc = CASS_OK;
	char query[128];
	snprintf(query, 128, "INSERT INTO variables_by_date_patch "
			"(variable,date,patchid,value) "
			"VALUES (?,'%s',?,?);", datestr);
	//printf(query);
	rc = patchdb_prepare_statement(cass_session,
			(const char*)&query, &var_by_date_patch_stmt);
	if (rc != CASS_OK) {
		exit(EXIT_FAILURE);
	}

	snprintf(query, 128, "INSERT INTO patches_by_variable_date "
			"(patchid,variable,date,value) "
			"VALUES (?,?,'%s',?);", datestr);
	//printf(query);
	rc = patchdb_prepare_statement(cass_session,
			(const char*)&query, &patch_by_var_date_stmt);
	if (rc != CASS_OK) {
		exit(EXIT_FAILURE);
	}

	// Make batch for batch execution
	patchdb_batch = cass_batch_new(CASS_BATCH_TYPE_UNLOGGED);
}

void _bind_to_stmts(const char* patchid, const char* var, cass_double_t value) {
	//CassError rc = CASS_OK;
	CassStatement* statement = NULL;

	statement = cass_prepared_bind(var_by_date_patch_stmt);
	cass_statement_bind_string(statement, 0, var);
	cass_statement_bind_string(statement, 1, patchid);
	cass_statement_bind_double(statement, 2, value);
	cass_batch_add_statement(patchdb_batch, statement);
	cass_statement_free(statement);

	statement = cass_prepared_bind(patch_by_var_date_stmt);
	cass_statement_bind_string(statement, 0, patchid);
	cass_statement_bind_string(statement, 1, var);
	cass_statement_bind_double(statement, 2, value);
	cass_batch_add_statement(patchdb_batch, statement);
	cass_statement_free(statement);
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

    int year = -1;
    int month = -1;
    int day = -1;
    bool data_to_write = false;

    char patchid[64];

    while (1) {
    	zmq_msg_t msg;
    	int rc = zmq_msg_init(&msg);
    	assert(rc == 0);
    	/* Block until a message is available to be received from socket */
    	rc = zmq_recvmsg(responder, &msg, 0);
    	assert(rc != -1);
    	// TODO: Recv as multipart message with control information preceding
    	// data.
    	// See: http://zguide.zeromq.org/page:all#Multipart-Messages

    	// De-serialize message Protocol Buffers message
    	rhessys::OutputPatch p;
    	_ZmqToPb(&msg, &p);
    	/* Release message */
    	zmq_msg_close(&msg);

    	snprintf(patchid, 64, "%d:%d:%d:%d", p.basin_id(),
    			p.hill_id(), p.zone_id(), p.patch_id());
    	fprintf(debug, "%s\n", patchid);

    	// Prepare statements (if needed)
    	if (p.year() != year || p.month() != month || p.day() != day) {
    		printf("Need to make prepared statements...\n");
    		year = p.year();
    		month = p.month();
    		day = p.day();
    		if (data_to_write) {
    			// Write data via batch insert
    			printf("Writing data to Cassandra...\n");
				CassError cass_rc = CASS_OK;
				CassFuture* future = NULL;
				future = cass_session_execute_batch(cass_session,
							patchdb_batch);
				cass_future_wait(future);

				cass_rc = cass_future_error_code(future);
				if (rc != CASS_OK) {
					patchdb_print_error(future, "Batch execution of output_patch");
					exit(EXIT_FAILURE);
				}

				cass_future_free(future);
				// Free batch
				cass_batch_free(patchdb_batch);

				// Free prepared statements
				cass_prepared_free(var_by_date_patch_stmt);
				cass_prepared_free(patch_by_var_date_stmt);
    		}
    		_cass_prep_stmt(year, month, day);
    		data_to_write = false;
    	}

    	// Bind data to prepared statements
    	data_to_write = true;
    	_bind_to_stmts(patchid, "rain_thr", (cass_double_t)p.rain_throughfall());

        //sleep (1);          //  Do some 'work'
        zmq_send (responder, "A", 1, 0);
    }

    return EXIT_SUCCESS;
}

void quit(int sig) {
	// TODO: Make sure outstanding data are written to the DB before quitting
	fprintf(debug, "Received SIGTERM\n");
	fclose(debug);

	// Shutdown Cassandra connection
	destroy_patchdb(cass_cluster, cass_session);

    exit(sig);
}
