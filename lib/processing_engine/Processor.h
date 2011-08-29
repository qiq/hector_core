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
#include "ObjectProperties.h"
#include "Resource.h"
#include "SyncQueue.h"

class Config;
class OutputFilter;
class ProcessingEngine;

class Processor : public Object {
public:
	Processor(ObjectRegistry *objects, const char *id, ProcessingEngine *engine, bool batch);
	~Processor();
	bool Init(Config *config, const char *serverId, const char *processingEngineId);
	bool IsRunning();
	void RunThread(int id);
	void ThreadFinished();
	void Start();
	void Stop();
	void Pause();
	void Resume();

	SyncQueue<Resource> *GetInputQueue();

	char *GetInputQueueItems(const char *name);

protected:
	typedef struct {
		Module *module;
		Module::Type type;
		int processingResources;	// numbers of processing resources
		std::queue<Resource*> *inputResources;
		std::queue<Resource*> *outputResources;
	} ModuleInfo;

	int nThreads;				// total threads (some may be already finished)
	pthread_t *threads;
	bool running;
	bool batch;				// batch mode: report sleeping Processors to PE
	bool sleeping;				// whether we reported sleeping to PE
	int runningThreads;			// number of threads currently running (and sleeping)
	int sleepingThreads;			// number of sleeping threads
	bool pauseInput;			// guarded by pauseInputCond
	CondLock pauseInputCond;		// here we wait if pauseInput == true

	ProcessingEngine *engine;		// parent, beware you call only thread-safe methods!
	std::vector<ModuleInfo*> *modules;	// all modules; every thread has a module instance (first dimension)
	SyncQueue<Resource> *inputQueue;	// input queue
	std::vector<OutputFilter*> outputFilters;	// filters of output resources
	std::queue<Resource*> deletedResources;	// deleted resources to be appended to the engine's output queue
	SyncQueue<Resource> *engineOutputQueue;	// used only for appending deleted resources to engine's output queue

	char *GetPauseInput(const char *name);
	void SetPauseInput(const char *name, const char *value);

	ObjectProperties<Processor> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	bool SaveCheckpointSync(const char *path);
	bool RestoreCheckpointSync(const char *path);

	// update sleeping thread count (and possibly notify PE)
	void UpdateSleeping(int count);

	// connect processors to other processors
	bool Connect();
	// process resource and append it to other precesses' queues
	bool QueueResource(Resource *r, struct timeval *timeout, int *filterIndex);
	// apply simple/input/output modules to a resource
	Resource *ApplyModules(vector<ModuleInfo*> *mis, Resource *resource, int index, bool sleep, bool *stop);
	// return index of the next multi-module
	int NextMultiModuleIndex(vector<ModuleInfo*> *mis, int index);
	// append resource either to multi-module input queue or processor's output queue
	bool AppendResource(vector<ModuleInfo*> *mis, Resource *resource, int multiIndex, struct timeval *timeout, int *outputFilterIndex);
};

inline SyncQueue<Resource> *Processor::GetInputQueue() {
	return inputQueue;
}

inline char *Processor::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool Processor::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *Processor::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
