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
	Resource *ProcessInput(bool sleep);

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
	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();

	int typeId;		// to create TestResource
};

inline Module::Type TestInput::getType() {
	return INPUT;
}

inline char *TestInput::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestInput::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline bool TestInput::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *TestInput::listNamesSync() {
	return values->listNamesSync();
}

#endif
