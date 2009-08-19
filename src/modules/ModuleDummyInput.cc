/**
 * Dummy module.
 */
#include <config.h>

#include <stdlib.h>
#include "ModuleDummyInput.h"
#include "WebResource.h"

bool ModuleDummyInput::init(Config *config) {
	return true;
}

Resource *ModuleDummyInput::process() {
	WebResource *wr = new WebResource();
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	wr->setURL(s);
	LOG4CXX_INFO(logger, "Loading resource (URL: " << wr->getURL() << ")");
	return wr;
}

void ModuleDummyInput::createCheckpoint() {
}

char *ModuleDummyInput::getValue(const char *name) {
	return NULL;
}

bool ModuleDummyInput::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *ModuleDummyInput::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new ModuleDummyInput(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

