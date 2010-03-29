/**
 * Dummy module.
 */
#include <config.h>

#include "DummyOutput.h"
#include "WebResource.h"

bool DummyOutput::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *DummyOutput::ProcessSimple(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG_INFO(logger, "Saving resource (URL: " << wr->getURL() << ")");
		delete wr;
	}
	return NULL;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new DummyOutput(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}

