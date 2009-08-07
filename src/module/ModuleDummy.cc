/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleDummy.h"
#include "Server.h"
#include "WebResource.h"

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("module.DummyModule"));

bool ModuleDummy::Init(Server *server, Config *config, const char *name) {
	this->server = server;

	return true;
}

void ModuleDummy::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Dummy: processing resource " << wr->getURL());
	}
	return;
}

void ModuleDummy::createCheckpoint() {
	// TODO
}

// factory functions

extern "C" Module* create() {
	return new ModuleDummy();
}

extern "C" void destroy(Module* p) {
	delete p;
}
