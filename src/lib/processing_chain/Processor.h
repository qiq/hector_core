/**
 * Processor contains one or more modules, that actually process documents.
 */

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <config.h>

#include <pthread.h>
#include <string>
#include <vector>
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

	vector<Module*> *modules; 		// all modules
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
	bool Init(Config *config);
	bool Connect(); // connect processors to other processors
	bool Running();
	bool appendResource(Resource *r, bool sleep); // process resource and append it to other resources' queues
	void runThread(int id);
	void Start();
	void Stop();
	void Pause();
	void Resume();

	SyncQueue<Resource> *getQueue();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline SyncQueue<Resource> *Processor::getQueue() {
	return queue;
}

#endif
