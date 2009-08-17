/**
 * Dummy module.
 */
#include <config.h>

#include <stdlib.h>
#include "ModuleLoadResource.h"
#include "WebResource.h"

bool ModuleLoadResource::Init(Config *config) {
	return true;
}

Resource *ModuleLoadResource::Process() {
	WebResource *wr = new WebResource();
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	wr->setURL(s);
	LOG4CXX_INFO(logger, "Loading resource (URL: " << wr->getURL() << ")");
	return wr;
}

void ModuleLoadResource::createCheckpoint() {
}

char *ModuleLoadResource::getValue(const char *name) {
	return NULL;
}

bool ModuleLoadResource::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *ModuleLoadResource::listNames() {
	return new vector<string>();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new ModuleLoadResource(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

