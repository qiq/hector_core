// wrapper around a list of SyncQueue<Resource>
// used for processor output

#ifndef _FILTERQUEUE_H_
#define _FILTERQUEUE_H_

#include "ObjectRegistry.h"
#include "Queue.h"
#include "SyncQueue.h"
#include "PrioritySyncQueue.h"

class OutputQueue {
	int filter;
	SyncQueue<Resource> *queue;
public:
	OutputQueue(Queue *queue, int filter): filter(filter), queue(queue) {};
	~OutputQueue() {};

	int getFilter();
	Queue *getQueue();
};

inline int OutputQueue::getFilter() {
	return filter;
}

inline Queue *OutputQueue::getQueue() {
	return queue;
}

class FilterQueue {
	ObjectRegistry *objects;

	vector<OutputQueue*> outputQueues;

	static log4cxx::LoggerPtr logger;
public:
	FilterQueue(ObjectRegistry *objects);
	~FilterQueue();

	bool Init(Config *config, const char *id);
	void Start();
	void Stop();
	Resource *getResource(bool wait);
	int getResources(Resource **r, int size, bool wait);
};
#endif
