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
#include "ObjectProperties.h"

class Dump : public Module {
public:
	Dump(ObjectRegistry *objects, const char *id, int threadIndex);
	~Dump();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessSimpleSync(Resource *resource);

private:
	ObjectProperties<Dump> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();
};

inline 	Module::Type Dump::GetType() {
	return SIMPLE;
}

inline char *Dump::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool Dump::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *Dump::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
