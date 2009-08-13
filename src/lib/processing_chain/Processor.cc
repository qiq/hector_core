/**
 *
 */

#include <config.h>

#include <pthread.h>
#include <signal.h>
#include <ltdl.h>
#include "common.h"
#include "Processor.h"
#include "LibraryLoader.h"

log4cxx::LoggerPtr Processor::logger(log4cxx::Logger::getLogger("lib.processing_chain.Processor"));

Processor::Processor(ObjectRegistry *objects, const char *id): Object(objects, id) {
	threads = NULL;
	running = false;

	inputQueue = NULL;
	priorityInputQueue = NULL;
}

Processor::~Processor() {
	delete[] threads;

	for (vector<Module*>::iterator iter = modules.begin(); iter != modules.end(); ++iter) {
		delete *iter;
	}
	for (vector<OutputQueue*>::iterator iter = outputQueues.begin(); iter != outputQueues.end(); ++iter) {
		delete *iter;
	}
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

	// module(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/module/@ref", getId());
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
			Module *m = (*create)(objects, mid);
			if (!m->Init(config))
				return false;
			modules.push_back(m);
		}
		delete v;
	}

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue/@ref", getId());
	v = config->getValues(buffer);
	int i = 1;
	if (v) {
		if (v->size() > 1) {
			// one queue, no priority
			const char *qid = (*v)[0].c_str();
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue[%d]/@priority", qid, i);
			char *s = config->getFirstValue(buffer);
			if (s) {
				LOG4CXX_INFO(logger, "InputQueue priority ignored: " << qid);
				return false;
			}
			free(s);

			inputQueue = dynamic_cast<Queue*>(objects->getObject(qid));
			if (!inputQueue) {
				LOG4CXX_ERROR(logger, "Queue not found: " << qid);
				return false;
			}
		} else {
			// more sub-queues, should have distinct priority
			priorityInputQueue = new PrioritySyncQueue<Resource>();
			int n = v->size();
			int i = 1;
			for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
				const char *qid = iter->c_str();
				snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue[%d]/@priority", qid, i);
				char *s = config->getFirstValue(buffer);
				int priority = 0;
				if (s) {
					if (sscanf(s, "%d", &priority) != 1) {
						LOG4CXX_ERROR(logger, "Invalid inputQueue priority: " << s);
						return false;
					}
				}
				free(s);
				Queue *queue = dynamic_cast<Queue*>(objects->getObject(qid));
				if (!queue) {
					LOG4CXX_ERROR(logger, "Queue not found: " << qid);
					return false;
				}

				priorityInputQueue->addQueue(queue->getQueue(), priority, i == n);
				++i;
			}
		}
		delete v;
	}

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue/@ref", getId());
	v = config->getValues(buffer);
	i = 1;
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *qid = iter->c_str();
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue[%d]/@filter", qid, i);
			char *s = config->getFirstValue(buffer);
			int filter = 0;
			if (s) {
				if (sscanf(s, "%d", &filter) != 1) {
					LOG4CXX_ERROR(logger, "Invalid inputQueue priority: " << s);
					return false;
				}
			}
			free(s);
			Queue *queue = dynamic_cast<Queue*>(objects->getObject(qid));
			if (!queue) {
				LOG4CXX_ERROR(logger, "Queue not found: " << qid);
				return false;
			}

			OutputQueue *oq = new OutputQueue(queue, filter);
			outputQueues.push_back(oq);
			i++;
		}
		delete v;
	}

	// check modules
	int n = modules.size();
	i = 0;
	for (vector<Module*>::iterator iter = modules.begin(); iter != modules.end(); ++iter) {
		switch ((*iter)->getType()) {
		case MODULE_INPUT:
			if (i != 0) {
				LOG4CXX_ERROR(logger, "Input module must be first in a Processor: " << (*iter)->getId());
				return false;
			}
			if (inputQueue || priorityInputQueue) {
				LOG4CXX_ERROR(logger, "InputQueue and input module must not be used together: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_OUTPUT:
			if (i != n-1) {
				LOG4CXX_ERROR(logger, "Output module must be last in a Processor: " << (*iter)->getId());
				return false;
			}
			if (outputQueues.size() > 0) {
				LOG4CXX_ERROR(logger, "OutputQueue and output module must not be used together: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_MULTI:
		case MODULE_SELECT:
			if (i != 0 && n == 1) {
				LOG4CXX_ERROR(logger, "Multi/Select module must be the only in a Processor: " << (*iter)->getId());
				return false;
			}
			if ((!inputQueue && !priorityInputQueue) || outputQueues.size() == 0) {
				LOG4CXX_ERROR(logger, "No input or output queue defined: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_SIMPLE:
			if (i == 0 && !inputQueue && !priorityInputQueue) {
				LOG4CXX_ERROR(logger, "No input queue defined: " << (*iter)->getId());
				return false;
			} else if (i == n-1 && outputQueues.size() == 0) {
				LOG4CXX_ERROR(logger, "No output queue defined: " << (*iter)->getId());
				return false;
			}
		default:
			LOG4CXX_ERROR(logger, "Should not happen");
			break;
		}
		i++;
	}


	free(baseDir);

	return true;
}

bool Processor::Running() {
	bool result;
	runningLock.lock();
	result = running;
	runningLock.unlock();
	return result;
}

void *run_processor_thread(void *ptr) {
	Processor *p = (Processor *)ptr;
	p->runThread();
	return NULL;
}

void Processor::runThread() {
	module_t firstModuleType = modules.front()->getType();
	if (firstModuleType == MODULE_MULTI || firstModuleType == MODULE_SELECT) {
		// FIXME: use pipe to signal there are available 
		const int maxRequests = 100;	// FIXME: make it configurable
		Resource **inputResources = new Resource*[maxRequests+1];
		Resource **outputResources = new Resource*[maxRequests+1];
		int activeResources = 0;
		int finishedResources = 0;
		inputResources[0] = NULL;

		while (Running()) {
			// get up to maxRequests Resources items from the source queue, blocked in case we have no running requests
			int n = inputQueue ? inputQueue->getResources(inputResources, maxRequests-activeResources, activeResources == 0) : priorityInputQueue->getItems(inputResources, maxRequests-activeResources, activeResources == 0);
			inputResources[activeResources+n] = NULL;
			activeResources += n;

			// process new requests, get finished requests
			n = modules[0]->Process(inputResources, outputResources + finishedResources);
			finishedResources += n;
			inputResources[0] = NULL;
		
			// put requests into dstQueue, blocked in case we have no running requests
			// FIXME: upravit (rozdelit podle stavu a filtru do jednotilvych front
			//n = dstQueue->putItems(outputResources, finishedResources, activeResources == 0);
			// TODO: use cyclic buffer instead of copying
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
		Resource *resource;

		while (Running()) {
			if (firstModuleType != MODULE_INPUT)
				resource = inputQueue ? inputQueue->getResource(true) : priorityInputQueue->getItem(true);
			for (vector<Module*>::iterator iter = modules.begin(); iter != modules.end(); ++iter) {
				switch ((*iter)->getType()) {
				case MODULE_INPUT:
					resource = (*iter)->Process();
					assert(resource != NULL);
					break;
				case MODULE_OUTPUT:
				case MODULE_SIMPLE:
					(*iter)->Process(resource);
					break;
				case MODULE_MULTI:
				case MODULE_SELECT:
				default:
					LOG4CXX_ERROR(logger, "Should not happen");
					break;
				}
			}
			module_t lastModuleType = modules.front()->getType();
			if (lastModuleType != MODULE_OUTPUT)
				;
				//TODO dstQueue->putItem(resource, true);
		}
	}
}

void Processor::Start() {
	running = true;
	threads = new pthread_t[nThreads];

	for (int i = 0; i < nThreads; i++) {
		pthread_create(&threads[i], NULL, run_processor_thread, (void *)this);
	}
}

void Processor::Stop() {
	runningLock.lock();
	running = false;
	runningLock.unlock();
	for (int i = 0; i < nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
}

void Processor::createCheckpoint() {
	//TODO
}

char *Processor::getValue(const char *name) {
	return NULL;
}

bool Processor::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *Processor::listNames() {
	return new vector<string>();
}
