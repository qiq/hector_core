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

log4cxx::LoggerPtr ProcessingChain::logger(log4cxx::Logger::getLogger("robot.ProcessingChain"));

ProcessingChain::ProcessingChain() {
}

ProcessingChain::~ProcessingChain() {
	for (unsigned i = 0; i < processors.size(); i++) {
		delete processors[i];
	}
	for (unsigned i = 0; i < queues.size(); i++) {
		delete queues[i];
	}
}

bool ProcessingChain::Init(Config *config, const char *name) {
	// queueSize
	int queueSize = config->getValueInt(name, "queueSize");
	if (queueSize == INT_MAX) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "queueSize"), config->getXMLcolumn(name, "queueSize"), "Missing/invalid queueSize");
		return false;
	}
	if (queueSize < 0 || queueSize > 100000) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "queueSize"), config->getXMLcolumn(name, "queueSize"), "Invalid queueSize");
		return false;
	}

	// items
	int items = config->getSize(name, "item");
	for (int i = 0; i < items; i++) {
		const char *item = config->getValue(name, "item", i);
		assert(item != NULL);
		const char *ptype = config->getType(item);
		if (ptype == NULL) {
			LOG_CONFIG_ERROR1(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Unknown module: %s", item);
			return false;
		}
		if (!strcmp(ptype, "ProcessorInput")) {
			if (i != 0) {
				LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Invalid module (ProcessorInput must be first)");
				return false;
			}
			SyncQueue<Resource> *rqDst = new SyncQueue<Resource>(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorInput(rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorSimple")) {
			if (i == 0 || i == items-1) {
				LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Invalid module (ProcessorSimple cannot be first or last in a chain)");
				return false;
			}
			SyncQueue<Resource> *rqSrc = queues.back();
			SyncQueue<Resource> *rqDst = new SyncQueue<Resource>(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorSimple(rqSrc, rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorParallel")) {
			if (i == 0 || i == items-1) {
				LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Invalid module (ProcessorParallel cannot be first or last in a chain)");
				return false;
			}
			SyncQueue<Resource> *rqSrc = queues.back();
			SyncQueue<Resource> *rqDst = new SyncQueue<Resource>(queueSize);
			queues.push_back(rqDst);
			Processor *p = new ProcessorParallel(rqSrc, rqDst);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else if (!strcmp(ptype, "ProcessorOutput")) {
			if (i != items-1) {
				LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Invalid module (ProcessorOutput must be last)");
				return false;
			}
			SyncQueue<Resource> *rqSrc = queues.back();
			Processor *p = new ProcessorOutput(rqSrc);
			if (!p->Init(config, item))
				return false;
			processors.push_back(p);
		} else {
			LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "item"), config->getXMLcolumn(name, "item"), "Unknown type");
			return false;
		}
	}

	return true;
}

void ProcessingChain::Start() {
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Start();
	}
}

void ProcessingChain::Stop() {
	// cancel waiting threads
	for (unsigned i = 0; i < queues.size(); i++) {
		queues[i]->cancelAll();
	}
	// cancel running threads and join all threads
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Stop();
	}
}

void ProcessingChain::createCheckpoint() {
	// for all modules try to create a checkpoint
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->createCheckpoint();
	}
}
