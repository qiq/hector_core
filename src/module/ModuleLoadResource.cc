/**
 * Dummy module.
 */
#include <config.h>

#include <stdlib.h>
#include "ModuleLoadResource.h"
#include "Server.h"
#include "WebResource.h"

bool ModuleLoadResource::Init(Server *server, Config *config, const char *id) {
	this->server = server;

	return true;
}

Resource *ModuleLoadResource::Process() {
	WebResource *wr = new WebResource();
	char s[1024];
	snprintf(s, sizeof(s), "http://example.org/?id=%ld", random());
	wr->setURL(s);
	fprintf(stderr, "Loading resource (URL: %s)\n", wr->getURL());
	return wr;
}

void ModuleLoadResource::createCheckpoint() {
}

// the class factories

extern "C" Module* create() {
	return (Module*)new ModuleLoadResource();
}

extern "C" void destroy(Module* p) {
	delete p;
}

