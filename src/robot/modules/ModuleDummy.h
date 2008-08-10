/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_H_
#define _MODULE_DUMMY_H_

#include "ModuleSimple.h"

class ModuleDummy : public ModuleSimple {
public:
	ModuleDummy() {}
	virtual ~ModuleDummy() {}
	virtual bool Init(Config *config, const char *name);
	virtual void Process(Resource *resource);
};

#endif
