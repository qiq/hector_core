/**
 * Processing engine: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "Config.h"
#include "ProcessingEngine.h"
#include "Processor.h"
#include "Resources.h"

using namespace std;

ProcessingEngine::ProcessingEngine(ObjectRegistry *objects, const char *id): Object(objects, id) {
	propRun = false;
	propPause = false;
	inputQueue = NULL;
	outputQueue = NULL;
	waitingInQueue = false;
	waiting = 0;
	cancel = false;

	values = new ObjectValues<ProcessingEngine>(this);

	values->addGetter("run", &ProcessingEngine::getRun);
	values->addSetter("run", &ProcessingEngine::setRun);
	values->addGetter("pause", &ProcessingEngine::getPause);
	values->addSetter("pause", &ProcessingEngine::setPause);
}

ProcessingEngine::~ProcessingEngine() {
	// NB: we do not own inputQueue
	delete outputQueue;
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
			Processor *p = new Processor(objects, this, pid);
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

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "//ProcessingEngine[@id='%s']/inputProcessor/@ref", getId());
	char *ref = config->getFirstValue(buffer);
	if (ref) {
		Processor *p = dynamic_cast<Processor*>(objects->getObject(ref));
		if (!p) {
			LOG_ERROR("Processor not found: " << ref);
			return false;
		}
		inputQueue = p->getInputQueue();
		if (!inputQueue) {
			LOG_ERROR("No input queue defined for processor: " << ref);
			return false;
		}
		free(ref);
	}

	return true;
}

// process resource using the processing engine
bool ProcessingEngine::PutResource(Resource *resource, bool sleep) {
	return inputQueue->putItem(resource, sleep, 0);
}

// get processed resource from the processing engine, NULL: deleted/not available (also cancelled)
Resource *ProcessingEngine::GetResource(int id, bool sleep) {
	Resource *resource;
	assert(outputQueue != NULL);
	pauseLock.Lock();
	pauseLock.Unlock();
	finishedLock.Lock();
	tr1::unordered_map<int, Resource*>::iterator iter;
	while ((iter = finishedResources.find(id)) == finishedResources.end()) {
		if (waitingInQueue) {
			waiting++;
			finishedLock.WaitSend();
			waiting--;
			if (cancel) {
				finishedLock.Unlock();
				return NULL;
			}
		} else {
			waitingInQueue = true;
			finishedLock.Unlock();
			resource = outputQueue->getItem(true);
			finishedLock.Lock();
			waitingInQueue = false;
			if (!resource) {	// cancelled
				finishedLock.Unlock();
				return NULL;
			}
			if (resource->getId() == id) {
				finishedLock.Unlock();
				return resource;
			}
			finishedResources[resource->getId()] = resource;
			finishedLock.SignalSend();
			finishedLock.Unlock();
			sched_yield();
			finishedLock.Lock();
		}
	}
	resource = iter->second;
	finishedResources.erase(id);
	finishedLock.Unlock();
	return resource;
}

int ProcessingEngine::ResourceNameToId(const char *name) {
	return Resources::NameToId(name);
}

Resource *ProcessingEngine::CreateResource(int id) {
	return Resources::CreateResource(id);
}

void ProcessingEngine::DeleteResource(Resource *resource) {
	int id = resource->getId();
	finishedLock.Lock();
	if (finishedResources.find(id) == finishedResources.end())
		finishedResources[id] = NULL;
	finishedLock.Unlock();
	delete resource;
}

void ProcessingEngine::StartSync() {
	if (!propRun) {
		cancel = false;
		if (outputQueue)
			outputQueue->Start();
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

		// cancel waiting readers
		cancel = true;
		while (waiting > 0) {
			finishedLock.SignalRecv();
			finishedLock.Unlock();
			sched_yield();
			finishedLock.Lock();
		}

		// cancel reader in the queue
		if (outputQueue)
			outputQueue->Stop();
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
	pauseLock.Lock();
	if (outputQueue)
		outputQueue->Pause();
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Pause();
	}
}

void ProcessingEngine::doResume() {
	pauseLock.Unlock();
	if (outputQueue)
		outputQueue->Resume();
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Resume();
	}
}

