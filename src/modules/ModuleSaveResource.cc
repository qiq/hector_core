/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleSaveResource.h"
#include "WebResource.h"

bool ModuleSaveResource::Init(Config *config) {
	return true;
}

void ModuleSaveResource::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Saving resource (URL: " << wr->getURL() << ")");
		delete wr;
	}
	return;
}

void ModuleSaveResource::createCheckpoint() {
}

char *ModuleSaveResource::getValue(const char *name) {
	return NULL;
}

bool ModuleSaveResource::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *ModuleSaveResource::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new ModuleSaveResource(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

