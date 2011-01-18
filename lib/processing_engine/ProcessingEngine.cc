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

	values->AddGetter("run", &ProcessingEngine::getRun);
	values->AddSetter("run", &ProcessingEngine::setRun);
	values->AddGetter("pause", &ProcessingEngine::getPause);
	values->AddSetter("pause", &ProcessingEngine::setPause);
	values->AddGetter("resourceCount", &ProcessingEngine::getResourceCount);
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
	v = config->GetValues(buffer);
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
	char *ref = config->GetFirstValue(buffer);
	if (ref) {
		Processor *p = dynamic_cast<Processor*>(objects->GetObject(ref));
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

// get processed resource from the processing engine, false: cancelled or timeout
bool ProcessingEngine::GetProcessedResources(tr1::unordered_set<int> *ids, vector<Resource*> *output, struct timeval *timeout) {
	if (!outputQueue)
		return false;

	bool result = true;	// false: cancelled or timeout
	pauseLock.Lock();
	pauseLock.Unlock();
	finishedLock.Lock();
	tr1::unordered_set<int>::iterator iter = ids->begin();
	while (iter != ids->end()) {
		int id = *iter;
		tr1::unordered_map<int, Resource*>::iterator iter2 = finishedResources.find(id);
		if (iter2 != finishedResources.end()) {
			// resource found
			output->push_back(iter2->second);
			finishedResources.erase(id);
			++iter;
			ids->erase(id);
			continue;
		}
		// we cannot wait for a resource
		if (!timeout) {
			++iter;
			continue;
		}
		if (waitingInQueue) {
			waiting++;
			bool timedOut = !finishedLock.WaitSend(timeout);
			waiting--;
			if (cancel || timedOut) {
				// timed out: just check all remaining ids,
				// whether they are ready, but do not wait for them
				timeout = NULL;
				iter = ids->begin();
				result = false;
				continue;
			}
		} else {
			waitingInQueue = true;
			finishedLock.Unlock();
			Resource *r = outputQueue->getItem(timeout);
			finishedLock.Lock();
			waitingInQueue = false;
			if (!r) {
				// cancel or timeout
				finishedLock.SignalSend();
				timeout = NULL;
				iter = ids->begin();
				result = false;
				continue;
			}
			bool found_my = false;
			bool found_foreign = false;
			while (r) {
				id = r->getId();
				if (ids->find(id) != ids->end()) {
					output->push_back(r);
					ids->erase(id);
					found_my = true;
				} else {
					finishedResources[id] = r;
					found_foreign = true;
				}
				r = outputQueue->getItem(NULL);
			}
			// re-set iterator if we deleted some of ids
			if (found_my)
				iter = ids->begin();
			if (found_foreign)
				finishedLock.SignalSend();
		}
	}
	finishedLock.Unlock();
	return result;
}

// get processed resource from the processing engine, false: cancelled or timeout
bool ProcessingEngine::GetProcessedResourcesOrdered(vector<int> *ids, vector<Resource*> *output, struct timeval *timeout) {
	if (!outputQueue)
		return false;	// false: cancelled or timeout

	pauseLock.Lock();
	pauseLock.Unlock();
	finishedLock.Lock();
	int index = 0;
	while (index < (int)ids->size()) {
		int id = (*ids)[index];
		tr1::unordered_map<int, Resource*>::iterator iter = finishedResources.find(id);
		if (iter != finishedResources.end()) {
			// resource found
			output->push_back(iter->second);
			finishedResources.erase(id);
			index++;
			continue;
		}
		// we cannot wait for a resource
		if (!timeout) {
			finishedLock.Unlock();
			return true;
		}
		if (waitingInQueue) {
			waiting++;
			bool timedOut = !finishedLock.WaitSend(timeout);
			waiting--;
			if (cancel || timedOut) {
				finishedLock.Unlock();
				return false;
			}
		} else {
			waitingInQueue = true;
			finishedLock.Unlock();
			Resource *r = outputQueue->getItem(timeout);
			finishedLock.Lock();
			waitingInQueue = false;
			if (!r) {
				// cancel or timeout
				finishedLock.SignalSend();
				finishedLock.Unlock();
				return false;
			}
			bool found_foreign = false;
			while (r) {
				id = r->getId();
				if ((*ids)[index] == id) {
					output->push_back(r);
					index++;
				} else {
					finishedResources[id] = r;
					found_foreign = true;
				}
				r = outputQueue->getItem(NULL);
			}
			if (found_foreign) {
				finishedLock.SignalSend();
				finishedLock.Unlock();
				sched_yield();
				finishedLock.Lock();
			}
		}
	}
	finishedLock.Unlock();
	return true;
}

// get processed resource from the processing engine, false: not available or cancelled
/*
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
}*/

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
		ObjectUnlock();		// we have to unlock, so that Processor threads may be cancelled and joined
		if (outputQueue)
			outputQueue->Stop();
		// cancel running threads and join all threads
		for (unsigned i = 0; i < processors.size(); i++) {
			processors[i]->Stop();
		}
		ObjectLockWrite();
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

bool ProcessingEngine::SaveCheckpoint(const char *path) {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		if (!(*iter)->LockSaveCheckpoint(path))
			return false;
	}
	return true;
}

bool ProcessingEngine::RestoreCheckpoint(const char *path) {
	for (vector<Processor*>::iterator iter = processors.begin(); iter != processors.end(); ++iter) {
		if (!(*iter)->LockRestoreCheckpoint(path))
			return false;
	}
	return true;
}

char *ProcessingEngine::GetValue(const char *name) {
	return values->GetValue(name);
}

bool ProcessingEngine::SetValue(const char *name, const char *value) {
	return values->SetValue(name, value);
}

bool ProcessingEngine::IsInitOnly(const char *name) {
	return values->IsInitOnly(name);
}

vector<string> *ProcessingEngine::ListNames() {
	return values->ListNames();
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

