/**
 * Processor contains one or more modules, that actually process documents.
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
#include "OutputFilter.h"
#include "Resource.h"
#include "SyncQueue.h"

class Processor : public Object {
	int nThreads;
	pthread_t *threads;

	vector<Module*> modules; 		// all modules
	SyncQueue<Resource> *queue;		// input queue
	vector<OutputFilter*> outputFilters;	// filters of output resources

	Lock runningLock;
	bool running;

	std::tr1::unordered_map<string, char*(Processor::*)(const char*)> getters;
	std::tr1::unordered_map<string, void(Processor::*)(const char*, const char*)> setters;

	char *getQueueItems(const char *name);

	static log4cxx::LoggerPtr logger;
public:
	Processor(ObjectRegistry *objects, const char *id);
	~Processor();
	bool init(Config *config);
	bool connect(); // connect processors to other processors
	bool Running();
	bool appendResource(Resource *r, bool sleep); // process resource and append it to other queues
	void runThread();
	void start();
	void stop();
	void pause();
	void resume();
	void createCheckpoint();

	SyncQueue<Resource> *getQueue();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline SyncQueue<Resource> *Processor::getQueue() {
	return queue;
}

#endif
