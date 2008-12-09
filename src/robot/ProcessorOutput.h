/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_OUTPUT_H_
#define _PROCESSOR_OUTPUT_H_

#include <log4cxx/logger.h>
#include "SyncQueue.h"
#include "Resource.h"
#include "Processor.h"
#include "ModuleOutput.h"

class ProcessorOutput : public Processor {
	SyncQueue<Resource> *srcQueue;
	vector<ModuleOutput*> modules;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorOutput(SyncQueue<Resource> *srcQueue);
	~ProcessorOutput();
	void runThread();
	bool Init(Config *config, const char *name);
	virtual void createCheckpoint();
};

#endif
