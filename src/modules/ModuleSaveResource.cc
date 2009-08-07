/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleSaveResource.h"
#include "Server.h"
#include "WebResource.h"

bool ModuleSaveResource::Init(Server *server, Config *config, const char *id) {
	this->server = server;

	return true;
}

void ModuleSaveResource::Process(Resource *resource) {
	WebResource *wb = dynamic_cast<WebResouce*>(resource);
	if (wb) {
		fprintf(stderr, "Saving resource (URL: %s)\n", resource->getURL());
	}
	return;
}

void ModuleSaveResource::createCheckpoint() {
}

// the class factories

extern "C" Module* create() {
	return (Module*)new ModuleSaveResource();
}

extern "C" void destroy(Module* p) {
	delete p;
}

