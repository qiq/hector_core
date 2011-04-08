/**
TestInput.la, input, native
Create resources of given type.

Dependencies: none

Parameters:
items		r/o	Total items processed
maxItems	init	Number of resources to generate
idPrefix	r/w	Prefix to be used as prefix for string in TestResource
resourceType	init	What resource to genereate.
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_INPUT_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_INPUT_H_

#include <config.h>

#include "Module.h"
#include "ObjectProperties.h"

class TestInput : public Module {
public:
	TestInput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestInput();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessInputSync(bool sleep);

private:
	int items;		// ObjectLock
	int maxItems;		// initOnly
	char *idPrefix;		// ObjectLock
	char *resourceType;	// ObjectLock

	char *GetItems(const char *name);
	char *GetMaxItems(const char *name);
	void SetMaxItems(const char *name, const char *value);
	char *GetIdPrefix(const char *name);
	void SetIdPrefix(const char *name, const char *value);
	char *GetResourceType(const char *name);
	void SetResourceType(const char *name, const char *value);

	ObjectProperties<TestInput> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	int typeId;		// to create TestResource
};

inline Module::Type TestInput::GetType() {
	return INPUT;
}

inline char *TestInput::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool TestInput::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *TestInput::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
