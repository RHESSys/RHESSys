#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <zmq.h>

FILE *debug;
void quit(int sig);

int main (int argc, char **argv) {
	(void)signal(SIGTERM, quit);

	if (argc != 4) {
		printf("patchdbmq: incorrect number of command line arguments\n");
	}
	char *cass_hostname = argv[1];
	char *cass_keyspace = argv[2];
	char *socket_path = argv[3];

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
        char buffer[65];
        zmq_recv(responder, buffer, 65, 0);
        buffer[64] = 0;
        fprintf(debug, buffer);
        fprintf(debug, "\n");
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
