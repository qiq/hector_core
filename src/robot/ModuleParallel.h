/**
 * Parallel module, it is able to process multiple instances of Resource
 */

#ifndef _MODULE_PARALLEL_H_
#define _MODULE_PARALLEL_H_

#include "Resource.h"
#include "Module.h"

class ModuleParallel : public Module {
public:
	ModuleParallel() {};
	virtual ~ModuleParallel();
	virtual int Process(Resource **in, Resource **out);
};

#endif
