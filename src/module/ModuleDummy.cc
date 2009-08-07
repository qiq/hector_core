/**
 * Dummy module.
 */

#include "ModuleDummy.h"
#include "Server.h"
#include "WebResource.h"

bool ModuleDummy::Init(Server *server, Config *config, const char *name) {
	this->server = server;

	return true;
}

void ModuleDummy::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(Resource);
	if (wr) {
		LOG_DEBUG("Dummy: processing resource " << wr->getURL());
	}
	return;
}

void ModuleDummy::createCheckpoint() {
}

// factory functions

extern "C" ModuleSimple* create() {
	return new ModuleDummy();
}

extern "C" void destroy(ModuleSimple* p) {
	delete p;
}
