/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_INPUT_H_
#define _PROCESSOR_INPUT_H_

#include "Processor.h"
#include "ModuleInput.h"

class ProcessorInput : public Processor {
	ResourceQueue *dstQueue;
	ModuleInput *module;
public:
	ProcessorInput(ResourceQueue *dstQueue);
	~ProcessorInput();
	void runThread();
	bool Init(Config *config, const char *name);
};

#endif
