/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <config.h>

#include <pthread.h>
#include <string>
#include <log4cxx/logger.h>
#include "Config.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Lock.h"
#include "Module.h"
#include "Queue.h"

class InputQueue {
	int priority;
	Queue *queue;
public:
	InputQueue(Queue *queue, int priority): priority(priority),  queue(queue) {};
	~InputQueue() {};

	int getPriority();
	Queue *getQueue();
};

inline int InputQueue::getPriority() {
	return priority;
}

inline Queue *InputQueue::getQueue() {
	return queue;
}

class OutputQueue {
	int filter;
	Queue *queue;
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

class Processor : public Object {
	int nThreads;
	pthread_t *threads;

	vector<Module*> modules;
	vector<InputQueue*> inputQueues;
	vector<OutputQueue*> outputQueues;

	Lock runningLock;
	bool running;

	static log4cxx::LoggerPtr logger;
public:
	Processor(ObjectRegistry *objects, const char *id);
	~Processor();
	bool Running();
	void runThread();
	bool Init(Config *config);
	void Start();
	void Stop();
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
