/*
 * zmq.h
 *
 *  Created on: Jul 5, 2015
 *      Author: miles
 */

#ifndef RHESSYS_INCLUDE_ZMQUTIL_H_
#define RHESSYS_INCLUDE_ZMQUTIL_H_

#include <google/protobuf/message.h>
#include <zmq.h>

using namespace std;

int PbToZmq(::google::protobuf::Message *src, zmq_msg_t *dest);

#endif /* RHESSYS_INCLUDE_ZMQUTIL_H_ */
