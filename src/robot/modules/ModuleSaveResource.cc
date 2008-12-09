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

void ModuleSaveResource::createCheckpoint() {
}

string *ModuleSaveResource::getStatistics() {
	return new string();
}

// the class factories

extern "C" ModuleOutput* create() {
	return (ModuleOutput*)new ModuleSaveResource();
}

extern "C" void destroy(ModuleOutput* p) {
	delete p;
}

