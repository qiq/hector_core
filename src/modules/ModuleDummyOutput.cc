/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleDummyOutput.h"
#include "WebResource.h"

bool ModuleDummyOutput::Init(Config *config) {
	return true;
}

Resource *ModuleDummyOutput::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Saving resource (URL: " << wr->getURL() << ")");
		delete wr;
	}
	return NULL;
}

void ModuleDummyOutput::SaveCheckpoint(const char *path, const char *id) {
	// TODO
}

void ModuleDummyOutput::RestoreCheckpoint(const char *path, const char *id) {
	// TODO
}

char *ModuleDummyOutput::getValue(const char *name) {
	return NULL;
}

bool ModuleDummyOutput::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *ModuleDummyOutput::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new ModuleDummyOutput(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

