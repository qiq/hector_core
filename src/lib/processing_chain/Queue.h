/**
 * Processing: call modules
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Config.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Resource.h"
#include "SyncQueue.h"

class Queue : public Object {
	SyncQueue<Resource> *queue;

	// id of a queue: used to avoid deadlocks
	static Lock queueIdLock;
	static int nextQueueId;
	int queueId;

	static log4cxx::LoggerPtr logger;
public:
	Queue(ObjectRegistry *objects, const char *id);
	~Queue();
	bool init(Config *config);
	void start();
	void stop();
	void createCheckpoint();

	int getQueueId();

	bool putResource(Resource *resource, bool sleep);
	int putResources(Resource **r, int size, bool sleep);
	Resource *getResource(bool sleep);
	int getResources(Resource **r, int size, bool sleep);
	SyncQueue<Resource> *getSyncQueue();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline int Queue::getQueueId() {
	return queueId;
}

inline SyncQueue<Resource> *Queue::getSyncQueue() {
	return queue;
}

#endif
