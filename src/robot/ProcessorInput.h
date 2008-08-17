/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_INPUT_H_
#define _PROCESSOR_INPUT_H_

#include <log4cxx/logger.h>
#include "SyncQueue.h"
#include "Resource.h"
#include "Processor.h"
#include "ModuleInput.h"

class ProcessorInput : public Processor {
	SyncQueue<Resource> *dstQueue;
	ModuleInput *module;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorInput(SyncQueue<Resource> *dstQueue);
	~ProcessorInput();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
