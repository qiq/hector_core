/**
 * Dummy module.
 */

#include "ModuleDummy.h"

void ModuleDummy::Process(Resource *resource) {
	return;
}

// the class factories

extern "C" ModuleSimple* create() {
	return new ModuleDummy();
}

extern "C" void destroy(ModuleSimple* p) {
	delete p;
}

