/**
 * Processing engine: call several modules
 */

#ifndef _LIB_PROCESSING_ENGINE_PROCESSING_ENGINE_H_
#define _LIB_PROCESSING_ENGINE_PROCESSING_ENGINE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <tr1/unordered_map>
#include "common.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Processor.h"

class Config;

class ProcessingEngine : public Object {
public:
	ProcessingEngine(ObjectRegistry *objects, const char *id);
	~ProcessingEngine();
	bool Init(Config *config);
	void Start();
	void Stop();
	void Pause();
	void Resume();

	// process resource using the processing engine
	bool ProcessResource(Resource *resource, struct timeval *timeout);
	// get processed resource from the processing engine, result: available/not available
	Resource *GetProcessedResource(int id, struct timeval *timeout);

	// helper methods to create resources
	int ResourceNameToId(const char *name);
	Resource *CreateResource(int id);
	// This is called by Processor if resource is detected to be deleted.
	bool AppendToOutputQueue(Resource *resource);

	// helper method for Processor::Connect()
	SyncQueue<Resource> *getOutputQueue();

protected:
	std::vector<Processor*> processors;
	SyncQueue<Resource> *inputQueue;	// just a reference
	SyncQueue<Resource> *outputQueue;	// we own this queue
	CondLock finishedLock;
	bool waitingInQueue;
	std::tr1::unordered_map<int, Resource*> finishedResources;	// resources waiting for consumption
	PlainLock pauseLock;
	int waiting;
	bool cancel;

	bool propRun;
	bool propPause;

	ObjectValues<ProcessingEngine> *values;

	char *getRun(const char *name);
	void setRun(const char *name, const char *value);
	char *getPause(const char *name);
	void setPause(const char *name, const char *value);

	void StartSync();
	void StopSync();
	void PauseSync();
	void ResumeSync();

	void doPause();
	void doResume();

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();
};

inline void ProcessingEngine::Start() {
	ObjectLockWrite();
	StartSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Stop() {
	ObjectLockWrite();
	StopSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Pause() {
	ObjectLockWrite();
	PauseSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Resume() {
	ObjectLockWrite();
	ResumeSync();
	ObjectUnlock();
}

inline SyncQueue<Resource> *ProcessingEngine::getOutputQueue() {
	if (!outputQueue) {
		outputQueue = new SyncQueue<Resource>();
		outputQueue->addQueue(0, 0, 0);
	}

        return outputQueue;
}

#endif
