/**
 * Basic simple module, process one Resource at a time.
 */

#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Resource.h"
#include "Module.h"

class ModuleInput : public Module {
	Resource *resource;
public:
	ModuleInput() {};
	virtual ~ModuleInput();
	virtual void Process(Resource *resource);
};

#endif
