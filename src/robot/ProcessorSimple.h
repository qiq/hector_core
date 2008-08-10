/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_SIMPLE_H_
#define _PROCESSOR_SIMPLE_H_

#include <log4cxx/logger.h>
#include <vector>
#include "Processor.h"
#include "ModuleSimple.h"

class ProcessorSimple : public Processor {
	ResourceQueue *srcQueue;
	ResourceQueue *dstQueue;
	vector<ModuleSimple*> modules;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorSimple(ResourceQueue *srcQueue, ResourceQueue *dstQueue);
	virtual ~ProcessorSimple();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
