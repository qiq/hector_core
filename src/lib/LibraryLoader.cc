/**
 * Common functions, macros, etc
 */

#include <config.h>

#include "LibraryLoader.h"

log4cxx::LoggerPtr LibraryLoader::logger(log4cxx::Logger::getLogger("lib.LibraryLoader"));

Lock LibraryLoader::lock;
bool LibraryLoader::initialized;
stdext::hash_map<string, lt_dlhandle*, string_hash> LibraryLoader::handles;

LibraryLoader ll;

LibraryLoader::LibraryLoader() {
	initialized = false;
}

LibraryLoader::~LibraryLoader() {
	lock.lock();
	for (stdext::hash_map<string, lt_dlhandle*, string_hash>::iterator iter = handles.begin(); iter != handles.end(); ++iter) {
		lt_dlclose(*iter->second);
		delete iter->second;
	}
	lt_dlexit();
	initialized = false;
	lock.unlock();
}

void *LibraryLoader::loadLibrary(const char *lib, const char *sym) {
	lock.lock();
	if (!initialized) {
		if (lt_dlinit() != 0) {
			lock.unlock();
			LOG4CXX_ERROR(logger, "Cannot initialize libtool: " << lt_dlerror());
			return NULL;
		}
		initialized = true;
	}
	stdext::hash_map<string, lt_dlhandle*, string_hash>::iterator iter = handles.find(lib);
	lt_dlhandle *handle;
	if (iter != handles.end()) {
		handle = iter->second;
	} else {
		handle = new lt_dlhandle;
		*handle = lt_dlopen(lib);
		if (*handle == NULL) {
			lock.unlock();
			delete handle;
			LOG4CXX_ERROR(logger, "Cannot load library: " << lt_dlerror());
			return NULL;
		}
		handles[lib] = handle;
	}
	void *p = lt_dlsym(*handle, sym);
	lock.unlock();
	if (!p)
		LOG4CXX_ERROR(logger, "Cannot find symbol: " << lt_dlerror());
	return p;
}
