/**
 *
 */

#include <config.h>

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <ltdl.h>
#include "common.h"
#include "Config.h"
#include "OutputFilter.h"
#include "PerlModule.h"
#include "PythonModule.h"
#include "ProcessingEngine.h"
#include "Processor.h"
#include "LibraryLoader.h"

using namespace std;

Processor::Processor(ObjectRegistry *objects, const char *id, ProcessingEngine *engine, bool batch): Object(objects, id) {
	this->engine = engine;
	this->batch = batch;
	nThreads = 0;
	threads = NULL;
	running = false;
	sleeping = false;
	runningThreads = 0;
	sleepingThreads = 0;
	pauseInput = false;

	props = new ObjectProperties<Processor>(this);
	props->Add("pauseInput", &Processor::GetPauseInput, &Processor::SetPauseInput);
}

Processor::~Processor() {
	delete[] threads;

	for (int i = 0; i < nThreads; ++i) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			delete (*iter)->module;
			delete (*iter)->inputResources;
			delete (*iter)->outputResources;
			delete *iter;
		}
	}
	delete[] modules;

	for (vector<OutputFilter*>::iterator iter = outputFilters.begin(); iter != outputFilters.end(); ++iter) {
		delete *iter;
	}
	delete inputQueue;

	while (deletedResources.size() > 0) {
		delete deletedResources.front();
		deletedResources.pop();
	}

	delete props;
}

char *Processor::GetPauseInput(const char *name) {
	ObjectUnlock();
	pauseInputCond.Lock();
	char *result = bool2str(pauseInput);
	pauseInputCond.Unlock();
	ObjectLockRead();		// so that Object can release ObjectLock
	return result;
}

void Processor::SetPauseInput(const char *name, const char *value) {
	ObjectUnlock();
	pauseInputCond.Lock();
	bool old = pauseInput;
	pauseInput = str2bool(value);
	if (old && !pauseInput)
		pauseInputCond.SignalSend();
	pauseInputCond.Unlock();
	ObjectLockWrite();		// so that Object can release ObjectLock
}

