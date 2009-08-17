//
#include <config.h>

#include "common.h"
#include "Module.h"

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("lib.processing_chain.Module"));

Resource *Module::process() {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (source)");
	return NULL;
}

void Module::process(Resource *resource) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (single)");
}

int Module::process(Resource **in, Resource **out) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (multi)");
	return 0;
}
