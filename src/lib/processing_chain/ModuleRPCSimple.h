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
private:
	ExternalProcess *process;

	int messageId;

	static log4cxx::LoggerPtr logger;
public:
	ModuleRPCSimple(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~ModuleRPCSimple();
	bool Init(vector<pair<string, string> > *args);
	module_t getType();
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleRPCSimple::getType() {
	return MODULE_SIMPLE;
}

#endif