bool Processor::Init(Config *config) {
	// second stage?
	if (!config) {
		for (int i = 0; i < nThreads; ++i) {
			for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
				if (!(*iter)->module->Init(NULL))
					return false;
			}
		}
		return this->Connect();
	}

	char buffer[1024];
	char *s;
	vector<string> *v;

	// threads
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/threads", GetId());
	s = config->GetFirstValue(buffer);
	if (!s || sscanf(s, "%d", &nThreads) != 1 || nThreads <= 0 || nThreads > 100) {
		LOG_ERROR(this, "Invalid number of threads: " << s);
		return false;
	}
	free(s);

	modules = new vector<ModuleInfo*>[nThreads];

	// module(s)
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/modules/Module/@id", GetId());
	v = config->GetValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *mid = iter->c_str();
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/@lib", mid);
			char *name = config->GetFirstValue(buffer);
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/@type", mid);
			char *type = config->GetFirstValue(buffer);
			if (!type || !strcmp(type, "native")) {
				// C++ library module
				Module *(*create)(ObjectRegistry*, const char*, int) = (Module*(*)(ObjectRegistry*, const char*, int))LibraryLoader::LoadLibrary(name, "hector_module_create");
				if (!create) {
					LOG_ERROR(this, "Module/lib not found: " << name);
					return false;
				}
				for (int i = 0; i < nThreads; ++i) {
					ModuleInfo *mi = new ModuleInfo;
					mi->module = (*create)(objects, mid, i);
					modules[i].push_back(mi);
				}
				free(type);
				free(name);
			} else if (!strcmp(type, "perl")) {
				// Perl module
				for (int i = 0; i < nThreads; ++i) {
					ModuleInfo * mi = new ModuleInfo;
					mi->module = new PerlModule(objects, mid, i, name);
					modules[i].push_back(mi);
				}
				free(type);
				free(name);
			} else if (!strcmp(type, "python")) {
				// Python module
				for (int i = 0; i < nThreads; ++i) {
					ModuleInfo * mi = new ModuleInfo;
					mi->module = new PythonModule(objects, mid, i, name);
					modules[i].push_back(mi);
				}
				free(type);
				free(name);
			} else {
				LOG_ERROR(this, "Unknown module type (" << name << ", " << type << ")");
				return false;
			}
			// create name-value argument pairs
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param/@name", mid);
			vector<string> *names = config->GetValues(buffer);
			vector<pair<string, string> > *c = new vector<pair<string, string> >();
			string logLevel;
			for (int i = 0; names && i < (int)names->size(); i++) {
				snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param[%d]/@value", mid, i+1);
				char *val = config->GetFirstValue(buffer);
				if (!val) {
					snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param[%d]/text()", mid, i+1);
					val = config->GetFirstValue(buffer);
					if (!val) {
						LOG_ERROR(this, "No value for param: " << (*names)[i].c_str());
						return false;
					}
				}
				if ((*names)[i] == "logLevel")
					logLevel = val;
				else
					c->push_back(pair<string, string>((*names)[i], val));
				free(val);
			}
			delete names;

			for (int i = 0; i < nThreads; ++i) {
				ModuleInfo *mi = modules[i].back();
				if (!mi->module->Init(c))
					return false;
				if (logLevel.length() > 0)
					mi->module->SetLogLevel(logLevel.c_str());
				mi->type = mi->module->GetType();
				if (mi->type == Module::MULTI) {
					mi->processingResources = 0;
					mi->inputResources = new queue<Resource*>();
					mi->outputResources = new queue<Resource*>();
				} else {
					mi->inputResources = NULL;
					mi->outputResources = NULL;
				}
			}
			delete c;
		}
		delete v;
	}

	// input queue(s)
	inputQueue = new SyncQueue<Resource>();
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue", GetId());
	v = config->GetValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			// priority
			int priority = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@priority", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &priority) != 1) {
					LOG_ERROR(this, "Invalid priority: " << s);
					return false;
				}
				free(s);
			}
			// maxItems
			int maxItems = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@maxItems", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &maxItems) != 1) {
					LOG_ERROR(this, "Invalid maxItems: " << s);
					return false;
				}
				free(s);
			}
			// maxSize
			int maxSize = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@maxSize", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &maxSize) != 1) {
					LOG_ERROR(this, "Invalid maxSize: " << s);
					return false;
				}
				free(s);
			}
			inputQueue->addQueue(priority, maxItems, maxSize);

			// so that we can get actual size of a queue
			snprintf(buffer, sizeof(buffer), "queue_size.%d", priority);
			props->Add(buffer, &Processor::GetInputQueueItems);
		}
	}

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor", GetId());
	v = config->GetValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			OutputFilter *f = new OutputFilter();
			outputFilters.push_back(f);
			// reference
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@ref", GetId(), i+1);
			char *ref = config->GetFirstValue(buffer);
			if (!ref) {
				LOG_ERROR(this, "Missing reference: " << s);
				return false;
			}
			f->SetProcessor(ref);
			free(ref);
			// priority
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@priority", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				int priority;
				if (sscanf(s, "%d", &priority) != 1) {
					LOG_ERROR(this, "Invalid priority: " << s);
					return false;
				}
				f->SetPriority(priority);
				free(s);
			}
			// copy
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@copy", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				if (!strcmp(s, "1") || !strcasecmp(s, "true"))
					f->SetCopy(true);
				free(s);
			}
			// filter
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@filter", GetId(), i+1);
			s = config->GetFirstValue(buffer);
			if (s) {
				int filter;
				if (sscanf(s, "%d", &filter) != 1) {
					LOG_ERROR(this, "Invalid filter: " << s);
					return false;
				}
				f->SetFilter(filter);
				free(s);
			}
		}
	}

	// check modules
	if (nThreads > 0) {
		int n = modules[0].size();
		for (int i = 0; i < n; i++) {
			switch (modules[0][i]->type) {
			case Module::INPUT:
				if (i != 0) {
					LOG_ERROR(this, "Input module must be first");
					return false;
				}
				if (inputQueue->GetQueuesCount() > 0) {
					LOG_ERROR(this, "queue and input module must not be used together");
					return false;
				}
				break;
			case Module::OUTPUT:
				if (i != n-1) {
					LOG_ERROR(this, "Output module must be the last one in a Processor");
					return false;
				}
				if (outputFilters.size() > 0) {
					LOG_ERROR(this, "nextProcessor and output module must not be used together");
					return false;
				}
				break;
			case Module::SIMPLE:
			case Module::MULTI:
				if (i == 0 && inputQueue->GetQueuesCount() == 0) {
					LOG_ERROR(this, "No input queue defined");
					return false;
				} else if (i == n-1 && outputFilters.size() == 0) {
					LOG_ERROR(this, "No output queue defined");
					return false;
				}
				break;
			default:
				LOG_ERROR(this, "Should not happen");
				break;
			}
		}
	}

	return true;
}

