/**
 * Dummy module.
 */
#include <config.h>

#include "DummyOutput.h"
#include "WebResource.h"

bool DummyOutput::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *DummyOutput::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Saving resource (URL: " << wr->getURL() << ")");
		delete wr;
	}
	return NULL;
}

void DummyOutput::SaveCheckpoint(const char *path, const char *id) {
	// TODO
}

void DummyOutput::RestoreCheckpoint(const char *path, const char *id) {
	// TODO
}

char *DummyOutput::getValue(const char *name) {
	return NULL;
}

bool DummyOutput::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *DummyOutput::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new DummyOutput(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

