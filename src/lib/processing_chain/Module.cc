//
#include <config.h>

#include "common.h"
#include "Module.h"

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("lib.processing_chain.Module"));

void Module::Process(Resource *resource) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (single)");
}

int Module::Process(Resource **in, Resource **out) {
	LOG4CXX_ERROR(logger, "Unimplemented resource processing (multi)");
	return 0;
}