// connect processors to other processors
bool Processor::Connect() {
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin(); iter != outputFilters.end(); ++iter) {
		int priority = (*iter)->GetPriority();
		const char *ref = (*iter)->GetProcessor();
		assert(ref != NULL);
		SyncQueue<Resource> *q;
		Processor *p = dynamic_cast<Processor*>(objects->GetObject(ref));
		if (p) {
			q = p->GetInputQueue();
		} else {
			ProcessingEngine *pe = dynamic_cast<ProcessingEngine*>(objects->GetObject(ref));
			if (!pe) {
				LOG_ERROR(this, "Processor or ProcessingEngine not found: " << ref);
				return false;
			}
			q = pe->GetOutputQueue();
			if (!q)
				q = pe->CreateOutputQueue();
		}

		if (!q) {
			LOG_ERROR(this, "No input queue defined for Processor/ProcessingEngine: " << ref);
			return false;
		}

		if (!q->hasQueue(priority)) {
			LOG_ERROR(this, "No input queue with priority " << priority << " for Processor/ProcessingEngine: " << ref);
			return false;
		}
		(*iter)->SetQueue(q);
	}

	// either PE's outputQueue or NULL
	engineOutputQueue = engine->GetOutputQueue();

	return true;
}

bool Processor::IsRunning() {
	bool result;
	ObjectLockRead();
	result = running;
	ObjectUnlock();
	return result;
}

struct thread {
	int id;
	Processor *p;
};

void *run_thread(void *ptr) {
	struct thread *t = (struct thread *)ptr;
	Processor *p = t->p;
	int id = t->id;
	delete t;
	p->RunThread(id);
	p->ThreadFinished();
	return NULL;
}

void Processor::ThreadFinished() {
	ObjectLockWrite();
	runningThreads--;
	bool finished = runningThreads == 0;
	ObjectUnlock();
	if (batch && finished)
		engine->ProcessorSleeping();
}

// returns: false if would sleep or cancelled
bool Processor::QueueResource(Resource *r, struct timeval *timeout, int *filterIndex) {
	int status = r->GetStatus();
	bool appended = false;
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin()+(filterIndex ? *filterIndex : 0); iter != outputFilters.end(); ++iter) {
		OutputFilter *f = *iter;
		if (f->IsEmptyFilter() || f->GetFilter() == status) {
			Resource *copy = (*iter)->GetCopy() ? r->Clone() : NULL;
			if (!f->ProcessResource(copy ? copy : r, timeout))
				return false;	// cancelled or no space available
			if (filterIndex)
				*filterIndex++;
			if (!copy) {
				appended = true;
				if (f->GetQueue() == engineOutputQueue)
					engine->UpdateResourceCount(-1);
				break;
			}
			engine->UpdateResourceCount(1);
		}
	}
	if (!appended) {
		LOG_ERROR(this, "Lost resource (id: " << r->GetId() << ")");
		Resource::GetRegistry()->ReleaseResource(r);
		engine->UpdateResourceCount(-1);
	}

	return true;
}

