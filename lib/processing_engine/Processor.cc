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
#include "PerlModule.h"
#include "Processor.h"
#include "LibraryLoader.h"

using namespace std;

Processor::Processor(ObjectRegistry *objects, const char *id): Object(objects, id) {
	threads = NULL;
	running = false;

	values = new ObjectValues<Processor>(this);
}

Processor::~Processor() {
	delete[] threads;

	for (int i = 0; i < nThreads; ++i) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			delete *iter;
		}
	}
	delete[] modules;

	for (vector<OutputFilter*>::iterator iter = outputFilters.begin(); iter != outputFilters.end(); ++iter) {
		delete *iter;
	}
	delete inputQueue;

	delete values;
}

bool Processor::Init(Config *config) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	// threads
	snprintf(buffer, sizeof(buffer), "//Processor[@id='%s']/threads", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &nThreads) != 1 || nThreads <= 0 || nThreads > 100) {
		LOG_ERROR("Invalid number of threads: " << s);
		return false;
	}
	free(s);

	modules = new vector<Module*>[nThreads];

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
					LOG_ERROR("Module/lib not found: " << name);
					return false;
				}
				for (int i = 0; i < nThreads; ++i) {
					Module *m = (*create)(objects, mid, i);
					modules[i].push_back(m);
				}
				free(type);
				free(name);
			} else if (!strcmp(type, "perl")) {
				// Perl module
				for (int i = 0; i < nThreads; ++i) {
					Module *m = new PerlModule(objects, mid, i, name);
					modules[i].push_back(m);
				}
				free(type);
				free(name);
			} else {
				LOG_ERROR("Unknown module type (" << name << ", " << type << ")");
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
						LOG_ERROR("No value for param: " << (*names)[i].c_str());
						continue;
					}
				}
				c->push_back(pair<string, string>((*names)[i], val));
				free(val);
			}
			delete names;

			for (int i = 0; i < nThreads; ++i) {
				if (!modules[i].back()->Init(c))
					return false;
			}
			delete c;

			moduleType.push_back(modules[0].back()->getType());
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
					LOG_ERROR("Invalid priority: " << s);
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
					LOG_ERROR("Invalid maxItems: " << s);
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
					LOG_ERROR("Invalid maxSize: " << s);
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
				LOG_ERROR("Missing reference: " << s);
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
					LOG_ERROR("Invalid priority: " << s);
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
					LOG_ERROR("Invalid filter: " << s);
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
			switch (moduleType[i]) {
			case Module::INPUT:
				if (i != 0) {
					LOG_ERROR("Input module must be first");
					return false;
				}
				if (inputQueue->getQueuesCount() > 0) {
					LOG_ERROR("queue and input module must not be used together");
					return false;
				}
				break;
			case Module::OUTPUT:
				if (i != n-1) {
					LOG_ERROR("Output module must be the last one in a Processor");
					return false;
				}
				if (outputFilters.size() > 0) {
					LOG_ERROR("nextProcessor and output module must not be used together");
					return false;
				}
				break;
			case Module::MULTI:
				if (i != 0 && n == 1) {
					LOG_ERROR("Multi/Select module must be the only one in a Processor");
					return false;
				}
				if (inputQueue->getQueuesCount() == 0 || outputFilters.size() == 0) {
					LOG_ERROR( "No input or output queue defined");
					return false;
				}
				break;
			case Module::SIMPLE:
				if (i == 0 && inputQueue->getQueuesCount() == 0) {
					LOG_ERROR("No input queue defined");
					return false;
				} else if (i == n-1 && outputFilters.size() == 0) {
					LOG_ERROR("No output queue defined");
					return false;
				}
				break;
			default:
				LOG_ERROR("Should not happen");
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
		Processor *p = dynamic_cast<Processor*>(objects->getObject(ref));
		if (!p) {
			LOG_ERROR("Processor not found: " << ref);
			return false;
		}

		SyncQueue<Resource> *q = p->getInputQueue();
		if (!q) {
			LOG_ERROR("No input queue defined for processor: " << ref);
			return false;
		}

		if (!q->hasQueue(priority)) {
			LOG_ERROR("No input queue with priority " << priority << " for processor: " << ref);
			return false;
		}
		(*iter)->setQueue(q);
	}
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

