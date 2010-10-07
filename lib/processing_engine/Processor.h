/**
 * Processor contains one or more modules, that actually process documents.
 */

#ifndef _LIB_PROCESSING_ENGINE_PROCESSOR_H_
#define _LIB_PROCESSING_ENGINE_PROCESSOR_H_

#include <config.h>

#include <pthread.h>
#include <queue>
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

	typedef struct {
		Module *module;
		Module::Type type;
		std::queue<Resource*> *inputResources;
		std::queue<Resource*> *outputResources;
	} ModuleInfo;

	int nThreads;				// properties, locked by object lock
	pthread_t *threads;
	bool running;

	std::vector<ModuleInfo*> *modules;	// all modules; every thread has a module instance (first dimension)
	SyncQueue<Resource> *inputQueue;	// input queue
	std::vector<OutputFilter*> outputFilters;	// filters of output resources

	ObjectValues<Processor> *values;

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

	// process resource and append it to other precesses' queues
	bool QueueResource(Resource *r, bool sleep, int *filterIndex);
	// apply simple/input/output modules to a resource
	Resource *ApplyModules(vector<ModuleInfo*> *mis, Resource *resource, int index, bool *stop);
	// return index of the next multi-module
	int NextMultiModuleIndex(vector<ModuleInfo*> *mis, int index);
	// append resource either to multi-module input queue or processor's output queue
	bool AppendResource(vector<ModuleInfo*> *mis, Resource *resource, int multiIndex, bool sleep, int *outputFilterIndex);
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
