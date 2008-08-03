/**
 * Processing chain: call several modules
 */

#include <assert.h>
#include <stdlib.h>
#include "ProcessingChain.h"
#include "ProcessorInput.h"
#include "ProcessorSimple.h"
#include "ProcessorParallel.h"
#include "ProcessorOutput.h"

ProcessingChain::ProcessingChain() {
	running = false;
}

ProcessingChain::~ProcessingChain() {
	// TODO: delete processors, queues, ???
}

bool ProcessingChain::Init(Config *config, const char *name) {
	// queueSize
	int queueSize = config->getValueInt(name, "queueSize");
	if (queueSize == INT_MAX) {
		// TODO: log("Missing/invalid queueSize (int)")
		return false;
	}
	if (queueSize < 0 || queueSize > 100000) {
		// TODO: log("Invalid queueSize")
		return false;
	}

	// items
	int items = config->getSize(name, "item");
	for (int i = 0; i < items; i++) {
		const char *item = config->getValue(name, "item", i);
		assert(item != NULL);
		const char *ptype = config->getType(item);
		if (ptype == NULL) {
			// TODO: log("Unknown module");
			return false;
		}
		if (!strcmp(ptype, "ProcessorInput")) {
			if (i != 0) {
				// TODO log("Invalid module (ProcessorInput must be first)");
				return false;
			}
			ResourceQueue *rqDst = new ResourceQueue(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorInput(rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorSimple")) {
			if (i == 0 || i == items-1) {
				// TODO log("Invalid module (ProcessorSimple cannot be first or last in a chain)");
				return false;
			}
			ResourceQueue *rqSrc = queues.back();
			ResourceQueue *rqDst = new ResourceQueue(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorSimple(rqSrc, rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorParallel")) {
			if (i == 0 || i == items-1) {
				// TODO log("Invalid module (ProcessorParallel cannot be first or last in a chain)");
				return false;
			}
			ResourceQueue *rqSrc = queues.back();
			ResourceQueue *rqDst = new ResourceQueue(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorParallel(rqSrc, rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorOutput")) {
			if (i != items-1) {
				// TODO log("Invalid module (ProcessorOutput must be last)");
				return false;
			}
			ResourceQueue *rqSrc = queues.back();
			Processor *p = new ProcessorOutput(rqSrc);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else {
			// TODO: log("Unknown type")
			return false;
		}
	}

	return true;
}

void ProcessingChain::Start() {
	
}

void ProcessingChain::Stop() {
}
