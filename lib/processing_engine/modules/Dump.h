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
	Module::Type getType();
	Resource *ProcessSimple(Resource *resource);

private:
	ObjectValues<Dump> *values;
	char *GetValue(const char *name);
	bool SetValue(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListNames();
};

inline 	Module::Type Dump::getType() {
	return SIMPLE;
}

inline char *Dump::GetValue(const char *name) {
	return values->GetValue(name);
}

inline bool Dump::SetValue(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline bool Dump::IsInitOnly(const char *name) {
	return values->IsInitOnly(name);
}

inline std::vector<std::string> *Dump::ListNames() {
	return values->ListNames();
}

#endif
