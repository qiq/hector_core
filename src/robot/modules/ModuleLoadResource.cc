/**
 * Dummy module.
 */

#include <stdlib.h>
#include "ModuleLoadResource.h"

bool ModuleLoadResource::Init(Config *config, const char *name) {
	return true;
}

void ModuleLoadResource::Process(Resource *resource) {
	
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	resource->setURL(s);
	fprintf(stderr, "Loading resource (URL: %s)\n", resource->getURL());
	return;
}

// the class factories

extern "C" ModuleInput* create() {
	return (ModuleInput*)new ModuleLoadResource();
}

extern "C" void destroy(ModuleInput* p) {
	delete p;
}

