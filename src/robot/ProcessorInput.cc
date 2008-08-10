
#include <assert.h>
#include "ProcessorInput.h"

log4cxx::LoggerPtr ProcessorInput::logger(log4cxx::Logger::getLogger("robot.ProcessorInput"));

ProcessorInput::ProcessorInput(ResourceQueue *dstQueue) {
	this->dstQueue = dstQueue;
}

ProcessorInput::~ProcessorInput() {
}

void ProcessorInput::runThread() {
	// get one item from somewhere, process it and put it into dstQueue
	while (running)  {
		Resource *resource = new Resource();
		module->Process(resource);
		dstQueue->putResource(resource, true);
	}
}

bool ProcessorInput::Init(Config *config, const char *name) {
	// threads
	nThreads = config->getValueInt(name, "threads");
	if (nThreads == INT_MAX) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "threads"), config->getXMLcolumn(name, "threads"), "Missing threads (int)");
		return false;
	}
	if (nThreads < 0 || nThreads > 100) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "threads"), config->getXMLcolumn(name, "threads"), "Invalid number of threads (int)");
		return false;
	}

	// construct module
	if (config->getSize(name, "module") != 1) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "no module/too many modules");
		return false;
	}
	const char *m = config->getValue(name, "module");
	assert(m != NULL);
	const char *mtype = config->getType(m);
	if (mtype == NULL) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Unknown type");
		return false;
	}
	if (strcmp(mtype, "ModuleInput")) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Invalid module (must be ModuleInput)");
		return false;
	}
	const char *lib = config->getValue(m, "lib");
	ModuleInput *(*create)() = (ModuleInput*(*)())loadLibrary(lib, "create");
	if (create == NULL) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(m, "lib"), config->getXMLcolumn(m, "lib"), "Cannot use module");
		return false;
	}
	module = (*create)();
	if (!module->Init(config, m))
		return false;

	return true;
}
