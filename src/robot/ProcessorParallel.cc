
#include <assert.h>
#include "ProcessorParallel.h"

log4cxx::LoggerPtr ProcessorParallel::logger(log4cxx::Logger::getLogger("robot.ProcessorParallel"));

ProcessorParallel::ProcessorParallel(SyncQueue<Resource> *srcQueue, SyncQueue<Resource> *dstQueue) {
	this->srcQueue = srcQueue;
	this->dstQueue = dstQueue;
	module = NULL;
	maxRequests = 0;
}

ProcessorParallel::~ProcessorParallel() {
	delete module;
}

void ProcessorParallel::runThread() {
	Resource **inputResources = new Resource*[maxRequests+1];
	Resource **outputResources = new Resource*[maxRequests+1];
	int activeResources = 0;
	int finishedResources = 0;
	inputResources[0] = NULL;
	while (Running())  {
		// get up to maxRequests Resources items from the srcQueue, blocked in case we have no running requests
		int n = srcQueue->getItems(inputResources, maxRequests-activeResources, activeResources == 0);
		inputResources[activeResources+n] = NULL;
		activeResources += n;

		// process new requests, get finished requests
		n = module->Process(inputResources, outputResources + finishedResources);
		finishedResources += n;
		inputResources[0] = NULL;
		
		// put requests into dstQueue, blocked in case we have no running requests
		n = dstQueue->putItems(outputResources, finishedResources, activeResources == 0);
		// TODO: use cyclic buffer instead of copying?
		for (int i = 0; i < finishedResources-n; i++) {
			outputResources[i] = outputResources[n+i];
		}
		finishedResources -= n;
		activeResources -= n;
	}
	for (int i = 0; inputResources[i] != NULL; i++) {
		delete inputResources[i];
	}
	delete[] inputResources;
	for (int i = 0; i < finishedResources; i++) {
		delete outputResources[i];
	}
	delete[] outputResources;
}

bool ProcessorParallel::Init(Config *config, const char *name) {
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

	// maxRequests
	maxRequests = config->getValueInt(name, "maxRequests");
	if (maxRequests == INT_MAX) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "maxRequests"), config->getXMLcolumn(name, "maxRequests"), "Missing number of requests (int)");
		return false;
	}
	if (maxRequests < 0 || maxRequests > 100) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "maxRequests"), config->getXMLcolumn(name, "maxRequests"), "Invalid number of requests (int)");
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
		LOG_CONFIG_ERROR1(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Unknown type %s", mtype);
		return false;
	}
	if (strcmp(mtype, "ModuleParallel")) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "module"), config->getXMLcolumn(name, "module"), "Invalid module (must be ModuleParallel)");
		return false;
	}
	const char *lib = config->getValue(m, "lib");
	ModuleParallel *(*create)() = (ModuleParallel*(*)())loadLibrary(lib, "create");
	if (create == NULL) {
		LOG_CONFIG_ERROR1(logger, config->getXMLline(m, "lib"), config->getXMLcolumn(m, "lib"), "Cannot use module %s", lib);
		return false;
	}
	module = (*create)();
	if (!module->Init(config, m))
		return false;

	return true;
}
