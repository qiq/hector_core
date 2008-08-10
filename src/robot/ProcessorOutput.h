/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_OUTPUT_H_
#define _PROCESSOR_OUTPUT_H_

#include <log4cxx/logger.h>
#include "Processor.h"
#include "ModuleOutput.h"

class ProcessorOutput : public Processor {
	ResourceQueue *srcQueue;
	vector<ModuleOutput*> modules;

	static log4cxx::LoggerPtr logger;
public:
	ProcessorOutput(ResourceQueue *srcQueue);
	~ProcessorOutput();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
