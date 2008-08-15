
#include <assert.h>
#include "ProcessorSimple.h"

log4cxx::LoggerPtr ProcessorSimple::logger(log4cxx::Logger::getLogger("robot.ProcessorSimple"));

ProcessorSimple::ProcessorSimple(ResourceQueue *srcQueue, ResourceQueue *dstQueue) {
	this->srcQueue = srcQueue;
	this->dstQueue = dstQueue;
}

ProcessorSimple::~ProcessorSimple() {
}

void ProcessorSimple::runThread() {
	// get one item from srcQueue, process it and put it into dstQueue
	while (1)  {
		Resource *resource = srcQueue->getResource(true);
		vector<ModuleSimple*>::iterator iter;
		for (iter = modules.begin(); iter != modules.end(); iter++) {
			(*iter)->Process(resource);
		}
		dstQueue->putResource(resource, true);
	}
}

bool ProcessorSimple::Init(Config *config, const char *name) {
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

	// construct all modules
	int modules = config->getSize(name, "module");
	for (int i = 0; i < modules; i++) {
		const char *m = config->getValue(name, "module", i);
		assert(m != NULL);
		const char *mtype = config->getType(m);
		if (mtype == NULL) {
			LOG_CONFIG_ERROR1(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Unknown type: %s", mtype);
			return false;
		}
		if (strcmp(mtype, "ModuleSimple")) {
			LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Invalid module (must be ModuleSimple)");
			return false;
		}
		const char *lib = config->getValue(m, "lib");
		ModuleSimple *(*create)() = (ModuleSimple*(*)())loadLibrary(lib, "create");
		if (create == NULL) {
			LOG_CONFIG_ERROR0(logger, config->getXMLline(m, "lib"), config->getXMLcolumn(m, "lib"), "Cannot use module");
			return false;
		}
		ModuleSimple *module = (*create)();
		if (!module->Init(config, m))
			return false;
		this->modules.push_back(module);
	}

	return true;
}
