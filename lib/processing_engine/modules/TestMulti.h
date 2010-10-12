/**
 * Test module, does nothing.
 */

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_MULTI_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_MULTI_H_

#include <config.h>

#include <queue>
#include <string>
#include <tr1/unordered_map>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"
#include "TestResource.h"

class TestMulti : public Module {
public:
	TestMulti(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestMulti();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	int ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources);
	int ProcessingResources();

private:
	int items;		// ObjectLock
	char *foo;		// ObjectLock
	queue<TestResource*> *resources; // private to Process* (+ProcessingResources)

	ObjectValues<TestMulti> *values;

	char *getItems(const char *name);
	char *getFoo(const char *name);
	void setFoo(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();
};

inline Module::Type TestMulti::getType() {
	return MULTI;
}

inline char *TestMulti::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestMulti::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline bool TestMulti::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *TestMulti::listNamesSync() {
	return values->listNamesSync();
}

#endif
