/*
 *
 */

#ifndef _LIB_PROCESSING_ENGINE_BASE_SERVER_
#define _LIB_PROCESSING_ENGINE_BASE_SERVER_

#include "config.h"

#include <vector>
#include <tr1/unordered_map>
#include "ObjectRegistry.h"
#include "SimpleHTTPServer.h"
#include "SimpleHTTPConn.h"

class Config;
class ProcessingEngine;

class BaseServer : public SimpleHTTPServer {
public:
	BaseServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines);
	virtual ~BaseServer() {};
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	bool HandleRequest(SimpleHTTPConn *conn);
	virtual bool HandleExtension(SimpleHTTPConn *conn);

protected:
	ObjectRegistry *objects;
	vector<ProcessingEngine*> *engines;
	std::tr1::unordered_map<string, ProcessingEngine*> name2engine;

	static log4cxx::LoggerPtr logger;
};

#endif