Resource *Processor::ApplyModules(vector<ModuleInfo*> *mis, Resource *resource, int index, bool sleep, bool *stop) {
	// process obtained resource through all non-multi modules
	while (index < (int)mis->size() && (*mis)[index]->type != Module::MULTI) {
		ModuleInfo *minfo = (*mis)[index];
		// apply modules
		switch (minfo->type) {
		case Module::INPUT:
			assert(resource == NULL);
			while (1) {
				resource = minfo->module->ProcessInput(sleep);
				if (resource)
					LOG_TRACE_R(minfo->module, resource, "I >")
				else
					LOG_TRACE(minfo->module, "I > 0");
				if (!resource || !resource->IsSetFlag(Resource::DELETED))
					break;				// OK: no more resources or resource was not deleted
				deletedResources.push(resource);	// deleted resource
			}
			if (!resource) {
				if (stop)
					*stop = true;
				return NULL;
			}
			engine->UpdateResourceCount(1);
			break;
		case Module::OUTPUT:
			LOG_TRACE_R(minfo->module, resource, "O <");
			resource = minfo->module->ProcessOutput(resource);
			Resource::GetRegistry()->ReleaseResource(resource);
			engine->UpdateResourceCount(-1);
			resource = NULL;
			break;
		case Module::SIMPLE:
			if (!resource->IsSetFlag(Resource::SKIP)) {
				LOG_TRACE_R(minfo->module, resource, "<");
				resource = minfo->module->ProcessSimple(resource);
				if (resource)
					LOG_TRACE_R(minfo->module, resource, ">")
				else
					LOG_TRACE(minfo->module, "> 0");
			}
			if (!resource) {
				LOG_ERROR(this, "Resource lost");
				return NULL;
			}
			if (resource->IsSetFlag(Resource::DELETED)) {
				// deleted resource
				deletedResources.push(resource);
				return NULL;
			}
			break;
		case Module::MULTI:
		default:
			LOG_ERROR(this, "Should not happen");
			break;
		}
		index++;
	}
	return resource;
}

int Processor::NextMultiModuleIndex(vector<ModuleInfo*> *mis, int index) {
	while (index < (int)mis->size() && (*mis)[index]->type != Module::MULTI)
		index++;
	return index == (int)mis->size() ? -1 : index;
}

