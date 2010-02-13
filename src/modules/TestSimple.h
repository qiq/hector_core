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

class TestSimple : public Module {
public:
	TestSimple(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestSimple();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);

protected:
	int items;
	char *foo;

	std::tr1::unordered_map<string, char*(TestSimple::*)()> getters;
	std::tr1::unordered_map<string, void(TestSimple::*)(const char*)> setters;

	char *getItems();
	char *getFoo();
	void setFoo(const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();
};

inline module_t TestSimple::getType() {
	return MODULE_SIMPLE;
}

#endif
