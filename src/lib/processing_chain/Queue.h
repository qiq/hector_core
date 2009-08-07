/**
 * Processing: call modules
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "Config.h"
#include "Object.h"
#include "Resource.h"
#include "SyncQueue.h"

class Server;

class Queue : public Object {
private:
	SyncQueue<Resource> *queue;

	Server *server;

	static log4cxx::LoggerPtr logger;
public:
	Queue();
	~Queue();
	bool Init(Server *server, Config *config, const char *id);
	void Start();
	void Stop();
	void createCheckpoint();

	bool putResource(Resource *resource);
	Resource *getResource();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

#endif
