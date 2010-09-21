/**
 * Test module, does nothing.
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
	Resource *Process(Resource *resource);

private:
	int typeId;		// to create TestResource

	int items;		// guarded by ObjectLock
	int maxItems;		// guarded by ObjectLock
	char *idPrefix;		// guarded by ObjectLock
	char *resourceType;	// guarded by ObjectLock

	ObjectValues<TestInput> *values;

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getIdPrefix(const char *name);
	void setIdPrefix(const char *name, const char *value);
	char *getResourceType(const char *name);
	void setResourceType(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();
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

inline std::vector<std::string> *TestInput::listNamesSync() {
	return values->listNamesSync();
}

#endif
