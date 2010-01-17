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

Resource *DummyInput::Process(Resource *resource) {
	assert(resource == NULL);
	WebResource *wr = new WebResource();
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	wr->setURL(s);
	LOG4CXX_INFO(logger, "Loading resource (URL: " << wr->getURL() << ")");
	return wr;
}

void DummyInput::SaveCheckpoint(const char *path, const char *id) {
	// TODO
}

void DummyInput::RestoreCheckpoint(const char *path, const char *id) {
	// TODO
}

char *DummyInput::getValue(const char *name) {
	return NULL;
}

bool DummyInput::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *DummyInput::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new DummyInput(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

