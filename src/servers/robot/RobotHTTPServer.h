/*
 *
 */

#ifndef _ROBOT_HTTP_SERVER_
#define _ROBOT_HTTP_SERVER_

#include <pthread.h>
#include "ObjectRegistry.h"
#include "server/SimpleHTTPServer.h"
#include "server/SimpleHTTPConn.h"

class RobotHTTPServer : public SimpleHTTPServer {
private:
	ObjectRegistry *objects;
public:
	RobotHTTPServer(ObjectRegistry *objects);
	~RobotHTTPServer();
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
