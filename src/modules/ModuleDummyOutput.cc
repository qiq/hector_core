/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleDummyOutput.h"
#include "WebResource.h"

bool ModuleDummyOutput::init(Config *config) {
	return true;
}

void ModuleDummyOutput::process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Saving resource (URL: " << wr->getURL() << ")");
		delete wr;
	}
	return;
}

void ModuleDummyOutput::createCheckpoint() {
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

