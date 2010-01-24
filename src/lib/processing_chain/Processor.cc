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
#include "Processor.h"
#include "LibraryLoader.h"

log4cxx::LoggerPtr Processor::logger(log4cxx::Logger::getLogger("lib.processing_chain.Processor"));

Processor::Processor(ObjectRegistry *objects, const char *id): Object(objects, id) {
	threads = NULL;
	running = false;
}

Processor::~Processor() {
	delete[] threads;

	for (int i = 0; i < nThreads; ++i) {
		for (vector<Module*>::iterator iter = modules[i].begin(); iter != modules[i].end(); ++iter) {
			delete *iter;
		}
	}
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin(); iter != outputFilters.end(); ++iter) {
		delete *iter;
	}
	delete queue;
}

bool Processor::Init(Config *config) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	char *baseDir = config->getFirstValue("/Config/@baseDir");
	if (!baseDir) {
		LOG4CXX_ERROR(logger, "Cannot find baseDir");
		return false;
	}

	// threads
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/threads", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &nThreads) != 1) {
		LOG4CXX_ERROR(logger, "Invalid number of threads: " << s);
		return false;
	}
	free(s);

	modules = new vector<Module*>[nThreads];

	// module(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/modules/module/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *mid = iter->c_str();
			snprintf(buffer, sizeof(buffer), "/Config/Module[@id='%s']/lib/@name", mid);
			s = config->getFirstValue(buffer);
			if (!s) {
				LOG4CXX_ERROR(logger, "Module/lib not found: " << mid);
				return false;
			}
			snprintf(buffer, sizeof(buffer), "%s/%s", baseDir, s);
			free(s);
			Module *(*create)(ObjectRegistry*, const char*) = (Module*(*)(ObjectRegistry*, const char*))LibraryLoader::loadLibrary(buffer, "create");
			if (!create) {
				LOG4CXX_ERROR(logger, "Module/lib not found: " << buffer);
				return false;
			}
			// create name-value argument pairs
			snprintf(buffer, sizeof(buffer), "/Config/Module[@id='%s']/param/@name", getId());
			vector<string> *names = config->getValues(buffer);
			snprintf(buffer, sizeof(buffer), "/Config/Module[@id='%s']/param/@value", getId());
			vector<string> *values = config->getValues(buffer);
			assert(names->size() == values->size());
			vector<pair<string, string> > *c = new vector<pair<string, string> >();
			for (int i = 0; i < names->size(); i++) {
				c->push_back(pair<string, string>((*names)[i], (*values)[i]));
			}
			delete values;
			delete names;

			for (int i = 0; i < nThreads; ++i) {
				Module *m = (*create)(objects, mid);
				if (!m->Init(c))
					return false;
				modules[i].push_back(m);
			}
			delete c;
		}
		delete v;
	}

	// input queue(s)
	queue = new SyncQueue<Resource>();
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/input/queue", getId());
	v = config->getValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			// priority
			int priority = 0;
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/input/queue[%d]/@priority", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &priority) != 1) {
					LOG4CXX_ERROR(logger, "Invalid priority: " << s);
					return false;
				}
				free(s);
			}
			// maxItems
			int maxItems = 0;
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/input/queue[%d]/@maxItems", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &maxItems) != 1) {
					LOG4CXX_ERROR(logger, "Invalid maxItems: " << s);
					return false;
				}
				free(s);
			}
			// maxSize
			int maxSize = 0;
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/input/queue[%d]/@maxSize", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (sscanf(s, "%d", &maxSize) != 1) {
					LOG4CXX_ERROR(logger, "Invalid maxSize: " << s);
					return false;
				}
				free(s);
			}
			queue->addQueue(priority, maxItems, maxSize);

			// so that we can get actual size of a queue
			snprintf(buffer, sizeof(buffer), "queue_size.%d", priority);
			getters[buffer] = &Processor::getQueueItems;
		}
	}

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/output/nextProcessor", getId());
	v = config->getValues(buffer);
	if (v) {
		int n = v->size();
		delete v;
		for (int i = 0; i < n; i++) {
			OutputFilter *f = new OutputFilter();
			outputFilters.push_back(f);
			// reference
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/output/nextProcessor[%d]/@ref", getId(), i+1);
			char *ref = config->getFirstValue(buffer);
			if (!ref) {
				LOG4CXX_ERROR(logger, "Missing reference: " << s);
				return false;
			}
			f->setProcessor(ref);
			free(ref);
			// priority
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/output/nextProcessor[%d]/@priority", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				int priority;
				if (sscanf(s, "%d", &priority) != 1) {
					LOG4CXX_ERROR(logger, "Invalid priority: " << s);
					return false;
				}
				f->setPriority(priority);
				free(s);
			}
			// copy
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/output/nextProcessor[%d]/@copy", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				if (!strcmp(s, "1") || !strcasecmp(s, "true"))
					f->setCopy(true);
				free(s);
			}
			// filter
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/input/queue[%d]/@filter", getId(), i+1);
			s = config->getFirstValue(buffer);
			if (s) {
				int filter;
				if (sscanf(s, "%d", &filter) != 1) {
					LOG4CXX_ERROR(logger, "Invalid filter: " << s);
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
		int i = 0;
		for (vector<Module*>::iterator iter = modules[0].begin(); iter != modules[0].end(); ++iter) {
			switch ((*iter)->getType()) {
			case MODULE_INPUT:
				if (i != 0) {
					LOG4CXX_ERROR(logger, "Input module must be first in a Processor: " << getId());
					return false;
				}
				if (queue->getQueuesCount() > 0) {
					LOG4CXX_ERROR(logger, "queue and input module must not be used together: " << getId());
					return false;
				}
				break;
			case MODULE_OUTPUT:
				if (i != n-1) {
					LOG4CXX_ERROR(logger, "Output module must be last in a Processor: " << getId());
					return false;
				}
				if (outputFilters.size() > 0) {
					LOG4CXX_ERROR(logger, "nextProcessor and output module must not be used together: " << getId());
					return false;
				}
				break;
			case MODULE_MULTI:
			case MODULE_SELECT:
				if (i != 0 && n == 1) {
					LOG4CXX_ERROR(logger, "Multi/Select module must be the only one in a Processor: " << getId());
					return false;
				}
				if (queue->getQueuesCount() == 0 || outputFilters.size() == 0) {
					LOG4CXX_ERROR(logger, "No input or output queue defined: " << getId());
					return false;
				}
				break;
			case MODULE_SIMPLE:
				if (i == 0 && queue->getQueuesCount() == 0) {
					LOG4CXX_ERROR(logger, "No input queue defined: " << getId());
					return false;
				} else if (i == n-1 && outputFilters.size() == 0) {
					LOG4CXX_ERROR(logger, "No output queue defined: " << getId());
					return false;
				}
				break;
			default:
				LOG4CXX_ERROR(logger, "Should not happen");
				break;
			}
			i++;
		}
	}

	free(baseDir);

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
			LOG4CXX_ERROR(logger, "Processor not found: " << ref);
			return false;
		}

		SyncQueue<Resource> *q = p->getQueue();
		if (!q) {
			LOG4CXX_ERROR(logger, "No input queue defined for processor: " << ref);
			return false;
		}

		if (!q->hasQueue(priority)) {
			LOG4CXX_ERROR(logger, "No input queue with priority " << priority << " for processor: " << ref);
			return false;
		}
		(*iter)->setQueue(q);
	}
	return true;
}

