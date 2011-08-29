/**
Null.la, input/simple/multi/output, native
Does nothing to the resource.

Dependencies: none

Parameters:
moduleType		init	output	Type of module (input, simple, multi or output)
items			r/o	n/a	Total items processed
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_NULL_H_
#define _LIB_PROCESSING_ENGINE_MODULES_NULL_H_

#include <config.h>

#include "Module.h"
#include "ObjectProperties.h"

class Null : public Module {
public:
	Null(ObjectRegistry *objects, const char *id, int threadIndex);
	~Null();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessInputSync(bool sleep);
	Resource *ProcessSimpleSync(Resource *resource);
	bool ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources, int *processingResources);
	Resource *ProcessOutputSync(Resource *resource);

private:
	Module::Type moduleType;
	int items;

	char *GetModuleType(const char *name);
	void SetModuleType(const char *name, const char *value);
	char *GetItems(const char *name);

	ObjectProperties<Null> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	int typeId;
};

inline Module::Type Null::GetType() {
	return moduleType;
}

inline char *Null::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool Null::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *Null::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
