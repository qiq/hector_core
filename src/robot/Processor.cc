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
}

Processor::~Processor() {
}

void *run_processor_thread(void *ptr) {
	pthread_detach(pthread_self());
	Processor *module = (Processor *)ptr;
	module->runThread();
	return NULL;
}

void Processor::Start() {
	threads = new pthread_t[nThreads];

	for (int i = 0; i < nThreads; i++) {
		pthread_create(&threads[i], NULL, run_processor_thread, (void *)this);
	}
}

void Processor::Stop() {
	for (int i = 0; i < nThreads; i++) {
		pthread_kill(threads[i], 9);
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
//	void *handle = dlopen(lib, RTLD_NOW);
//	if (handle == NULL) {
//		fprintf(stderr, "... %s\n", dlerror());
//		LOG4CXX_ERROR(logger, "Cannot load library");
//		return NULL;
//	}
//	void *p = dlsym(handle, sym);
//	return p;
}

