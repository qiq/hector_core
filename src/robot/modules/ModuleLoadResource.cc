/**
 * Dummy module.
 */

#include "ModuleLoadResource.h"

bool ModuleLoadResource::Init(Config *config, const char *name) {
	return true;
}

void ModuleLoadResource::Process(Resource *resource) {
	return;
}

// the class factories

extern "C" ModuleInput* create() {
	return (ModuleInput*)new ModuleLoadResource();
}

extern "C" void destroy(ModuleInput* p) {
	delete p;
}

