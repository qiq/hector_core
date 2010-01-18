/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_RPC_SIMPLE_H_
#define _MODULE_RPC_SIMPLE_H_

#include "Module.h"
#include "ExternalProcess.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class ModuleRPCSimple : public Module {
	const char *path;
	const char **args;

	pthread_key_t buffer_key;
public:
	ModuleRPCSimple();
	~ModuleRPCSimple();
	bool Init(vector<pair<string, string> > *args);
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
