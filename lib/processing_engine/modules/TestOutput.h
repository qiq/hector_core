/**
 * Test module, does nothing.
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
	void ProcessOutput(Resource *resource);

private:
	int items;		// guarded by ObjectLock

	ObjectValues<TestOutput> *values;

	char *getItems(const char *name);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();
};

inline Module::Type TestOutput::getType() {
	return OUTPUT;
}

inline char *TestOutput::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestOutput::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline std::vector<std::string> *TestOutput::listNamesSync() {
	return values->listNamesSync();
}

#endif
