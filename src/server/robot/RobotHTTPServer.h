/*
 *
 */

#ifndef _ROBOT_HTTP_SERVER_
#define _ROBOT_HTTP_SERVER_

#include <pthread.h>
#include "Server.h"
#include "server/SimpleHTTPServer.h"
#include "server/SimpleHTTPConn.h"

class RobotHTTPServer : public SimpleHTTPServer {
private:
	Server *server;
public:
	RobotHTTPServer(Server *server);
	~RobotHTTPServer();
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
