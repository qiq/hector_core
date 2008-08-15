/**
 * Dummy module.
 */

#include "ModuleSaveResource.h"

bool ModuleSaveResource::Init(Config *config, const char *name) {
	return true;
}

void ModuleSaveResource::Process(Resource *resource) {
	fprintf(stderr, "Saving resource (URL: %s)\n", resource->getURL());
	return;
}

// the class factories

extern "C" ModuleOutput* create() {
	return (ModuleOutput*)new ModuleSaveResource();
}

extern "C" void destroy(ModuleOutput* p) {
	delete p;
}

