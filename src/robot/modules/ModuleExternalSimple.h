/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_EXTERNAL_SIMPLE_H_
#define _MODULE_EXTERNAL_SIMPLE_H_

#include <log4cxx/logger.h>
#include <ext/hash_set>
#include "ModuleSimple.h"
#include "ExternalProcess.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class ModuleExternalSimple : public ModuleSimple {
	const char *path;
	const char **args;

	pthread_key_t buffer_key;

	static log4cxx::LoggerPtr logger;
public:
	ModuleExternalSimple();
	~ModuleExternalSimple();
	bool Init(Config *config, const char *name);
	void Process(Resource *resource);
};

#endif
