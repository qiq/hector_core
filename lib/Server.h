/**
 * Main server object
 * - store all properties
 * - store all sub-objects (processing engines, etc)
 * - load server library
 */
#ifndef _SERVER_H_
#define _SERVER_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "common.h"
#include "Config.h"
#include "Object.h"
#include "ProcessingEngine.h"
#include "SimpleHTTPServer.h"

class Server : public Object {
public:
	Server(const char *id);
	~Server();
	bool Init(Config *config);
	void Start(bool wait);
	void Stop();
	void Pause();
	void Resume();

protected:
	char *serverHost;
	int serverPort;
	int threads;
	std::vector<ProcessingEngine*> processingEngines;
	SimpleHTTPServer *simpleHTTPServer;

	bool waitForFinish;
	bool isRunning;

	static log4cxx::LoggerPtr logger;
};

#endif
