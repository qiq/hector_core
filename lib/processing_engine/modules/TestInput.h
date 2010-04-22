/**
 * Test module, does nothing.
 */

#ifndef _TEST_INPUT_H_
#define _TEST_INPUT_H_

#include <config.h>

#include "Module.h"
#include "ObjectValues.h"

class TestInput : public Module {
public:
	TestInput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestInput();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);

private:
	int typeId;		// to create TestResource

	int items;
	int maxItems;
	char *idPrefix;

	ObjectValues<TestInput> *values;

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getIdPrefix(const char *name);
	void setIdPrefix(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();
};

inline module_t TestInput::getType() {
	return MODULE_INPUT;
}

inline char *TestInput::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestInput::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *TestInput::listNamesSync() {
	return values->listNamesSync();
}

#endif
