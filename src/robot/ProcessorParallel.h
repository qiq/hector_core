/**
 * Basic module, loads objects from one queue and puts them in another queue.
 */

#ifndef _PROCESSOR_PARALLEL_H_
#define _PROCESSOR_PARALLEL_H_

#include "Processor.h"
#include "ModuleParallel.h"

class ProcessorParallel : public Processor {
	ResourceQueue *srcQueue;
	ResourceQueue *dstQueue;
	ModuleParallel *module;
	int requests;
	int maxRequests;
public:
	ProcessorParallel(ResourceQueue *srcQueue, ResourceQueue *dstQueue);
	virtual ~ProcessorParallel();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
