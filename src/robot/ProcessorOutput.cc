
#include <assert.h>
#include "ProcessorOutput.h"

log4cxx::LoggerPtr ProcessorOutput::logger(log4cxx::Logger::getLogger("robot.ProcessorOutput"));

ProcessorOutput::ProcessorOutput(ResourceQueue *srcQueue) {
	this->srcQueue = srcQueue;
}

ProcessorOutput::~ProcessorOutput() {
	vector<ModuleOutput*>::iterator iter;
	for (iter = modules.begin(); iter != modules.end(); iter++) {
		delete (*iter);
	}
}

static void delete_resource(void *ptr) {
	Resource *resource = (Resource *)ptr;
	delete resource;
}

void ProcessorOutput::runThread() {
	// get one item from srcQueue, process it and destroy
	while (1)  {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		Resource *resource = srcQueue->getResource(true);
		pthread_cleanup_push(delete_resource, resource);
		pthread_testcancel();
		pthread_cleanup_pop(0);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		vector<ModuleOutput*>::iterator iter;
		for (iter = modules.begin(); iter != modules.end(); iter++) {
			(*iter)->Process(resource);
		}
		delete resource;
	}
}

bool ProcessorOutput::Init(Config *config, const char *name) {
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
		if (strcmp(mtype, "ModuleOutput")) {
			LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Invalid module (must be ModuleOutput)");
			return false;
		}
		const char *lib = config->getValue(m, "lib");
		ModuleOutput *(*create)() = (ModuleOutput*(*)())loadLibrary(lib, "create");
		if (create == NULL) {
			LOG_CONFIG_ERROR0(logger, config->getXMLline(m, "lib"), config->getXMLcolumn(m, "lib"), "Cannot use module");
			return false;
		}
		ModuleOutput *module = (*create)();
		if (!module->Init(config, m))
			return false;
		this->modules.push_back(module);
	}

	return true;
}
