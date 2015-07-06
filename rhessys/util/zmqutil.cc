#include <iostream>

#include <google/protobuf/message.h>
#include <zmq.h>

using namespace std;

int PbToZmq(::google::protobuf::Message *src, zmq_msg_t *dest) {
	// Adapted from: http://stackoverflow.com/questions/16732774/whats-elegant-way-to-send-binary-data-serizlized-with-googles-protocol-buffers
    int size = src->ByteSize();
    int rc = zmq_msg_init_size(dest, size);
    if (rc==0) {
        try {
            rc = src->SerializeToArray(zmq_msg_data(dest), size)?0:-1;
        }
        catch (google::protobuf::FatalException fe) {
            std::cout << "PbToZmq " << fe.message() << std::endl;
        }
    }
    return rc;
}
