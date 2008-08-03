
#include <assert.h>
#include "ProcessorOutput.h"

ProcessorOutput::ProcessorOutput(ResourceQueue *srcQueue) {
	this->srcQueue = srcQueue;
}

ProcessorOutput::~ProcessorOutput() {
}

void ProcessorOutput::runThread() {
	// get one item from somewhere, process it and put it into dstQueue
	while (running)  {
		Resource *resource = srcQueue->getResource(true);
		module->Process(resource);
	}
}

bool ProcessorOutput::Init(Config *config, const char *name) {
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
	if (strcmp(mtype, "ModuleOutput")) {
		// TODO log("Invalid module (must be ModuleOutput)"
		return false;
	}
	const char *lib = config->getValue(m, "lib");
	ModuleOutput *(*create)() = (ModuleOutput*(*)())loadLibrary(lib, "create");
	if (create == NULL) {
		// TODO log("Cannot use module");
		return false;
	}
	module = (*create)();
	if (!module->Init(config, m))
		return false;

	return true;
}
