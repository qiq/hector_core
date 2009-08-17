/*
 *
 */

#ifndef _ROBOT_HTTP_SERVER_
#define _ROBOT_HTTP_SERVER_

#include <pthread.h>
#include <log4cxx/logger.h>
#include "ObjectRegistry.h"
#include "server/SimpleHTTPServer.h"
#include "server/SimpleHTTPConn.h"

class RobotHTTPServer : public SimpleHTTPServer {
	ObjectRegistry *objects;

	static log4cxx::LoggerPtr logger;
public:
	RobotHTTPServer(ObjectRegistry *objects);
	~RobotHTTPServer();
	bool handleRequest(SimpleHTTPConn *conn);
};

#endif
