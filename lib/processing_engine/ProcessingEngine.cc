/**
 * Processing engine: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "ProcessingEngine.h"
#include "Processor.h"

using namespace std;

ProcessingEngine::ProcessingEngine(ObjectRegistry *objects, const char *id): Object(objects, id) {
	propRun = false;
	propPause = false;

	values = new ObjectValues<ProcessingEngine>(this);

	values->addGetter("run", &ProcessingEngine::getRun);
	values->addSetter("run", &ProcessingEngine::setRun);
	values->addGetter("pause", &ProcessingEngine::getPause);
	values->addSetter("pause", &ProcessingEngine::setPause);
}

ProcessingEngine::~ProcessingEngine() {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		delete (*iter);
	}
	delete values;
}

bool ProcessingEngine::Init(Config *config) {
	char buffer[1024];
	vector<string> *v;

	// create children: processors
	snprintf(buffer, sizeof(buffer), "//ProcessingEngine[@id='%s']/Processor/@id", getId());
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
			if (!(*iter)->Connect())
				return false;
		}
		delete v;
	} else {
		LOG_INFO("No Processors");
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
	return values->getValueSync(name);
}

bool ProcessingEngine::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

bool ProcessingEngine::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

vector<string> *ProcessingEngine::listNamesSync() {
	return values->listNamesSync();
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
		LOG_ERROR("Invalid 'run' value: " << value);
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
		LOG_ERROR("Invalid 'pause' value: " << value);
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

