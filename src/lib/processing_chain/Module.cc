//
#include <config.h>

#include "common.h"
#include "Module.h"

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("lib.processing_chain.Module"));

Resource *Module::Process(Resource *resource) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (single)");
	return NULL;
}

int Module::Process(Resource **in, Resource **out) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (multi)");
	return 0;
}

char *Module::getValue(const char *name) {
	return NULL;
}

bool Module::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *Module::listNames() {
	return new vector<string>();
}
