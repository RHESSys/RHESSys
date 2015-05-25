#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (int argc, char **argv) {

	if (argc != 3) {
		printf("patchdbmq: incorrect number of command line arguments\n");
	}
	char *cass_hostname = argv[1];
	char *cass_keyspace = argv[2];

	printf("patchdbmq: Connecting Cassandra cluster: %s...\n",
			cass_hostname);
	printf("patchdbmq: Using keyspace: %s\n",
			cass_keyspace);

    //  Socket to talk to clients
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "ipc:///tmp/patchdbmq");
    assert (rc == 0);

    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        printf ("Received Hello\n");
        sleep (1);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
