// wrapper around a list of SyncQueue<Resource>
// used for processor output

#ifndef _FILTERQUEUE_H_
#define _FILTERQUEUE_H_

#include "Lock.h"
#include "ObjectRegistry.h"
#include "Queue.h"
#include "SyncQueue.h"
#include "PrioritySyncQueue.h"

class OutputQueue {
	Queue *queue;
	SyncQueue<Resource> *syncQueue;
	int filter;
	Lock *lock;
public:
	OutputQueue(Queue *queue, int filter);
	~OutputQueue() {};
	bool operator<(OutputQueue other);

	int getFilter();
	Queue *getQueue();
	SyncQueue<Resource> *getSyncQueue();
	Lock *getLock();
};

inline OutputQueue::OutputQueue(Queue *queue, int filter): queue(queue), filter(filter) {
	syncQueue = queue->getSyncQueue();
	lock = syncQueue->getLock();
}

inline int OutputQueue::getFilter() {
	return filter;
}

inline Queue *OutputQueue::getQueue() {
	return queue;
}

inline SyncQueue<Resource> *OutputQueue::getSyncQueue() {
	return syncQueue;
}

inline Lock *OutputQueue::getLock() {
	return lock;
}

inline bool OutputQueue::operator<(OutputQueue other) {
	return queue->getQueueId() < other.getQueue()->getQueueId();
}

class FilterQueue {
	ObjectRegistry *objects;

	vector<OutputQueue*> *filterOutputQueue;
	SyncQueue<Resource> *simpleOutputQueue;
	int simpleFilter;

	static log4cxx::LoggerPtr logger;
public:
	FilterQueue(ObjectRegistry *objects);
	~FilterQueue();

	bool init(Config *config, const char *id);
	void start();
	void stop();
	int getQueueCount();

	bool putResource(Resource *r, bool sleep);
	int putResources(Resource **r, int size, bool sleep);
};

#endif
