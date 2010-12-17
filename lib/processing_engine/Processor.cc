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
#include "ProcessingEngine.h"
#include "Processor.h"
#include "LibraryLoader.h"

using namespace std;

Processor::Processor(ObjectRegistry *objects, ProcessingEngine *engine, const char *id): Object(objects, id) {
	this->engine = engine;
	threads = NULL;
	running = false;
	pauseInput = false;

	values = new ObjectValues<Processor>(this);
	values->addGetter("pauseInput", &Processor::getPauseInput);
	values->addSetter("pauseInput", &Processor::setPauseInput);
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

	delete values;
}

char *Processor::getPauseInput(const char *name) {
	ObjectUnlock();
	pauseInputCond.Lock();
	return bool2str(pauseInput);
	pauseInputCond.Unlock();
	ObjectLockRead();		// so that Object can release ObjectLock
}

void Processor::setPauseInput(const char *name, const char *value) {
	ObjectUnlock();
	pauseInputCond.Lock();
	bool old = pauseInput;
	pauseInput = str2bool(value);
	if (old && !pauseInput)
		pauseInputCond.SignalSend();
	pauseInputCond.Unlock();
	ObjectLockRead();		// so that Object can release ObjectLock
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
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/threads", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &nThreads) != 1 || nThreads <= 0 || nThreads > 100) {
		LOG_ERROR(this, "Invalid number of threads: " << s);
		return false;
	}
	free(s);

	modules = new vector<ModuleInfo*>[nThreads];

	// module(s)
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/modules/Module/@id", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *mid = iter->c_str();
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/@lib", mid);
			char *name = config->getFirstValue(buffer);
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/@type", mid);
			char *type = config->getFirstValue(buffer);
			if (!type || !strcmp(type, "native")) {
				// C++ library module
				Module *(*create)(ObjectRegistry*, const char*, int) = (Module*(*)(ObjectRegistry*, const char*, int))LibraryLoader::loadLibrary(name, "create");
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
			} else {
				LOG_ERROR(this, "Unknown module type (" << name << ", " << type << ")");
				return false;
			}
			// create name-value argument pairs
			snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param/@name", mid);
			vector<string> *names = config->getValues(buffer);
			vector<pair<string, string> > *c = new vector<pair<string, string> >();
			for (int i = 0; names && i < names->size(); i++) {
				snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param[%d]/@value", mid, i+1);
				char *val = config->getFirstValue(buffer);
				if (!val) {
					snprintf(buffer, sizeof(buffer), "//Module[@id='%s']/param[%d]/text()", mid, i+1);
					val = config->getFirstValue(buffer);
					if (!val) {
						LOG_ERROR(this, "No value for param: " << (*names)[i].c_str());
						continue;
					}
				}
				c->push_back(pair<string, string>((*names)[i], val));
				free(val);
			}
			delete names;

			for (int i = 0; i < nThreads; ++i) {
				ModuleInfo *mi = modules[i].back();
				if (!mi->module->Init(c))
					return false;
				mi->type = mi->module->getType();
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
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue", getId());
	v = config->getValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			// priority
			int priority = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@priority", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &priority) != 1) {
					LOG_ERROR(this, "Invalid priority: " << s);
					return false;
				}
				free(s);
			}
			// maxItems
			int maxItems = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@maxItems", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &maxItems) != 1) {
					LOG_ERROR(this, "Invalid maxItems: " << s);
					return false;
				}
				free(s);
			}
			// maxSize
			int maxSize = 0;
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/input/queue[%d]/@maxSize", getId(), i+1);
			s = config->getFirstValue(buffer);
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
			values->addGetter(buffer, &Processor::getInputQueueItems);
		}
	}

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor", getId());
	v = config->getValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			OutputFilter *f = new OutputFilter();
			outputFilters.push_back(f);
			// reference
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@ref", getId(), i+1);
			char *ref = config->getFirstValue(buffer);
			if (!ref) {
				LOG_ERROR(this, "Missing reference: " << s);
				return false;
			}
			f->setProcessor(ref);
			free(ref);
			// priority
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@priority", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				int priority;
				if (sscanf(s, "%d", &priority) != 1) {
					LOG_ERROR(this, "Invalid priority: " << s);
					return false;
				}
				f->setPriority(priority);
				free(s);
			}
			// copy
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@copy", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (!strcmp(s, "1") || !strcasecmp(s, "true"))
					f->setCopy(true);
				free(s);
			}
			// filter
			snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/output/nextProcessor[%d]/@filter", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				int filter;
				if (sscanf(s, "%d", &filter) != 1) {
					LOG_ERROR(this, "Invalid filter: " << s);
					return false;
				}
				f->setFilter(filter);
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
				if (inputQueue->getQueuesCount() > 0) {
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
				if (i == 0 && inputQueue->getQueuesCount() == 0) {
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
		int priority = (*iter)->getPriority();
		const char *ref = (*iter)->getProcessor();
		assert(ref != NULL);
		SyncQueue<Resource> *q;
		Processor *p = dynamic_cast<Processor*>(objects->getObject(ref));
		if (p) {
			q = p->getInputQueue();
		} else {
			ProcessingEngine *pe = dynamic_cast<ProcessingEngine*>(objects->getObject(ref));
			if (!pe) {
				LOG_ERROR(this, "Processor or ProcessingEngine not found: " << ref);
				return false;
			}
			q = pe->getOutputQueue();
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
		(*iter)->setQueue(q);
	}

	// either PE's outputQueue or NULL
	engineOutputQueue = engine->getOutputQueue();

	return true;
}

bool Processor::isRunning() {
	bool result;
	ObjectLockWrite();
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
	p->runThread(id);
	return NULL;
}

// returns: false if would sleep or cancelled
bool Processor::QueueResource(Resource *r, struct timeval *timeout, int *filterIndex) {
	int status = r->getStatus();
	bool appended = false;
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin()+(filterIndex ? *filterIndex : 0); iter != outputFilters.end(); ++iter) {
		OutputFilter *f = *iter;
		if (f->isEmptyFilter() || f->getFilter() == status) {
			Resource *copy = (*iter)->getCopy() ? r->Clone() : NULL;
			if (!f->processResource(copy ? copy : r, timeout))
				return false;	// cancelled or no space available
			if (filterIndex)
				*filterIndex++;
			if (!copy) {
				appended = true;
				if (f->getQueue() == engineOutputQueue)
					engine->UpdateResourceCount(-1);
				break;
			}
			engine->UpdateResourceCount(1);
		}
	}
	if (!appended) {
		LOG_ERROR(this, "Lost resource (id: " << r->getId() << ")");
		Resource::ReleaseResource(r);
		engine->UpdateResourceCount(-1);
	}

	return true;
}

