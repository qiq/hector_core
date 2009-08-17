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
	running = false;

	getters["running"] = &ProcessingChain::getRunning;
	setters["running"] = &ProcessingChain::setRunning;
}

ProcessingChain::~ProcessingChain() {
	// order of destruction is important: processors use queues
	for (unsigned i = 0; i < processors.size(); i++) {
		delete processors[i];
	}
	for (unsigned i = 0; i < queues.size(); i++) {
		delete queues[i];
	}
}

char *ProcessingChain::getRunning() {
	propertyLock.lock();
	bool r = running;
	propertyLock.unlock();
	return r ? strdup("1") : strdup("0");
}

void ProcessingChain::setRunning(const char *value) {
	propertyLock.lock();
	if (!strcmp(value, "0")) {
		if (running) {
			stop();
			running = false;
		}
	} else if (!strcmp(value, "1")) {
		if (!running) {
			start();
			running = true;
		}
	} else {
		LOG4CXX_ERROR(logger, "Invalid 'running' value: " << value);
	}
	propertyLock.unlock();
}

bool ProcessingChain::init(Config *config) {
	char buffer[1024];
	vector<string> *v;

	// crate children: queues
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/queue/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *qid = iter->c_str();
			Queue *q = new Queue(objects, qid);
			if (!q->init(config))
				return false;
			queues.push_back(q);
		}
		delete v;
	}

	// create children: processors
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/processor/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *pid = iter->c_str();
			Processor *p = new Processor(objects, pid);
			if (!p->init(config))
				return false;
			processors.push_back(p);
		}
		delete v;
	} else {
		LOG4CXX_INFO(logger, "No Processors for ProcessingChain: " << getId());
	}

	return true;
}

void ProcessingChain::start() {
	for (unsigned i = 0; i < queues.size(); i++) {
		queues[i]->start();
	}

	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->start();
	}
}

void ProcessingChain::stop() {
	// cancel waiting threads
	for (unsigned i = 0; i < queues.size(); i++) {
		queues[i]->stop();
	}
	// cancel running threads and join all threads
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->stop();
	}
}

void ProcessingChain::createCheckpoint() {
	// for all modules try to create a checkpoint
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->createCheckpoint();
	}
}

char *ProcessingChain::getValue(const char *name) {
	char *result = NULL;
	stdext::hash_map<string, char*(ProcessingChain::*)(), string_hash>::iterator iter = getters.find(name);
	if (iter != getters.end())
		result = (this->*(iter->second))();
	return result;
}

bool ProcessingChain::setValue(const char *name, const char *value) {
	stdext::hash_map<string, void(ProcessingChain::*)(const char*), string_hash>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(this->*(iter->second))(value);
		return true;
	}
	return false;
}

vector<string> *ProcessingChain::listNames() {
	vector<string> *result = new vector<string>();
	for (stdext::hash_map<string, char*(ProcessingChain::*)(), string_hash>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}
