/*
 *
 */

#ifndef _LIB_PROCESSING_ENGINE_SERVERS_TEST_SERVER_
#define _LIB_PROCESSING_ENGINE_SERVERS_TEST_SERVER_

#include "config.h"

#include <vector>
#include <tr1/unordered_map>
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
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	bool HandleRequest(SimpleHTTPConn *conn);
};

#endif
