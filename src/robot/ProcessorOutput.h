/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_OUTPUT_H_
#define _PROCESSOR_OUTPUT_H_

#include "Processor.h"
#include "ModuleOutput.h"

class ProcessorOutput : public Processor {
	ResourceQueue *srcQueue;
	ModuleOutput *module;
public:
	ProcessorOutput(ResourceQueue *srcQueue);
	~ProcessorOutput();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
