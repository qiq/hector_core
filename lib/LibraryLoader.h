#ifndef _LIBRARY_LOADER_H_
#define _LIBRARY_LOADER_H_

#include <config.h>

#include <ltdl.h>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "common.h"
#include "Lock.h"

using namespace std;

class LibraryLoader {
	static Lock lock;
	static bool initialized;
	static std::tr1::unordered_map<string, lt_dlhandle*> handles;

	static log4cxx::LoggerPtr logger;
public:
	LibraryLoader();
	~LibraryLoader();

	static void *loadLibrary(const char *lib, const char *sym);
};

#endif