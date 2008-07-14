/*
 *
 */

#ifndef _ROBOT_HTTP_SERVER_
#define _ROBOT_HTTP_SERVER_

#include <pthread.h>
#include "SimpleHTTPServer.h"
#include "SimpleHTTPHandler.h"
#include "SimpleHTTPConn.h"

class RobotHTTPServer : SimpleHTTPHandler {
	SimpleHTTPServer *server;
	pthread_t thread;
public:
	RobotHTTPServer();
	~RobotHTTPServer();
	void runThread();
	void Run();
	void ProcessGET(SimpleHTTPConn *conn);
	void ProcessPOST(SimpleHTTPConn *conn);
};

#endif
