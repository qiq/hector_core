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
#include "ObjectValues.h"

class TestInput : public Module {
public:
	TestInput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestInput();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *ProcessInputSync(bool sleep);

private:
	int items;		// ObjectLock
	int maxItems;		// initOnly
	char *idPrefix;		// ObjectLock
	char *resourceType;	// ObjectLock

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getIdPrefix(const char *name);
	void setIdPrefix(const char *name, const char *value);
	char *getResourceType(const char *name);
	void setResourceType(const char *name, const char *value);

	ObjectValues<TestInput> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();

	int typeId;		// to create TestResource
};

inline Module::Type TestInput::getType() {
	return INPUT;
}

inline char *TestInput::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool TestInput::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *TestInput::ListNamesSync() {
	return values->ListNames();
}

#endif