// returns: filterIndex or -1 (sleep and cancelled)
bool Processor::appendResource(Resource *r, bool sleep, int *filterIndex) {
	int status = r->getStatus();
	bool appended = false;
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin()+*filterIndex; iter != outputFilters.end(); ++iter) {
		OutputFilter *f = *iter;
		if (f->isEmptyFilter() || f->getFilter() == status) {
			Resource *copy = (*iter)->getCopy() ? r->Clone() : NULL;
			if (!f->processResource(r, sleep))
				return false;	// cancelled or no space available
			*filterIndex++;
			if (!copy) {
				appended = true;
				break;
			}
			r = copy;
		}
	}
	if (!appended) {
		LOG_ERROR("Lost resource (id: " << r->getId() << ")");
		delete r;
	}

	return true;
}

void Processor::runThread(int id) {
	if (moduleType[0] != Module::MULTI) {
		// simple processing (no parallel or select)
		Resource *resource = NULL;
		while (isRunning()) {
			if (moduleType[0] != Module::INPUT) {
				if (!(resource = inputQueue->getItem(true)))
					break;	// cancelled
			}

			bool stop = false;
			bool next = false;
			for (int i = 0; !stop && !next && i < modules[id].size(); i++) {
				switch (moduleType[i]) {
				case Module::INPUT:
					resource = modules[id][i]->Process(NULL);
					if (!resource)
						stop = true;
					break;
				case Module::OUTPUT:
					(void)modules[id][i]->Process(resource);
					resource = NULL;
					break;
				case Module::SIMPLE:
					resource = modules[id][i]->Process(resource);
					if (!resource)
						next = true;
					break;
				case Module::MULTI:
				default:
					LOG_ERROR("Should not happen");
					break;
				}
			}
			if (stop) {
				LOG_ERROR("No resource, thread " << id << " terminated");
				break;
			}
			if (resource) {
				int filterIndex = 0;
				if (!appendResource(resource, true, &filterIndex))
					break;	// cancelled
				resource = NULL;
			}
		}
		delete resource;
	} else {
		// parallel processing: read items from input queue, process them and put items into output queue
		queue<Resource*> *inputResources = new queue<Resource*>();
		queue<Resource*> *outputResources = new queue<Resource*>();
		int outputFilterIndex = 0;

		Module *module = modules[id].front();
		int n;
		while (isRunning()) {
			n = module->ProcessMulti(inputResources, outputResources);
			// n >= 0: at least one resource is being processed, do not block
			// n < 0: no resources are being processed, wait for input resources
			// m = number of output resources appended to output queue (may be less than outputResources->size())

			// n >= 0: get upto min(n, m) input resources, but do not wait for them
			// n < 0: get upto min(-n, m) input resources, wait for at least one input resource, if output queue is full, wait for it to be ready

			// append queue resources to output queue, m is number of resources appended
			bool block = n < 0;
			n = block ? -n : n;
			int m = 0;
			while (outputResources->size() > 0) {
				if (!appendResource(outputResources->front(), block && m == 0, &outputFilterIndex))
					break;	// cancelled or no space available
				outputResources->pop();
				m++;
			}
			int readMax = n;
			if (outputResources->size() > 0) {
				if (block && m == 0)
					break;	// cancelled
				// output queue is full: only allow to read m resources from the input queue
				readMax = m;
			}

			// get upto readMax resources from the input queue	
			readMax -= inputResources->size();
			int i = 0;
			while (i < readMax) {
				Resource *resource = inputQueue->getItem(block && i == 0);
				if (!resource)
					break; // cancelled or no resources
				inputResources->push(resource);
				i++;
			}
			if (block && i == 0)
				break;	// cancelled
		}
	}
}

void Processor::Start() {
	ObjectLockWrite();
	running = true;
	threads = new pthread_t[nThreads];

	inputQueue->Start();
	for (int i = 0; i < nThreads; i++) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->Start();
		}
	}

	for (int i = 0; i < nThreads; i++) {
		struct thread *t = new struct thread;
		t->p = this;
		t->id = i;
		pthread_create(&threads[i], NULL, run_thread, (void *)t);
	}
	LOG_INFO("Processor started (" << nThreads << ")");
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
	for (int i = 0; i < nThreads; i++) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->Stop();
		}
	}

	for (int i = 0; i < nThreads; i++) {
		pthread_join(copy[i], NULL);
	}

	delete[] copy;
	LOG_INFO("Processor stopped (" << nThreads << ")");
}

void Processor::Pause() {
	inputQueue->Pause();
	for (int i = 0; i < nThreads; i++) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->Pause();
		}
	}
}

void Processor::Resume() {
	inputQueue->Resume();
	for (int i = 0; i < nThreads; i++) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			(*iter)->Resume();
		}
	}
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

