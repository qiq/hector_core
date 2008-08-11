/**
 * Basic simple module, process one Resource at a time.
 */

#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"

class ModuleInput : public Module {
public:
	ModuleInput() {};
	virtual ~ModuleInput() {};
	virtual void Process(Resource *resource) = 0;
};

#endif
