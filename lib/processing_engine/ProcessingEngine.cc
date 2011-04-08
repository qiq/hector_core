/**
 * Processing engine: call several modules
 */

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "Config.h"
#include "ProcessingEngine.h"
#include "Processor.h"
#include "Server.h"

using namespace std;

ProcessingEngine::ProcessingEngine(ObjectRegistry *objects, const char *id, Server *server, bool batch): Object(objects, id) {
	this->server = server;
	this->batch = batch;
	propRun = false;
	propPause = false;
	inputQueue = NULL;
	outputQueue = NULL;
	waitingInQueue = false;
	waiting = 0;
	cancel = false;
	processorsSleeping = 0;
	sleeping = false;

	values = new ObjectProperties<ProcessingEngine>(this);
	values->Add("run", &ProcessingEngine::GetRun, &ProcessingEngine::SetRun);
	values->Add("pause", &ProcessingEngine::GetPause, &ProcessingEngine::SetPause);
	values->Add("resourceCount", &ProcessingEngine::GetResourceCount);
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
	snprintf(buffer, sizeof(buffer), "//ProcessingEngine[@id='%s']/Processor/@id", GetId());
	v = config->GetValues(buffer);
	if (v) {
		// create and initialize all Processors
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *pid = iter->c_str();
			Processor *p = new Processor(objects, pid, this, batch);
			if (!p->Init(config))
				return false;
			processors.push_back(p);
		}
		delete v;
	} else {
		LOG_INFO(this, "No Processors");
	}

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "//ProcessingEngine[@id='%s']/inputProcessor/@ref", GetId());
	char *ref = config->GetFirstValue(buffer);
	if (ref) {
		Processor *p = dynamic_cast<Processor*>(objects->GetObject(ref));
		if (!p) {
			LOG_ERROR(this, "Processor not found: " << ref);
			return false;
		}
		inputQueue = p->GetInputQueue();
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
	return inputQueue->PutItem(resource, timeout, 0);
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
			Resource *r = outputQueue->GetItem(timeout);
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
				id = r->GetId();
				if (ids->find(id) != ids->end()) {
					output->push_back(r);
					ids->erase(id);
					found_my = true;
				} else {
					finishedResources[id] = r;
					found_foreign = true;
				}
				r = outputQueue->GetItem(NULL);
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
			Resource *r = outputQueue->GetItem(timeout);
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
				id = r->GetId();
				if ((*ids)[index] == id) {
					output->push_back(r);
					index++;
				} else {
					finishedResources[id] = r;
					found_foreign = true;
				}
				r = outputQueue->GetItem(NULL);
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
			DoResume();
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
		DoPause();
		propPause = true;
	}
}

void ProcessingEngine::ResumeSync() {
	if (propPause) {
		DoResume();
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

char *ProcessingEngine::GetPropertySync(const char *name) {
	return values->GetProperty(name);
}

bool ProcessingEngine::SetPropertySync(const char *name, const char *value) {
	return values->SetProperty(name, value);
}

vector<string> *ProcessingEngine::ListPropertiesSync() {
	return values->ListProperties();
}

char *ProcessingEngine::GetRun(const char *name) {
	return bool2str(propRun);
}

void ProcessingEngine::SetRun(const char *name, const char *value) {
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

char *ProcessingEngine::GetPause(const char *name) {
	return bool2str(propPause);
}

void ProcessingEngine::SetPause(const char *name, const char *value) {
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

char *ProcessingEngine::GetResourceCount(const char *name) {
	return int2str(resourceCount);
}

void ProcessingEngine::DoPause() {
	pauseLock.Lock();
	if (outputQueue)
		outputQueue->Pause();
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Pause();
	}
}

void ProcessingEngine::DoResume() {
	pauseLock.Unlock();
	if (outputQueue)
		outputQueue->Resume();
	for (unsigned i = 0; i < processors.size(); i++) {
		processors[i]->Resume();
	}
}

void ProcessingEngine::CheckSleeping() {
	ObjectLockWrite();
	bool report_sleeping = false;
	bool report_wakeup = false;
	if (!sleeping) {
		if ((unsigned)processorsSleeping == processors.size() && resourceCount == 0) {
			report_sleeping = true;
			sleeping = true;
		}
	} else {
		if ((unsigned)processorsSleeping != processors.size() || resourceCount > 0) {
			report_wakeup = true;
			sleeping = false;
		}
	}
	ObjectUnlock();
	if (report_sleeping)
		server->ProcessingEngineSleeping();
	if (report_wakeup)
		server->ProcessingEngineWakeup();
}
