/**
 *
 */

#include <config.h>

#include <pthread.h>
#include <signal.h>
#include <ltdl.h>
#include "common.h"
#include "Processor.h"
#include "Server.h"

log4cxx::LoggerPtr Processor::logger(log4cxx::Logger::getLogger("lib.processing_chain.Processor"));

Processor::Processor() {
	threads = NULL;
	running = false;
}

Processor::~Processor() {
	delete[] threads;
}

//	<Processor id="load-resource">
//		<type>simple</type>
//		<threads>1</threads>
//		<module ref="load-resource-module"/>
//		<module ref="preprocess-resource-module"/>
//		<outputQueue ref="q1"/>
//	</Processor>

bool Processor::Init(Server *server, Config *config, const char *id) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	this->server = server;

	char *baseDir = config->getFirstValue("/Config/@baseDir");
	if (!baseDir) {
		LOG4CXX_ERROR(logger, "Cannot find baseDir");
		return false;
	}

	// type
	//snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/type", id);
	//s = config->getFirstValue(buffer);
	//if (!strcmp(s, "simple")) {
	//	type = simple;
	//} else if (!strcmp(s, "multi")) {
	//	type = multi;
	//} else if (!strcmp(s, "select"))
	//	type = select;
	//} else {
	//	LOG4CXX_ERROR(logger, "Invalid Processor type: " << s);
	//	return false;
	//}
	//free(s);

	// threads
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/threads", id);
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &nThreads) != 1) {
		LOG4CXX_ERROR(logger, "Invalid number of threads: " << s);
		return false;
	}
	free(s);

	// module(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/module/@ref", id);
	v = config->getValues(buffer);
	for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
		const char *mid = iter->c_str();
		snprintf(buffer, sizeof(buffer), "/Config/Module[@id='%s']/lib/@name", mid);
		s = config->getFirstValue(buffer);
		if (!s) {
			LOG4CXX_ERROR(logger, "Module/lib not found: " << mid);
			return false;
		}
		snprintf(buffer, sizeof(buffer), "%s/%s", baseDir, s);
		Module *(*create)() = (Module*(*)())loadLibrary(buffer, "create");
		Module *m = (*create)();
		if (!m->Init(server, config, mid))
			return false;
		modules.push_back(m);
		server->registerObject(m);
	}

	// check modules
	int n = modules.size();
	int i = 0;
	for (vector<Module*>::iterator iter = modules.begin(); iter != modules.end(); iter++) {
		switch ((*iter)->getType()) {
		case MODULE_INPUT:
			if (i != 0) {
				LOG4CXX_ERROR(logger, "Input module must be first in a Processor: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_OUTPUT:
			if (i != n-1) {
				LOG4CXX_ERROR(logger, "Output module must be last in a Processor: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_MULTI:
		case MODULE_SELECT:
			if (i != 0 && n == 1) {
				LOG4CXX_ERROR(logger, "Multi/Select module must be the only in a Processor: " << (*iter)->getId());
				return false;
			}
			break;
		case MODULE_SIMPLE:
		default:
			break;
		}
		i++;
	}

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue/@ref", id);
	v = config->getValues(buffer);
	i = 1;
	for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
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
		Queue *queue = dynamic_cast<Queue*>(server->getObject(qid));
		if (!queue) {
			LOG4CXX_ERROR(logger, "Queue not found: " << qid);
			return false;
		}

		InputQueue *iq = new InputQueue(queue, priority);
		inputQueues.push_back(iq);
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
	Processor *module = (Processor *)ptr;
	module->runThread();
	return NULL;
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
