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
	propRun = false;
	propPause = false;

	getters["run"] = &ProcessingChain::getRun;
	setters["run"] = &ProcessingChain::setRun;
	getters["pause"] = &ProcessingChain::getPause;
	setters["pause"] = &ProcessingChain::setPause;
}

ProcessingChain::~ProcessingChain() {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		delete (*iter);
	}
}

char *ProcessingChain::getRun() {
	propertyLock.lock();
	bool r = propRun;
	propertyLock.unlock();
	return bool2str(r);
}

char *ProcessingChain::getPause() {
	propertyLock.lock();
	bool r = propPause;
	propertyLock.unlock();
	return bool2str(r);
}

void ProcessingChain::setRun(const char *value) {
	switch (str2bool(value)) {
	case 0:
		stop();
		break;
	case 1:
		start();
		break;
	default:
		LOG4CXX_ERROR(logger, "Invalid 'run' value: " << value);
	}
}

void ProcessingChain::setPause(const char *value) {
	switch (str2bool(value)) {
	case 0:
		resume();
		break;
	case 1:
		pause();
		break;
	default:
		LOG4CXX_ERROR(logger, "Invalid 'pause' value: " << value);
	}
}

void ProcessingChain::doPause() {
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->pause();
	}
}

void ProcessingChain::doResume() {
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->resume();
	}
}


bool ProcessingChain::init(Config *config) {
	char buffer[1024];
	vector<string> *v;

	// create children: processors
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingChain[@id='%s']/processor/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		// create and initialize all Processors
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *pid = iter->c_str();
			Processor *p = new Processor(objects, pid);
			if (!p->init(config))
				return false;
			processors.push_back(p);
		}
		// connect Processors to other Processors
		for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
			(*iter)->connect();
		}
		delete v;
	} else {
		LOG4CXX_INFO(logger, "No Processors for ProcessingChain: " << getId());
	}

	return true;
}

void ProcessingChain::start() {
	propertyLock.lock();
	if (!propRun) {
		for (unsigned i = 0; i < processors.size(); i++) {
			processors[i]->start();
		}
		propRun = true;
	}
	propertyLock.unlock();
}

void ProcessingChain::stop() {
	propertyLock.lock();
	if (propRun) {
		if (propPause) {
			doResume();
			propPause = false;
		}
		// cancel running threads and join all threads
		for (unsigned i = 0; i < processors.size(); i++) {
			processors[i]->stop();
		}
		propRun = false;
	}
	propertyLock.unlock();
}

void ProcessingChain::pause() {
	propertyLock.lock();
	if (propRun && !propPause) {
		doPause();
		propPause = true;
	}
	propertyLock.unlock();
}

void ProcessingChain::resume() {
	propertyLock.lock();
	if (propPause) {
		doResume();
		propPause = false;
	}
	propertyLock.unlock();
}

void ProcessingChain::createCheckpoint() {
	// for all modules try to create a checkpoint
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->createCheckpoint();
	}
}

char *ProcessingChain::getValue(const char *name) {
	char *result = NULL;
	std::tr1::unordered_map<string, char*(ProcessingChain::*)()>::iterator iter = getters.find(name);
	if (iter != getters.end())
		result = (this->*(iter->second))();
	return result;
}

bool ProcessingChain::setValue(const char *name, const char *value) {
	std::tr1::unordered_map<string, void(ProcessingChain::*)(const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(this->*(iter->second))(value);
		return true;
	}
	return false;
}

vector<string> *ProcessingChain::listNames() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(ProcessingChain::*)()>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}
