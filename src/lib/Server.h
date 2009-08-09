/**
 * Main server object
 * - store all properties
 * - store all sub-objects (processing chains, etc)
 * - load server library
 */
#ifndef _SERVER_H_
#define _SERVER_H_

#include <config.h>

#include <ext/hash_map>
#include <log4cxx/logger.h>
#include "common.h"
#include "Config.h"
#include "Lock.h"
#include "Object.h"
#include "processing_chain/ProcessingChain.h"
#include "server/SimpleHTTPServer.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class Server : public Object {
	char *serverHost;
	int serverPort;
	int threads;
	vector<ProcessingChain*> processingChains;
	SimpleHTTPServer *simpleHTTPServer;

	Lock propertiesLock;
	bool waitForFinish;
	bool isRunning;

	static log4cxx::LoggerPtr logger;
public:
	Server(const char *id);
	~Server();
	bool Init(Config *config);
	void Start(bool wait);
	void Stop();
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
