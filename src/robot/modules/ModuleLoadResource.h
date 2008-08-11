/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_LOAD_RESOURCE_H_
#define _MODULE_LOAD_RESOURCE_H_

#include "ModuleInput.h"

class ModuleLoadResource : public ModuleInput {
public:
	ModuleLoadResource() {};
	virtual ~ModuleLoadResource() {};
	bool Init(Config *config, const char *name);
	void Process(Resource *resource);
};

#endif
