/**
 * Processing chain: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "ProcessingChain.h"
#include "Processor.h"

log4cxx::LoggerPtr ProcessingChain::logger(log4cxx::Logger::getLogger("lib.processing_chain.ProcessingChain"));

ProcessingChain::ProcessingChain(ObjectRegistry *objects, const char *id): Object(objects, id) {
}

ProcessingChain::~ProcessingChain() {
	for (unsigned i = 0; i < processors.size(); i++) {
		delete processors[i];
	}
	for (unsigned i = 0; i < queues.size(); i++) {
		delete queues[i];
	}
}

bool ProcessingChain::Init(Config *config) {
	char buffer[1024];
	vector<string> *v;

	// crate children: queues
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/queue/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
			const char *qid = iter->c_str();
			Queue *q = new Queue(objects, qid);
			if (!q->Init(config))
				return false;
			queues.push_back(q);
		}
		delete v;
	}

	// create children: processors
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/processor/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
			const char *pid = iter->c_str();
			Processor *p = new Processor(objects, pid);
			if (!p->Init(config))
				return false;
			processors.push_back(p);
		}
		delete v;
	} else {
		LOG4CXX_INFO(logger, "No Processors for ProcessingChain: " << getId());
	}

	return true;
}

void ProcessingChain::Start() {
	for (unsigned i = 0; i < queues.size(); i++) {
		queues[i]->Start();
	}

	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Start();
	}
}

void ProcessingChain::Stop() {
	// cancel waiting threads
	for (unsigned i = 0; i < queues.size(); i++) {
		queues[i]->Stop();
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

char *ProcessingChain::getValue(const char *name) {
	return NULL;
}

bool ProcessingChain::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *ProcessingChain::listNames() {
	return new vector<string>();
}
