/**
 * Basic simple module, process one Resource at a time.
 */

#ifndef _MODULE_SIMPLE_H_
#define _MODULE_SIMPLE_H_

#include "Resource.h"
#include "Module.h"

class ModuleSimple : public Module {
	Resource *resource;
public:
	ModuleSimple() {};
	virtual ~ModuleSimple() {};
	virtual void Process(Resource *resource) = 0;
};

#endif
