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
#include "ObjectValues.h"
#include "OutputFilter.h"
#include "Resource.h"
#include "SyncQueue.h"

class Processor : public Object {
public:
	Processor(ObjectRegistry *objects, const char *id);
	~Processor();
	bool Init(Config *config);
	bool Connect(); // connect processors to other processors
	bool isRunning();
	bool appendResource(Resource *r, bool sleep); // process resource and append it to other resources' queues
	void runThread(int id);
	void Start();
	void Stop();
	void Pause();
	void Resume();

	SyncQueue<Resource> *getQueue();

	char *getQueueItems(const char *name);

protected:
	int nThreads;				// properties, locked by object lock
	pthread_t *threads;
	bool running;

	vector<Module*> *modules; 		// all modules
	SyncQueue<Resource> *queue;		// input queue
	vector<OutputFilter*> outputFilters;	// filters of output resources

	ObjectValues<Processor> *values;

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

	static log4cxx::LoggerPtr logger;
};

inline SyncQueue<Resource> *Processor::getQueue() {
	return queue;
}

inline char *Processor::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool Processor::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *Processor::listNamesSync() {
	return values->listNamesSync();
}

#endif
