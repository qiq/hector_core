/**
 * Processing chain: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "ProcessingChain.h"
#include "Processor.h"
#include "Server.h"

log4cxx::LoggerPtr ProcessingChain::logger(log4cxx::Logger::getLogger("lib.processing_chain.ProcessingChain"));

ProcessingChain::ProcessingChain() {
}

ProcessingChain::~ProcessingChain() {
	for (unsigned i = 0; i < processors.size(); i++) {
		server->unregisterObject(processors[i]->getId());
		delete processors[i];
	}
	for (unsigned i = 0; i < queues.size(); i++) {
		server->unregisterObject(queues[i]->getId());
		delete queues[i];
	}
}

bool ProcessingChain::Init(Server *server, Config *config, const char *id) {
	char buffer[1024];
	vector<string> *v;

	this->server = server;

	// create children: processors
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/processor/@ref", id);
	v = config->getValues(buffer);
	for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
		const char *pid = iter->c_str();
		Processor *p = new Processor();
		if (!p->Init(server, config, pid))
			return false;
		processors.push_back(p);
		server->registerObject(p);
	}
	delete v;

	// crate children: queues
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/queue/@ref", id);
	v = config->getValues(buffer);
	for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
		const char *pid = iter->c_str();
		Queue *q = new Queue();
		if (!q->Init(server, config, pid))
			return false;
		queues.push_back(q);
		server->registerObject(q);
	}
	delete v;

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

const char *ProcessingChain::getValue(const char *name) {
	return NULL;
}

bool ProcessingChain::setValue(const char *name, const char *value) {
	return false;
}