Resource *Processor::ApplyModules(vector<ModuleInfo*> *mis, Resource *resource, int index, bool sleep, bool *stop) {
	// process obtained resource through all non-multi modules
	while (index < mis->size() && (*mis)[index]->type != Module::MULTI) {
		ModuleInfo *minfo = (*mis)[index];
		// apply modules
		switch (minfo->type) {
		case Module::INPUT:
			assert(resource == NULL);
			while (1) {
				resource = minfo->module->ProcessInput(sleep);
				if (!resource || !resource->isSetFlag(Resource::DELETED))
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
			resource = minfo->module->ProcessOutput(resource);
			Resource::ReleaseResource(resource);
			engine->UpdateResourceCount(-1);
			resource = NULL;
			break;
		case Module::SIMPLE:
			if (!resource->isSetFlag(Resource::SKIP))
				resource = minfo->module->ProcessSimple(resource);
			if (!resource) {
				LOG_ERROR(this, "Resource lost");
				return NULL;
			}
			if (resource->isSetFlag(Resource::DELETED)) {
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
	while (index < mis->size() && (*mis)[index]->type != Module::MULTI)
		index++;
	return index == mis->size() ? -1 : index;
}

void Processor::runThread(int threadId) {
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
	while (isRunning()) {
		// process modules up-to first multi module
		int multiIndex = this->NextMultiModuleIndex(mis, 0);
		int resourcesRead = 0;
		while (resourcesRead < readMax) {
			Resource *resource = NULL;
			if ((*mis)[0]->type != Module::INPUT) {
				// get resource from the input queue
				resource = inputQueue->getItem((block && resourcesRead == 0) ? &timeout : NULL);
				if (!resource) {
					if (block && resourcesRead == 0)
						return; // cancelled
					else
						break; // no more resources available
				}
				if (inputQueue == engine->getInputQueue())
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
						if (!isRunning())
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
			minfo->processingResources = minfo->module->ProcessMulti(minfo->inputResources, minfo->outputResources, &n);
			n -= minfo->inputResources->size();
			if (n < minN)
				minN = n;
			if (minfo->processingResources > 0)
				block = false;
			delta -= minfo->inputResources->size()+minfo->outputResources->size()+minfo->processingResources;
			// some resources were added
			if (delta != 0) {
				assert(delta > 0);	// resources must not be deleted in ProcessMulti(), just marked as deleted
				engine->UpdateResourceCount(-delta);
			}
			// set-up info for the next iteration
			int nextMultiIndex = this->NextMultiModuleIndex(mis, multiIndex+1);

			resourcesQueued = 0;
			while (minfo->outputResources->size() > 0) {
				Resource *resource = minfo->outputResources->front();
				minfo->outputResources->pop();
				if (resource->isSetFlag(Resource::DELETED)) {
					// deleted resource: do not process it any more
					deletedResources.push(resource);
					continue;
				}
				resource = this->ApplyModules(mis, resource, multiIndex+1, false, NULL);
				if (resource) {
					if (nextMultiIndex >= 0) {
						// next multi module
						if (!resource->isSetFlag(Resource::SKIP))
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
				if (!engineOutputQueue->putItem(deletedResources.front(), NULL, 0)) {
					readMax = 0;
					break;
				}
				engine->UpdateResourceCount(-1);
			} else {
				// no output queue => no need to store anything, we just
				// discard the resource (and notify PE)
				LOG_DEBUG_R(this, deletedResources.front(), "Deleted");
				Resource::ReleaseResource(deletedResources.front());
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
	inputQueue->Pause();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Pause();
		}
	}
}

void Processor::Resume() {
	inputQueue->Resume();
	for (int i = 0; i < nThreads; i++) {
		for (vector<ModuleInfo*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->module->Resume();
		}
	}
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

char *Processor::getInputQueueItems(const char *name) {
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

