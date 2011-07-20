/**
 * Loads libraries, keeps track of loaded libraries, so that they are correctly
 * closed on destruction.
 */

#include <config.h>

#include <stdlib.h>
#include "LibraryLoader.h"

using namespace std;

log4cxx::LoggerPtr LibraryLoader::logger(log4cxx::Logger::getLogger("lib.LibraryLoader"));

PlainLock LibraryLoader::lock;
bool LibraryLoader::initialized;
tr1::unordered_map<string, lt_dlhandle*> LibraryLoader::handles;

LibraryLoader ll;

LibraryLoader::LibraryLoader() {
	initialized = false;
}

LibraryLoader::~LibraryLoader() {
	bool close = !getenv("HECTOR_NO_UNLOAD");
	for (tr1::unordered_map<string, lt_dlhandle*>::iterator iter = handles.begin(); iter != handles.end(); ++iter) {
		if (close)
			lt_dlclose(*iter->second);
		delete iter->second;
	}
	lt_dlexit();
	initialized = false;
	lock.Unlock();
}

void *LibraryLoader::LoadLibrary(const char *lib, const char *sym, bool reportErrors) {
	lock.Lock();
	if (!initialized) {
		if (lt_dlinit() != 0) {
			lock.Unlock();
			if (reportErrors)
				LOG4CXX_ERROR(logger, "Cannot initialize libtool: " << lt_dlerror());
			return NULL;
		}
		initialized = true;
	}
	tr1::unordered_map<string, lt_dlhandle*>::iterator iter = handles.find(lib);
	lt_dlhandle *handle;
	if (iter != handles.end()) {
		handle = iter->second;
	} else {
		handle = new lt_dlhandle;
		*handle = lt_dlopen(lib);
		if (*handle == NULL) {
			lock.Unlock();
			delete handle;
			if (reportErrors)
				LOG4CXX_ERROR(logger, "Cannot load library: " << lt_dlerror());
			return NULL;
		}
		handles[lib] = handle;
	}
	void *p = lt_dlsym(*handle, sym);
	lock.Unlock();
	if (!p && reportErrors)
		LOG4CXX_ERROR(logger, "Cannot find symbol: " << lt_dlerror());
	return p;
}
