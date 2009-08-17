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
#include "FilterQueue.h"
#include "PriorityQueue.h"

class Processor : public Object {
	int nThreads;
	pthread_t *threads;

	vector<Module*> modules;
	PriorityQueue *inputQueue;
	FilterQueue *outputQueue;

	Lock runningLock;
	bool running;

	static log4cxx::LoggerPtr logger;
public:
	Processor(ObjectRegistry *objects, const char *id);
	~Processor();
	bool Running();
	void runThread();
	bool init(Config *config);
	void start();
	void stop();
	void pause();
	void resume();
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