void Processor::RunThread(int threadId) {
	ObjectLockWrite();
	runningThreads++;
	ObjectUnlock();
	vector<ModuleInfo*> *mis = &this->modules[threadId];
	// number of resources to read from input module/input queue
	int readMax = 1;
	// output filter (used when resource cannot be fully queued into the output queue)
	int outputFilterIndex = 0;
	// resource waiting to be queued-in
	Resource *outputFilterResource = NULL;
	// block while reading/writing resources from/to a queue?
	bool block = true;	
	struct timeval timeout = { 0, 0 };
	while (IsRunning()) {
		// process modules up-to first multi module
		int multiIndex = this->NextMultiModuleIndex(mis, 0);
		int resourcesRead = 0;
		while (resourcesRead < readMax) {
			Resource *resource = NULL;
			if ((*mis)[0]->type != Module::INPUT) {
				// get resource from the input queue
				if (batch && block && resourcesRead == 0)
					UpdateSleeping(1);
				resource = inputQueue->GetItem((block && resourcesRead == 0) ? &timeout : NULL);
				if (batch && block && resourcesRead == 0)
					UpdateSleeping(-1);
				if (!resource) {
					if (block && resourcesRead == 0)
						return; // cancelled
					else
						break; // no more resources available
				}
				if (inputQueue == engine->GetInputQueue())
					engine->UpdateResourceCount(1);
				resourcesRead++;
			} else {
				// no resources to be processed: wait until pauseInput is cleared
				pauseInputCond.Lock();
				if (pauseInput) {
					if (!block) {
						pauseInputCond.Unlock();
						break;	// some resources are still being processed
					}
					// we should wait until input is un-paused
					LOG_DEBUG(this, "Processor input pause");
					while (pauseInput) {
						pauseInputCond.WaitSend(NULL);
						pauseInputCond.Unlock();
						if (!IsRunning())
							return;	// cancelled
						pauseInputCond.Lock();
					}
					LOG_DEBUG(this, "Processor input continue");
				}
				pauseInputCond.Unlock();
			}
	
			bool stop = false;
			resource = this->ApplyModules(mis, resource, 0, block && resourcesRead == 0, &stop);
			if (resource) {
				if (multiIndex >= 0) {
					// next multi module
					(*mis)[multiIndex]->inputResources->push(resource);
				} else {
					// append to output queue (we do not have any multi modules => wait forever)
					if (!QueueResource(resource, &timeout, NULL))
						return;		// cancelled
					outputFilterIndex = 0;
				}
				resourcesRead++;
			} else {
				if (block && resourcesRead == 0 && stop)
					return;	// cancelled
				else
					break;	// no more resources available (or resource was deleted)
			}
		}
		// call multi-module, and subsequent (multi-)modules
		block = true;
		// n = number of resources the multi-module is still able to accept
		int minN = multiIndex >= 0 ? 1000000 : 1;
		bool queueFull = false;
		int resourcesQueued;
		while (multiIndex >= 0) {
			// call multi module
			ModuleInfo *minfo = (*mis)[multiIndex];
			int delta = minfo->inputResources->size()+minfo->outputResources->size()+minfo->processingResources;
			int n;
			LOG_TRACE(minfo->module, "< (" << minfo->inputResources->size() << ", " << minfo->outputResources->size() << ")");
			bool busy = minfo->module->ProcessMulti(minfo->inputResources, minfo->outputResources, &n, &minfo->processingResources);
			LOG_TRACE(minfo->module, "> (" << minfo->inputResources->size() << ", " << minfo->outputResources->size() << ")");
			n -= minfo->inputResources->size();
			if (n < minN)
				minN = n;
			if (busy)
				block = false;
			delta -= minfo->inputResources->size()+minfo->outputResources->size()+minfo->processingResources;
			// some resources were added
			if (delta != 0) {
				if (delta > 0) {
					LOG_ERROR(this, "Some resources were deleted in " << minfo->module->GetId() << "::ProcessMulti(), which is prohibited. Just mark them deleted.");
				}
				// resources must not be deleted in ProcessMulti(), just marked as deleted
				assert(delta < 0);
				engine->UpdateResourceCount(-delta);
			}
			// set-up info for the next iteration
			int nextMultiIndex = this->NextMultiModuleIndex(mis, multiIndex+1);

			resourcesQueued = 0;
			while (minfo->outputResources->size() > 0) {
				Resource *resource = minfo->outputResources->front();
				minfo->outputResources->pop();
				if (resource->IsSetFlag(Resource::DELETED)) {
					// deleted resource: do not process it any more
					deletedResources.push(resource);
					continue;
				}
				resource = this->ApplyModules(mis, resource, multiIndex+1, false, NULL);
				if (resource) {
					if (nextMultiIndex >= 0) {
						// next multi module
						if (!resource->IsSetFlag(Resource::SKIP))
							(*mis)[nextMultiIndex]->inputResources->push(resource);
						else
							(*mis)[nextMultiIndex]->outputResources->push(resource);
					} else {
						if (outputFilterResource) {
							if (!QueueResource(outputFilterResource, (block && resourcesQueued == 0) ? &timeout : NULL, &outputFilterIndex))
								break;	// cancelled or no space available
							outputFilterIndex = 0;
							resourcesQueued++;
							outputFilterResource = NULL;
						}
						if (!QueueResource(resource, (block && resourcesQueued == 0) ? &timeout : NULL, &outputFilterIndex)) {
							outputFilterResource = resource;
							break; // cancelled or no space available
						}
						resourcesQueued++;
					}
				}
			}
			if (minfo->outputResources->size() > 0) {
				if (block && resourcesQueued == 0)
					return; // cancelled
				// no more space available in the output queue
				queueFull = true;
				assert(multiIndex == -1); // we are putting into the output queue
			}
			multiIndex = nextMultiIndex;
		}
		// in next round read resources so that no multi-module is over-loaded
		readMax = minN;
		// with the exception that output queue is full: allow only
		// replacement of queued resources then
		if (queueFull && resourcesQueued < readMax)
			readMax = resourcesQueued;
		// another exception: if there are deleted resources still to
		// be appended to the engine's output queue, we do not accept
		// any more resources
		while (deletedResources.size() > 0) {
			if (engineOutputQueue) {
				if (!engineOutputQueue->PutItem(deletedResources.front(), NULL, 0)) {
					readMax = 0;
					break;
				}
				engine->UpdateResourceCount(-1);
			} else {
				// no output queue => no need to store anything, we just
				// discard the resource (and notify PE)
				LOG_TRACE_R(this, deletedResources.front(), "Deleted");
				Resource::GetRegistry()->ReleaseResource(deletedResources.front());
				engine->UpdateResourceCount(-1);
			}
			deletedResources.pop();
		}
	}
}

