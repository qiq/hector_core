/**
 * Main server object
 * - store all properties
 * - store all sub-objects (processing engines, etc)
 * - load server library
 */
#ifndef _LIB_SERVER_H_
#define _LIB_SERVER_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "common.h"
#include "Config.h"
#include "PlainLock.h"
#include "Object.h"
#include "ProcessingEngine.h"
#include "SimpleHTTPServer.h"

class Server : public Object {
public:
	Server(const char *id, bool batch);
	~Server();
	bool Init(Config *config);
	void Start(bool wait);
	void Stop();
	void Pause();
	void Resume();

	// called by processing engine when all threads are sleeping (or not)
	void ProcessingEngineSleeping();
	void ProcessingEngineWakeup();

protected:
	char *serverHost;
	int serverPort;
	int threads;
	std::vector<ProcessingEngine*> processingEngines;
	SimpleHTTPServer *simpleHTTPServer;

	bool waitForFinish;
	bool isRunning;
	bool batch;	// batch mode
	int sleepingProcessingEngines;
	bool wokenUp;
	PlainLock usingWokenUp;
};

#endif
