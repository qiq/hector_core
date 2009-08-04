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

#include <ext/hash_map>
#include <log4cxx/logger.h>
#include "common.h"
#include "Config.h"
#include "Object.h"
#include "processing_chain/ProcessingChain.h"
#include "server/SimpleHTTPServer.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class Server : Object {
	char *serverHost;
	int serverPort;
	int threads;
	vector<ProcessingChain*> processingChains;
	SimpleHTTPServer *simpleHTTPServer;

	stdext::hash_map<const char*, Object*> objects;

	static log4cxx::LoggerPtr logger;
public:
	Server();
	~Server();
	bool Init(Config *config, const char *id);
	void Start(bool wait);
	void Stop();
	void createCheckpoint();

	void registerObject(Object *obj);
	bool unregisterObject(const char *id);
	const char *getObjectValue(const char *id, const char *name);
	bool setObjectValue(const char *id, const char *name, const char *value);
};

#endif
