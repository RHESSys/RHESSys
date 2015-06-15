#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <zmq.h>

#include "patch.pb.h"

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

int main (int argc, char **argv) {
	(void)signal(SIGTERM, quit);

	if (argc != 4) {
		printf("patchdbmq: incorrect number of command line arguments\n");
	}
	char *cass_hostname = argv[1];
	char *cass_keyspace = argv[2];
	char *socket_path = argv[3];

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

    while (1) {
    	zmq_msg_t msg;
    	int rc = zmq_msg_init(&msg);
    	assert(rc == 0);
    	/* Block until a message is available to be received from socket */
    	rc = zmq_recvmsg(responder, &msg, 0);
    	assert(rc != -1);

    	// De-serialize message Protocol Buffers message
    	rhessys::OutputPatch p;
    	_ZmqToPb(&msg, &p);
    	fprintf(debug, "%d:%d:%d:%d\n", p.basin_id(),
    			p.hill_id(), p.zone_id(), p.patch_id());

    	/* Release message */
    	zmq_msg_close (&msg);

        //sleep (1);          //  Do some 'work'
        zmq_send (responder, "A", 1, 0);
    }

    return EXIT_SUCCESS;
}

void quit(int sig) {
	fprintf(debug, "Received SIGTERM\n");
	fclose(debug);
    exit(sig);
}