bool Processor::Running() {
	bool result;
	runningLock.lock();
	result = running;
	runningLock.unlock();
	return result;
}

struct thread {
	int id;
	Processor *p;
};

void *run_processor_thread(void *ptr) {
	struct thread *t = (struct thread *)ptr;
	Processor *p = t->p;
	int id = t->id;
	delete t;
	p->runThread(id);
	return NULL;
}

bool Processor::appendResource(Resource *r, bool sleep) {
	int status = r->getStatus();
	bool copied = false;
	bool appended = false;
	for (vector<OutputFilter*>::iterator iter = outputFilters.begin(); iter != outputFilters.end(); ++iter) {
		OutputFilter *f = *iter;
		if (f->isEmptyFilter() || f->getFilter() == status) {
			if (!f->processResource(r, sleep || copied))
				return false;
			if ((*iter)->getCopy()) {
				copied = true;
				continue;
			}
			appended = true;
			break;
		}
	}
	if (!appended)
		LOG4CXX_WARN(logger, "Lost resource (id: " /* FIXME << r->getId()*/ << ") in processor " << getId());

	return (copied||appended);
}

void Processor::runThread(int id) {
	module_t firstModuleType = modules[id].front()->getType();
	if (firstModuleType == MODULE_MULTI || firstModuleType == MODULE_SELECT) {
		const int maxRequests = 100;	// FIXME: make it configurable
		Resource **inputResources = new Resource*[maxRequests+1];
		Resource **outputResources = new Resource*[maxRequests+1];
		int activeResources = 0;
		int finishedResources = 0;
		inputResources[0] = NULL;

		while (Running()) {
			// get up to maxRequests Resources items from the source queue, blocked in case we have no running requests
			int n = queue->getItems(inputResources, maxRequests-activeResources, activeResources == 0);
			if (n == 0 && activeResources == 0)
				break;	// cancelled
			inputResources[activeResources+n] = NULL;
			activeResources += n;

			// process new requests, get finished requests
			n = modules[id][0]->Process(inputResources, outputResources + finishedResources);
			finishedResources += n;
			inputResources[0] = NULL;

			// pass processed Resources into correct queues
			n = 0;
			while (n < finishedResources) {
				if (!appendResource(outputResources[n], activeResources == maxRequests))
					break;
				n++;
			}
			if (n == 0 && activeResources == maxRequests)
				break;	// cancelled
			// TODO: maybe use cyclic buffer instead of copying?
			for (int i = 0; i < finishedResources-n; i++) {
				outputResources[i] = outputResources[n+i];
			}
			finishedResources -= n;
			activeResources -= n;
		}

		for (int i = 0; inputResources[i] != NULL; i++) {
			delete inputResources[i];
		}
		delete[] inputResources;
		for (int i = 0; i < finishedResources; i++) {
			delete outputResources[i];
		}
		delete[] outputResources;
	} else {
		// simple processing (no parallel or select)
		Resource *resource = NULL;

		while (Running()) {
			if (firstModuleType != MODULE_INPUT) {
				if (!(resource = queue->getItem(true)))
					break;	// cancelled
			}

			for (vector<Module*>::iterator iter = modules[id].begin(); iter != modules[id].end(); ++iter) {
				switch ((*iter)->getType()) {
				case MODULE_INPUT:
					resource = (*iter)->Process(NULL);
					assert(resource != NULL);
					break;
				case MODULE_OUTPUT:
					(void)(*iter)->Process(resource);
					resource = NULL;
					break;
				case MODULE_SIMPLE:
					resource = (*iter)->Process(resource);
					assert(resource != NULL);
					break;
				case MODULE_MULTI:
				case MODULE_SELECT:
				default:
					LOG4CXX_ERROR(logger, "Should not happen");
					break;
				}
			}
			if (resource != NULL) {
				if (!appendResource(resource, true))
					break;	// cancelled
				resource = NULL;
			}
		}
		delete resource;
	}
}

void Processor::start() {
	running = true;
	threads = new pthread_t[nThreads];

	queue->clearCancel();

	for (int i = 0; i < nThreads; i++) {
		struct thread *t = new struct thread;
		t->p = this;
		t->id = i;
		pthread_create(&threads[i], NULL, run_processor_thread, (void *)t);
	}
}

void Processor::stop() {
	runningLock.lock();
	running = false;
	runningLock.unlock();

	queue->cancelAll();

	for (int i = 0; i < nThreads; i++) {
		pthread_join(threads[i], NULL);
	}

	delete[] threads;
	threads = NULL;
}

void Processor::pause() {
	queue->pause();
}

void Processor::resume() {
	queue->resume();
}

char *Processor::getValue(const char *name) {
	char *result = NULL;
	std::tr1::unordered_map<string, char*(Processor::*)(const char*)>::iterator iter = getters.find(name);
	if (iter != getters.end())
		result = (this->*(iter->second))(name);
	return result;
}

bool Processor::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *Processor::listNames() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(Processor::*)(const char*)>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

char *Processor::getQueueItems(const char *name) {
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
	int queueItems = queue->queueItems(priority);
	char s2[1024];
	snprintf(s2, sizeof(s2), "%d", queueItems);
	return strdup(s2);
}

