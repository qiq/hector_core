/**
TestMulti.la, multi, native
Process resources using ProcessMulti()

Dependencies: none

Parameters:
items		r/o	Total items processed
foo		r/w	Test string
timeTick	r/w	Max time to spend in ProcessMulti()
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
	Module::Type GetType();
	int ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);

private:
	int items;		// ObjectLock
	char *foo;		// ObjectLock
	int timeTick;		// ObjectLock

	char *GetItems(const char *name);
	char *GetFoo(const char *name);
	void SetFoo(const char *name, const char *value);
	char *GetTimeTick(const char *name);
	void SetTimeTick(const char *name, const char *value);

	ObjectValues<TestMulti> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();

	std::queue<TestResource*> resources;
};

inline Module::Type TestMulti::GetType() {
	return MULTI;
}

inline char *TestMulti::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool TestMulti::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *TestMulti::ListNamesSync() {
	return values->ListNames();
}

#endif
