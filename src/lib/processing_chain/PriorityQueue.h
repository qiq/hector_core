// Wrapper class for PrioritySyncQueue<Resource> or simple SyncQueue<Resource>

#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

#include <config.h>

#include "ObjectRegistry.h"
#include "Queue.h"
#include "SyncQueue.h"
#include "PrioritySyncQueue.h"

class PriorityQueue {
	ObjectRegistry *objects;

	// either inputQueue (simple case) or priorityInputQueue (priorities case) is used
	Queue *simpleInputQueue;
	PrioritySyncQueue<Resource> *priorityInputQueue;
	int queueCount;

	static log4cxx::LoggerPtr logger;
public:
	PriorityQueue(ObjectRegistry *objects);
	~PriorityQueue();

	bool init(Config *config, const char *id);
	void start();
	void stop();
	void pause();
	void resume();
	int getQueueCount();

	Resource *getResource(bool sleep);
	int getResources(Resource **r, int size, bool sleep);
};

#endif
