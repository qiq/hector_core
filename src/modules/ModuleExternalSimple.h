/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_EXTERNAL_SIMPLE_H_
#define _MODULE_EXTERNAL_SIMPLE_H_

#include <ext/hash_set>
#include "ModuleSimple.h"
#include "ExternalProcess.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class ModuleExternalSimple : public Module {
	const char *path;
	const char **args;

	pthread_key_t buffer_key;
public:
	ModuleExternalSimple();
	~ModuleExternalSimple();
	bool Init(Config *config, const char *name);
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);
};

#endif
