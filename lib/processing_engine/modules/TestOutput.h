/**
TestOutput.la, output, native
Just print out a message that resource arrived.

Dependencies: none

Parameters:
items		r/o	Total items processed
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_OUTPUT_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_OUTPUT_H_

#include <config.h>

#include "Module.h"
#include "ObjectValues.h"

class TestOutput : public Module {
public:
	TestOutput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestOutput();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *ProcessOutput(Resource *resource);

private:
	int items;		// ObjectLock

	char *getItems(const char *name);

	ObjectValues<TestOutput> *values;
	char *GetValue(const char *name);
	bool SetValue(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListNames();
};

inline Module::Type TestOutput::getType() {
	return OUTPUT;
}

inline char *TestOutput::GetValue(const char *name) {
	return values->GetValue(name);
}

inline bool TestOutput::SetValue(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline bool TestOutput::IsInitOnly(const char *name) {
	return values->IsInitOnly(name);
}

inline std::vector<std::string> *TestOutput::ListNames() {
	return values->ListNames();
}

#endif
