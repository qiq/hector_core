/**
 * Processing engine: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "Config.h"
#include "ProcessingEngine.h"
#include "Processor.h"

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
	values->addGetter("resourceCount", &ProcessingEngine::getResourceCount);
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
	// second stage?
	if (!config) {
		for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
			if (!(*iter)->Init(NULL))
				return false;
		}
		return true;
	}

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
		delete v;
	} else {
		LOG_INFO(this, "No Processors");
	}

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "//ProcessingEngine[@id='%s']/inputProcessor/@ref", getId());
	char *ref = config->getFirstValue(buffer);
	if (ref) {
		Processor *p = dynamic_cast<Processor*>(objects->getObject(ref));
		if (!p) {
			LOG_ERROR(this, "Processor not found: " << ref);
			return false;
		}
		inputQueue = p->getInputQueue();
		if (!inputQueue) {
			LOG_ERROR(this, "No input queue defined for processor: " << ref);
			return false;
		}
		free(ref);
	}

	return true;
}

// process resource using the processing engine
bool ProcessingEngine::ProcessResource(Resource *resource, struct timeval *timeout) {
	if (!inputQueue)
		return false;
	return inputQueue->putItem(resource, timeout, 0);
}

// get processed resource from the processing engine, false: not available or cancelled
Resource *ProcessingEngine::GetProcessedResource(int id, struct timeval *timeout) {
	if (!outputQueue)
		return NULL;

	pauseLock.Lock();
	pauseLock.Unlock();
	finishedLock.Lock();
	tr1::unordered_map<int, Resource*>::iterator iter;
	while ((iter = finishedResources.find(id)) == finishedResources.end()) {
		if (!timeout) {
			finishedLock.Unlock();
			return NULL;		// not available
		}
		if (waitingInQueue) {
			waiting++;
			bool timedOut = !finishedLock.WaitSend(timeout);
			waiting--;
			if (cancel || timedOut) {
				finishedLock.Unlock();
				return NULL;	// cancelled or timeout
			}
		} else {
			waitingInQueue = true;
			finishedLock.Unlock();
			Resource *r = outputQueue->getItem(timeout);
			finishedLock.Lock();
			waitingInQueue = false;
			if (!r) {
				finishedLock.SignalSend();
				finishedLock.Unlock();
				return NULL;	// cancelled or timeout
			}
			Resource *found = NULL;
			while (r) {
				if (r->getId() == id)
					found = r;
				else
					finishedResources[r->getId()] = r;
				r = outputQueue->getItem(NULL);
			}
			finishedLock.SignalSend();
			finishedLock.Unlock();
			if (found)
				return found;
			sched_yield();
			finishedLock.Lock();
		}
	}
	Resource *result = iter->second;
	finishedResources.erase(id);
	finishedLock.Unlock();
	return result;
}

void ProcessingEngine::StartSync() {
	if (!propRun) {
		cancel = false;
		resourceCount = 0;
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

bool ProcessingEngine::SaveCheckpointSync(const char *path) {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		if (!(*iter)->SaveCheckpoint(path))
			return false;
	}
	return true;
}

bool ProcessingEngine::RestoreCheckpointSync(const char *path) {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		if (!(*iter)->RestoreCheckpoint(path))
			return false;
	}
	return true;
}

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

void ProcessingEngine::setRun(const char *name, const char *value) {
	switch (str2bool(value)) {
	case 0:
		StopSync();
		break;
	case 1:
		StartSync();
		break;
	default:
		LOG_ERROR(this, "Invalid 'run' value: " << value);
	}
}

char *ProcessingEngine::getPause(const char *name) {
	return bool2str(propPause);
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
		LOG_ERROR(this, "Invalid 'pause' value: " << value);
	}
}

char *ProcessingEngine::getResourceCount(const char *name) {
	return int2str(resourceCount);
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