void Processor::Start() {
	ObjectLockWrite();
	running = true;
	threads = new pthread_t[nThreads];

	inputQueue->Start();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Start();
		}
	}

	for (int i = 0; i < nThreads; i++) {
		struct thread *t = new struct thread;
		t->p = this;
		t->id = i;
		pthread_create(&threads[i], NULL, run_thread, (void *)t);
	}
	LOG_INFO(this, "Processor started (" << nThreads << ")");
	ObjectUnlock();
}

void Processor::Stop() {
	ObjectLockWrite();
	running = false;

	pthread_t *copy = threads;
	threads = NULL;
	ObjectUnlock();

	// already stopped
	if (!copy)
		return;

	// cancel all threads waiting in input queues + multi module queues
	inputQueue->Stop();
	pauseInputCond.Lock();
	pauseInputCond.SignalSend();
	pauseInputCond.Unlock();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Stop();
		}
	}

	for (int i = 0; i < nThreads; i++) {
		pthread_join(copy[i], NULL);
	}

	delete[] copy;
	LOG_INFO(this, "Processor stopped (" << nThreads << ")");
}

void Processor::Pause() {
	ObjectLockWrite();
	inputQueue->Pause();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Pause();
		}
	}
	ObjectUnlock();
}

void Processor::Resume() {
	ObjectLockWrite();
	inputQueue->Resume();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Resume();
		}
	}
	ObjectUnlock();
}

bool Processor::SaveCheckpointSync(const char *path) {
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			if (!(*iter)->module->SaveCheckpoint(path))
				return false;
		}
	}
	return true;
}

bool Processor::RestoreCheckpointSync(const char *path) {
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			if (!(*iter)->module->RestoreCheckpoint(path))
				return false;
		}
	}
	return true;
}

char *Processor::GetInputQueueItems(const char *name) {
	// get queue priority first
	string n(name);
	size_t dot = n.find_last_of('.');
	if (dot == string::npos)
		return NULL;
	string s = n.substr(dot+1);
	int priority;
	if (sscanf(s.c_str(), "%d", &priority) != 1)
		return NULL;
	// get size of priority queue
	int queueItems = inputQueue->queueItems(priority);
	char s2[1024];
	snprintf(s2, sizeof(s2), "%d", queueItems);
	return strdup(s2);
}

inline void Processor::UpdateSleeping(int count) {
	assert(count != 0);
	bool reportSleeping = false;
	bool reportWakeup = false;
	ObjectLockWrite();
	sleepingThreads += count;
	if (sleeping) {
		if (sleepingThreads < runningThreads) {
			reportWakeup = true;
			sleeping = false;
		}
	} else {
		if (sleepingThreads == runningThreads) {
			reportSleeping = true;
			sleeping = true;
		}
	}
	ObjectUnlock();
	if (reportWakeup)
		engine->ProcessorWakeup();
	else if (reportSleeping)
		engine->ProcessorSleeping();
}
