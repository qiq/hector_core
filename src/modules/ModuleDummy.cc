/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleDummy.h"
#include "Server.h"
#include "WebResource.h"

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("module.DummyModule"));

bool ModuleDummy::Init(Config *config) {
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

const char *ModuleDummy::getValue(const char *name) {
	return NULL;
}

bool ModuleDummy::setValue(const char *name, const char *value) {
	return false;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return new ModuleDummy(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}
