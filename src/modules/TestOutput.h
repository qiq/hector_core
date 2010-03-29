/**
 * Test module, does nothing.
 */

#ifndef _TEST_OUTPUT_H_
#define _TEST_OUTPUT_H_

#include <config.h>

#include "Module.h"
#include "ObjectValues.h"

class TestOutput : public Module {
public:
	TestOutput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestOutput();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *ProcessSimple(Resource *resource);

private:
	int items;

	ObjectValues<TestOutput> *values;

	char *getItems(const char *name);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();
};

inline module_t TestOutput::getType() {
	return MODULE_OUTPUT;
}

inline char *TestOutput::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestOutput::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *TestOutput::listNamesSync() {
	return values->listNamesSync();
}

#endif
