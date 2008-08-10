/**
 * Dummy module.
 */

#include "ModuleDummy.h"

bool ModuleDummy::Init(Config *config, const char *name) {
	return true;
}

void ModuleDummy::Process(Resource *resource) {
	return;
}

// factory functions

extern "C" ModuleSimple* create() {
	return new ModuleDummy();
}

extern "C" void destroy(ModuleSimple* p) {
	delete p;
}
