#ifndef _LIBRARY_LOADER_H_
#define _LIBRARY_LOADER_H_

#include <config.h>

#include <ext/hash_map>
#include <log4cxx/logger.h>
#include <ltdl.h>
#include "common.h"
#include "Lock.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class LibraryLoader {
	static Lock lock;
	static bool initialized;
	static stdext::hash_map<string, lt_dlhandle*, string_hash> handles;

	static log4cxx::LoggerPtr logger;
public:
	LibraryLoader();
	~LibraryLoader();

	static void *loadLibrary(const char *lib, const char *sym);
};

#endif
