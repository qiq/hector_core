/**
 * Test module, does nothing.
 */

#ifndef _TEST_SIMPLE_H_
#define _TEST_SIMPLE_H_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include "common.h"
#include "Lock.h"
#include "Module.h"
#include "ObjectValues.h"

class TestSimple : public Module {
public:
	TestSimple(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestSimple();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *ProcessSimple(Resource *resource);

private:
	int items;
	char *foo;
	bool flipStatus;
	int setStatus;

	ObjectValues<TestSimple> *values;

	char *getItems(const char *name);
	char *getFoo(const char *name);
	void setFoo(const char *name, const char *value);
	char *getFlipStatus(const char *name);
	void setFlipStatus(const char *name, const char *value);
	char *getSetStatus(const char *name);
	void setSetStatus(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();
};

inline module_t TestSimple::getType() {
	return MODULE_SIMPLE;
}

inline char *TestSimple::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestSimple::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *TestSimple::listNamesSync() {
	return values->listNamesSync();
}

#endif
