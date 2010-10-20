/*
 *
 */

#ifndef _LIB_PROCESSING_ENGINE_SERVERS_TEST_SERVER_
#define _LIB_PROCESSING_ENGINE_SERVERS_TEST_SERVER_

#include <pthread.h>
#include <vector>
#include <log4cxx/logger.h>
#include "ObjectRegistry.h"
#include "SimpleHTTPServer.h"
#include "SimpleHTTPConn.h"
#include "TestResource.h"

class Config;
class ProcessingEngine;

class TestServer : public SimpleHTTPServer {
	ObjectRegistry *objects;
	vector<ProcessingEngine*> *engines;
	std::tr1::unordered_map<string, ProcessingEngine*> name2engine;
	int resourceId;

	static log4cxx::LoggerPtr logger;
public:
	TestServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines);
	~TestServer();
	bool Init(Config *config);
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
