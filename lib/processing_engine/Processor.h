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
#include "Object.h"
#include "ObjectRegistry.h"
#include "Module.h"
#include "ObjectValues.h"
#include "Resource.h"
#include "SyncQueue.h"

class Config;
class OutputFilter;
class ProcessingEngine;

class Processor : public Object {
public:
	Processor(ObjectRegistry *objects, ProcessingEngine *engine, const char *id);
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

	int nThreads;				// properties, guarded by object lock
	pthread_t *threads;
	bool running;
	bool pauseInput;			// guarded by ObjectLock and pauseInputCond
	CondLock pauseInputCond;		// here we wait if pauseInput == true

	ProcessingEngine *engine;		// parent, beware you call only thread-safe methods!
	std::vector<ModuleInfo*> *modules;	// all modules; every thread has a module instance (first dimension)
	SyncQueue<Resource> *inputQueue;	// input queue
	std::vector<OutputFilter*> outputFilters;	// filters of output resources
	std::queue<Resource*> deletedResources;	// deleted resources to be appended to the engine's output queue
	SyncQueue<Resource> *engineOutputQueue;	// used only for appending deleted resources to engine's output queue

	ObjectValues<Processor> *values;

	char *getPauseInput(const char *name);
	void setPauseInput(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();

	bool SaveCheckpointSync(const char *path);
	bool RestoreCheckpointSync(const char *path);

	// process resource and append it to other precesses' queues
	bool QueueResource(Resource *r, struct timeval *timeout, int *filterIndex);
	// apply simple/input/output modules to a resource
	Resource *ApplyModules(vector<ModuleInfo*> *mis, Resource *resource, int index, bool sleep, bool *stop);
	// return index of the next multi-module
	int NextMultiModuleIndex(vector<ModuleInfo*> *mis, int index);
	// append resource either to multi-module input queue or processor's output queue
	bool AppendResource(vector<ModuleInfo*> *mis, Resource *resource, int multiIndex, struct timeval *timeout, int *outputFilterIndex);
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

inline bool Processor::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *Processor::listNamesSync() {
	return values->listNamesSync();
}

#endif
