/**
 * Test module.
 */
#include <config.h>

#include <assert.h>
#include "common.h"
#include "TestInput.h"
#include "TestResource.h"

TestInput::TestInput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	idPrefix = NULL;

	values = new ObjectValues<TestInput>(this);

	values->addGetter("items", &TestInput::getItems);
	values->addGetter("maxItems", &TestInput::getMaxItems);
	values->addSetter("maxItems", &TestInput::setMaxItems);
	values->addGetter("idPrefix", &TestInput::getIdPrefix);
	values->addSetter("idPrefix", &TestInput::setIdPrefix);
}

TestInput::~TestInput() {
	free(idPrefix);

	delete values;
}

char *TestInput::getItems(const char *name) {
	return int2str(items);
}

char *TestInput::getMaxItems(const char *name) {
	return int2str(maxItems);
}

void TestInput::setMaxItems(const char *name, const char *value) {
	maxItems = str2int(value);
}

char *TestInput::getIdPrefix(const char *name) {
	return idPrefix ? strdup(idPrefix) : NULL;
}

void TestInput::setIdPrefix(const char *name, const char *value) {
	free(idPrefix);
	idPrefix = strdup(value);
}

bool TestInput::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	if (maxItems)
		LOG_INFO(logger, "Going to produce " << maxItems << " resources.");
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
	LOG_INFO(logger, "Loading resource (" << tr->getStr() << ")");
	return tr;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestInput(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}

