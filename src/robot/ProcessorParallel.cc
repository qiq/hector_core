
#include <assert.h>
#include "ProcessorParallel.h"

ProcessorParallel::ProcessorParallel(ResourceQueue *srcQueue, ResourceQueue *dstQueue) {
	this->srcQueue = srcQueue;
	this->dstQueue = dstQueue;
	module = NULL;
	maxRequests = 0;
}

ProcessorParallel::~ProcessorParallel() {
}

void ProcessorParallel::runThread() {
	Resource **inputRequests = new Resource*[maxRequests+1];
	Resource **outputRequests = new Resource*[maxRequests+1];
	int activeRequests = 0, finishedRequests = 0;
	while (running || finishedRequests > 0)  {
		if (running) {
			// get up to maxRequests Requests items from the srcQueue, blocked in case we have no running requests
			int n = srcQueue->getResources(inputRequests, maxRequests-activeRequests, activeRequests == 0);
			inputRequests[n] = NULL;
			activeRequests += n;

			// process new requests, get finished requests
			n = module->Process(inputRequests, outputRequests + finishedRequests);
			finishedRequests += n;
		}
		
		// put requests into dstQueue, blocked in case we have no running requests
		int n = dstQueue->putResources(outputRequests, finishedRequests, activeRequests == 0);
		// TODO: use cyclic buffer instead of copying?
		for (int i = 0; i < finishedRequests-n; i++) {
			outputRequests[i] = outputRequests[n+i];
		}
		finishedRequests -= n;
		activeRequests -= n;
	}
}

bool ProcessorParallel::Init(Config *config, const char *name) {
	// threads
	nThreads = config->getValueInt(name, "threads");
	if (nThreads == INT_MAX) {
		// TODO: log("Missing threads (int)")
		return false;
	}
	if (nThreads < 0 || nThreads > 100) {
		// TODO: log("Invalid number of threads (int)")
		return false;
	}

	// maxRequests
	maxRequests = config->getValueInt(name, "maxRequests");
	if (maxRequests == INT_MAX) {
		// TODO: log("Missing threads (int)")
		return false;
	}
	if (maxRequests < 0 || maxRequests > 100) {
		// TODO: log("Invalid number of threads (int)")
		return false;
	}

	// construct module
	if (config->getSize(name, "module") != 1) {
		// TODO: log("no module/too many modules");
		return false;
	}
	const char *m = config->getValue(name, "module");
	assert(m != NULL);
	const char *mtype = config->getType(m);
	if (mtype == NULL) {
		// TODO: log("Unknown type");
		return false;
	}
	if (strcmp(mtype, "ModuleParallel")) {
		// TODO log("Invalid module (must be ModuleParallel)"
		return false;
	}
	const char *lib = config->getValue(m, "lib");
	ModuleParallel *(*create)() = (ModuleParallel*(*)())loadLibrary(lib, "create");
	if (create == NULL) {
		// TODO log("Cannot use module");
		return false;
	}
	module = (*create)();
	if (!module->Init(config, m))
		return false;

	return true;
}