/**
 * Basic module, loads objects from one queue and puts them in another queue.
 */

#ifndef _PROCESSOR_PARALLEL_H_
#define _PROCESSOR_PARALLEL_H_

#include <log4cxx/logger.h>
#include "SyncQueue.h"
#include "Resource.h"
#include "Processor.h"
#include "ModuleParallel.h"

class ProcessorParallel : public Processor {
	SyncQueue<Resource> *srcQueue;
	SyncQueue<Resource> *dstQueue;
	ModuleParallel *module;
	int requests;
	int maxRequests;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorParallel(SyncQueue<Resource> *srcQueue, SyncQueue<Resource> *dstQueue);
	virtual ~ProcessorParallel();
	void runThread();
	bool Init(Config *config, const char *name);
	virtual void createCheckpoint();
};

#endif
