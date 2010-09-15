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
	void runThread(int id);
	void Start();
	void Stop();
	void Pause();
	void Resume();

	SyncQueue<Resource> *getInputQueue();

	char *getInputQueueItems(const char *name);

protected:
	int nThreads;				// properties, locked by object lock
	pthread_t *threads;
	bool running;

	std::vector<Module*> *modules; 		// all modules; every thread has a module instance
	std::vector<Module::Type> moduleType;	// type of processor modules (mainly due to expensive Perl calls)
	SyncQueue<Resource> *inputQueue;	// input queue
	std::vector<OutputFilter*> outputFilters;	// filters of output resources

	ObjectValues<Processor> *values;

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

	static log4cxx::LoggerPtr logger;

	bool appendResource(Resource *r, bool sleep, int *filterIndex); // process resource and append it to other resources' queues
};

inline SyncQueue<Resource> *Processor::getInputQueue() {
	return inputQueue;
}

inline char *Processor::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool Processor::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline std::vector<std::string> *Processor::listNamesSync() {
	return values->listNamesSync();
}

#endif
