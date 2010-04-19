/*
 *
 */

#ifndef _TEST_SERVER_
#define _TEST_SERVER_

#include <pthread.h>
#include <log4cxx/logger.h>
#include "ObjectRegistry.h"
#include "server/SimpleHTTPServer.h"
#include "server/SimpleHTTPConn.h"

class TestServer : public SimpleHTTPServer {
	ObjectRegistry *objects;

	static log4cxx::LoggerPtr logger;
public:
	TestServer(ObjectRegistry *objects);
	~TestServer();
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
