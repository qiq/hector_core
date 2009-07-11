/*
 *
 */

#ifndef _ROBOT_HTTP_SERVER_
#define _ROBOT_HTTP_SERVER_

#include <pthread.h>
#include "server/SimpleHTTPServer.h"
#include "server/SimpleHTTPConn.h"

class RobotHTTPServer : public SimpleHTTPServer {
public:
	RobotHTTPServer(const char *addr, int port) : SimpleHTTPServer(addr, port) {};
	~RobotHTTPServer();
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
