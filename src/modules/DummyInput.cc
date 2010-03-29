/**
 * Dummy module.
 */
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "DummyInput.h"
#include "WebResource.h"

bool DummyInput::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *DummyInput::ProcessSimple(Resource *resource) {
	assert(resource == NULL);
	WebResource *wr = new WebResource();
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	wr->setURL(s);
	LOG_INFO(logger, "Loading resource (URL: " << wr->getURL() << ")");
	return wr;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new DummyInput(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}

