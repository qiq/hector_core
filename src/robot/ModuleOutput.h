/**
 * Basic simple module, process one Resource at a time.
 */

#ifndef _MODULE_OUTPUT_H_
#define _MODULE_OUTPUT_H_

#include "Module.h"

class ModuleOutput : public Module {
public:
	ModuleOutput() {}
	virtual ~ModuleOutput() {};
	virtual void Process(Resource *resource) = 0;
};

//typedef ModuleOutput* create_t();
//typedef void destroy_t(ModuleOutput*);

#endif
