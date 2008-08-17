/**
 *
 */

#include <ltdl.h>
//#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>
#include "Processor.h"

log4cxx::LoggerPtr Processor::logger(log4cxx::Logger::getLogger("robot.Processor"));

Processor::Processor() {
	running = false;
}

Processor::~Processor() {
}

bool Processor::Running() {
	bool result;
	running_lock.lock();
	result = running;
	running_lock.unlock();
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
	running_lock.lock();
	running = false;
	running_lock.unlock();
	for (int i = 0; i < nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
}

void *Processor::loadLibrary(const char *lib, const char *sym) {
	if (lt_dlinit() != 0)
		LOG4CXX_ERROR(logger, "Cannot initialize ltdl");
	lt_dlhandle handle = lt_dlopen(lib);
	if (handle == NULL) {
		LOG4CXX_ERROR(logger, "Cannot load library " << lib << " (" << lt_dlerror() << ")");
		return NULL;
	}
	void *p = lt_dlsym(handle, sym);
	return p;
}

