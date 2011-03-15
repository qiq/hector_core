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
#include <tr1/unordered_set>
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
	// get processed resources from the processing engine, result: cancelled or timeout
	bool GetProcessedResources(std::tr1::unordered_set<int> *ids, vector<Resource*> *output, struct timeval *timeout);
	bool GetProcessedResourcesOrdered(std::vector<int> *ids, vector<Resource*> *output, struct timeval *timeout);
	// get processed resource from the processing engine, result: available/not available
	// TODO: remove
	//Resource *GetProcessedResource(int id, struct timeval *timeout);

	// helper methods for Processor::Connect()
	SyncQueue<Resource> *CreateOutputQueue();
	// used by Processor::Connect and by Processor when resource is
	// detected to be deleted. This method is thread-safe.
	SyncQueue<Resource> *GetOutputQueue();
	// used to detect, whether Processor's queue is PE's input queue. This
	// method is thread-safe.
	SyncQueue<Resource> *GetInputQueue();
	// Update number of resources processed in a PE. This method is
	// thread-safe.
	void UpdateResourceCount(int n);

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

	ObjectValues<ProcessingEngine> *values;

	bool propRun;		// ObjectLock
	bool propPause;		// ObjectLock
	int resourceCount;	// ObjectLock

	char *GetRun(const char *name);
	void SetRun(const char *name, const char *value);
	char *GetPause(const char *name);
	void SetPause(const char *name, const char *value);
	char *GetResourceCount(const char *name);

	void StartSync();
	void StopSync();
	void PauseSync();
	void ResumeSync();

	void doPause();
	void doResume();

	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();

	bool SaveCheckpointSync(const char *path);
	bool RestoreCheckpointSync(const char *path);
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

inline SyncQueue<Resource> *ProcessingEngine::CreateOutputQueue() {
	if (!outputQueue) {
		outputQueue = new SyncQueue<Resource>();
		outputQueue->addQueue(0, 0, 0);
	}
        return outputQueue;
}

inline SyncQueue<Resource> *ProcessingEngine::GetOutputQueue() {
	return outputQueue;
}

inline SyncQueue<Resource> *ProcessingEngine::GetInputQueue() {
	return inputQueue;
}

inline void ProcessingEngine::UpdateResourceCount(int n) {
	ObjectLockWrite();
	resourceCount += n;
	ObjectUnlock();
}

#endif
