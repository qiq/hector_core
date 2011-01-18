#ifndef _LIB_LIBRARY_LOADER_H_
#define _LIB_LIBRARY_LOADER_H_

#include <config.h>

#include <ltdl.h>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "common.h"
#include "PlainLock.h"

class LibraryLoader {
public:
	LibraryLoader();
	~LibraryLoader();

	static void *LoadLibrary(const char *lib, const char *sym);

private:
	static PlainLock lock;
	static bool initialized;
	static std::tr1::unordered_map<std::string, lt_dlhandle*> handles;

	static log4cxx::LoggerPtr logger;
};

#endif
