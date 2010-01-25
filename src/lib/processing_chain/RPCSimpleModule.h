/**
 * Dummy module, does nothing.
 */

#ifndef _RPC_SIMPLE_MODULE_H_
#define _RPC_SIMPLE_MODULE_H_

#include "Module.h"
#include "RPC.h"

using namespace std;

class RPCSimpleModule : public Module {
private:
	RPC *rpc;

	static log4cxx::LoggerPtr logger;
public:
	RPCSimpleModule(ObjectRegistry *objects, const char *id);
	~RPCSimpleModule();
	bool Init(vector<pair<string, string> > *args);
	bool Init(RPC *rpc, vector<pair<string, string> > *args);
	module_t getType();
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t RPCSimpleModule::getType() {
	return MODULE_SIMPLE;
}

#endif
