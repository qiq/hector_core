/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_SIMPLE_H_
#define _PROCESSOR_SIMPLE_H_

#include <vector>
#include "Processor.h"
#include "ModuleSimple.h"

class ProcessorSimple : public Processor {
	ResourceQueue *srcQueue;
	ResourceQueue *dstQueue;
	Resource *resource;
	vector<ModuleSimple*> modules;
public:
	ProcessorSimple(ResourceQueue *srcQueue, ResourceQueue *dstQueue);
	virtual ~ProcessorSimple();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
