/**
 * Basic simple module, process one Resource at a time.
 */

#ifndef _MODULE_OUTPUT_H_
#define _MODULE_OUTPUT_H_

#include "Resource.h"
#include "Module.h"

class ModuleOutput : public Module {
	Resource *resource;
public:
	ModuleOutput() {};
	virtual ~ModuleOutput();
	virtual void Process(Resource *resource);
};

#endif
