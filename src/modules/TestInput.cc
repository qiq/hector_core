/**
 * Test module.
 */
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include "TestInput.h"
#include "TestResource.h"

TestInput::TestInput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	idPrefix = NULL;
}

TestInput::~TestInput() {
	free(idPrefix);
}

bool TestInput::Init(vector<pair<string, string> > *params) {
	for (vector<pair<string, string> >::iterator iter = params->begin(); iter != params->end(); ++iter) {
		if (iter->first == "items") {
			maxItems = atoi(iter->second.c_str());
			if (maxItems)
				MODULE_LOG_INFO(logger, "Going to produce " << maxItems << " resources.");
		} else if (iter->first == "id_prefix") {
			idPrefix = strdup(iter->second.c_str());
		}
	}
	return true;
}

Resource *TestInput::Process(Resource *resource) {
	if (maxItems && items >= maxItems)
		return NULL;
	assert(resource == NULL);
	TestResource *tr = new TestResource();
	char s[1024];
	snprintf(s, sizeof(s), "%s%d-%d", idPrefix ? idPrefix : "", getThreadIndex(), items++);
	tr->setStr(s);
	MODULE_LOG_INFO(logger, "Loading resource (" << tr->getStr() << ")");
	return tr;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestInput(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}

