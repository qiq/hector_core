/**
 * Processing chain: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "ProcessingEngine.h"
#include "Processor.h"

log4cxx::LoggerPtr ProcessingEngine::logger(log4cxx::Logger::getLogger("lib.processing_enging.ProcessingEngine"));

ProcessingEngine::ProcessingEngine(ObjectRegistry *objects, const char *id): Object(objects, id) {
	propRun = false;
	propPause = false;

	getters["run"] = &ProcessingEngine::getRun;
	setters["run"] = &ProcessingEngine::setRun;
	getters["pause"] = &ProcessingEngine::getPause;
	setters["pause"] = &ProcessingEngine::setPause;
}

ProcessingEngine::~ProcessingEngine() {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		delete (*iter);
	}
}

bool ProcessingEngine::Init(Config *config) {
	char buffer[1024];
	vector<string> *v;

	// create children: processors
	snprintf(buffer, sizeof(buffer), "/Config/ProcessingEngine[@id='%s']/processor/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		// create and initialize all Processors
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *pid = iter->c_str();
			Processor *p = new Processor(objects, pid);
			if (!p->Init(config))
				return false;
			processors.push_back(p);
		}
		// connect Processors to other Processors
		for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
			(*iter)->Connect();
		}
		delete v;
	} else {
		LOG4CXX_INFO(logger, "No Processors for ProcessingEngine: " << getId());
	}

	return true;
}

void ProcessingEngine::StartSync() {
	if (!propRun) {
		for (unsigned i = 0; i < processors.size(); i++) {
			processors[i]->Start();
		}
		propRun = true;
	}
}

void ProcessingEngine::StopSync() {
	if (propRun) {
		if (propPause) {
			doResume();
			propPause = false;
		}
		// cancel running threads and join all threads
		for (unsigned i = 0; i < processors.size(); i++) {
			processors[i]->Stop();
		}
		propRun = false;
	}
}

void ProcessingEngine::PauseSync() {
	if (propRun && !propPause) {
		doPause();
		propPause = true;
	}
}

void ProcessingEngine::ResumeSync() {
	if (propPause) {
		doResume();
		propPause = false;
	}
}

//void ProcessingEngine::createCheckpoint() {
	// for all modules try to create a checkpoint
//	for (unsigned i = 0; i < processors.size(); i++) {
		//TODO processors[i]->createCheckpoint();
//	}
//}

char *ProcessingEngine::getValueSync(const char *name) {
	char *result = NULL;
	std::tr1::unordered_map<string, char*(ProcessingEngine::*)(const char*)>::iterator iter = getters.find(name);
	if (iter != getters.end())
		result = (this->*(iter->second))(name);
	return result;
}

bool ProcessingEngine::setValueSync(const char *name, const char *value) {
	std::tr1::unordered_map<string, void(ProcessingEngine::*)(const char*, const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(this->*(iter->second))(name, value);
		return true;
	}
	return false;
}

vector<string> *ProcessingEngine::listNamesSync() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(ProcessingEngine::*)(const char*)>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

char *ProcessingEngine::getRun(const char *name) {
	return bool2str(propRun);
}

char *ProcessingEngine::getPause(const char *name) {
	return bool2str(propPause);
}

void ProcessingEngine::setRun(const char *name, const char *value) {
	switch (str2bool(value)) {
	case 0:
		StopSync();
		break;
	case 1:
		StartSync();
		break;
	default:
		LOG4CXX_ERROR(logger, "Invalid 'run' value: " << value);
	}
}

void ProcessingEngine::setPause(const char *name, const char *value) {
	switch (str2bool(value)) {
	case 0:
		ResumeSync();
		break;
	case 1:
		PauseSync();
		break;
	default:
		LOG4CXX_ERROR(logger, "Invalid 'pause' value: " << value);
	}
}

void ProcessingEngine::doPause() {
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Pause();
	}
}

void ProcessingEngine::doResume() {
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Resume();
	}
}

