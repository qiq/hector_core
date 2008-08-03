/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_H_
#define _MODULE_DUMMY_H_

#include "ModuleSimple.h"

class ModuleDummy : public ModuleSimple {
public:
	ModuleDummy() {};
	~ModuleDummy() {};
	void Process(Resource *resource);
};

#endif
