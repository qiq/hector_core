/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_SIMPLE_H_
#define _PROCESSOR_SIMPLE_H_

#include <log4cxx/logger.h>
#include <vector>
#include "SyncQueue.h"
#include "Resource.h"
#include "Processor.h"
#include "ModuleSimple.h"

class ProcessorSimple : public Processor {
	SyncQueue<Resource> *srcQueue;
	SyncQueue<Resource> *dstQueue;
	vector<ModuleSimple*> modules;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorSimple(SyncQueue<Resource> *srcQueue, SyncQueue<Resource> *dstQueue);
	virtual ~ProcessorSimple();
	void runThread();
	bool Init(Config *config, const char *name);
	virtual void createCheckpoint();
};

#endif
