/**
 *
 */

#include <ltdl.h>
#include <pthread.h>
#include "Processor.h"

Processor::Processor() {
	this->running = false;
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
	running = true;

	for (int i = 0; i < nThreads; i++) {
		pthread_create(&threads[i], NULL, run_processor_thread, (void *)this);
	}
}

void Processor::Stop() {
	this->running = false;
}

void *Processor::loadLibrary(const char *lib, const char *sym) {
	lt_dlhandle handle = lt_dlopen(lib);
	if (handle == NULL) {
		// TODO: log(lt_dlerror());
		return NULL;
	}
	void *p = lt_dlsym(handle, sym);
	return p;
}

