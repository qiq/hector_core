/**
 * Main server object
 * - store all properties
 * - store all sub-objects (processing chains, etc)
 * - load server library
 * - register all objects, so that we can call getValue() and setValue() on them
 */
#ifndef _SERVER_H_
#define _SERVER_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "common.h"
#include "Config.h"
//#include "processing_chain/ProcessingChain.h"
#include "server/SimpleHTTPServer.h"

class Server {
	char *serverHost;
	int serverPort;
	int threads;
//	vector<ProcessingChain*> processingChains;
	SimpleHTTPServer *simpleHTTPServer;

	static log4cxx::LoggerPtr logger;
public:
	Server();
	~Server();
	bool Init(Config *config, const char *id);
	void Start(bool wait);
	void Stop();
	void createCheckpoint();
	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

#endif
