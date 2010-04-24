/**
 * Test module.
 */
#include <config.h>

#include <assert.h>
#include "common.h"
#include "Resources.h"
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
	if (!values->InitValues(params))
		return false;
	if (maxItems)
		LOG_INFO(logger, "Going to produce " << maxItems << " resources.");
	typeId = Resources::Name2Id("TestResource");
	if (typeId < 0) {
		LOG_ERROR(logger, "Cannot load TestResource library");
		return false;
	}
	return true;
}

Resource *TestInput::Process(Resource *resource) {
	ObjectLockRead();
	if (maxItems && items >= maxItems) {
		ObjectUnlock();
		return NULL;
	}
	ObjectUnlock();
	assert(resource == NULL);
	// we can use just new TestResource(), we use Resources::CreateResource() for demo purpose
	TestResource *tr = dynamic_cast<TestResource*>(Resources::CreateResource(typeId));
	tr->setId(getThreadIndex()*10000+items);
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
