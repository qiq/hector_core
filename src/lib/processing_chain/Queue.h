/**
 * Processing: call modules
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "Config.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Resource.h"
#include "SyncQueue.h"

class Queue : public Object {
private:
	SyncQueue<Resource> *queue;

	ObjectRegistry *objects;

	static log4cxx::LoggerPtr logger;
public:
	Queue(ObjectRegistry *objects, const char *id);
	~Queue();
	bool Init(Config *config);
	void Start();
	void Stop();
	void createCheckpoint();

	bool putResource(Resource *resource);
	Resource *getResource();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

#endif
