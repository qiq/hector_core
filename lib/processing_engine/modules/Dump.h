/**
Dump.la, simple, native
Dumps Resource content into log. Useful for debugging purposes.

Dependencies: none
Parameters: none
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_DUMP_H_
#define _LIB_PROCESSING_ENGINE_MODULES_DUMP_H_

#include <config.h>

#include <string>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"

class Dump : public Module {
public:
	Dump(ObjectRegistry *objects, const char *id, int threadIndex);
	~Dump();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessSimpleSync(Resource *resource);

private:
	ObjectValues<Dump> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();
};

inline 	Module::Type Dump::GetType() {
	return SIMPLE;
}

inline char *Dump::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool Dump::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *Dump::ListNamesSync() {
	return values->ListNames();
}

#endif
