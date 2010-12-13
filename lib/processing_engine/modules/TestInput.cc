/**
 * Test module.
 */
#include <config.h>

#include "common.h"
#include "ProcessingEngine.h"
#include "TestInput.h"
#include "TestResource.h"

using namespace std;

TestInput::TestInput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	idPrefix = NULL;
	resourceType = NULL;

	values = new ObjectValues<TestInput>(this);
	values->addGetter("items", &TestInput::getItems);
	values->addGetter("maxItems", &TestInput::getMaxItems);
	values->addSetter("maxItems", &TestInput::setMaxItems, true);
	values->addGetter("idPrefix", &TestInput::getIdPrefix);
	values->addSetter("idPrefix", &TestInput::setIdPrefix);
	values->addGetter("resourceType", &TestInput::getResourceType);
	values->addSetter("resourceType", &TestInput::setResourceType, true);
}

TestInput::~TestInput() {
	free(idPrefix);
	free(resourceType);

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

char *TestInput::getResourceType(const char *name) {
	return resourceType ? strdup(resourceType) : NULL;
}

void TestInput::setResourceType(const char *name, const char *value) {
	free(resourceType);
	resourceType = strdup(value);
}

bool TestInput::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!values->InitValues(params))
		return false;
	if (!resourceType)
		resourceType = strdup("TestResource");
	if (maxItems)
		LOG_INFO(this, "Going to produce " << maxItems << " resources of type " << resourceType << ".");
	typeId = Resource::NameToId(resourceType);
	if (typeId < 0) {
		LOG_ERROR(this, "Cannot load " << resourceType << " library");
		return false;
	}
	return true;
}

Resource *TestInput::ProcessInput(bool sleep) {
	ObjectLockRead();
	int i = items;
	ObjectUnlock();
	if (maxItems && i >= maxItems)
		return NULL;
	ObjectLockWrite();
	Resource *r = Resource::AcquireResource(typeId);
	r->setId(getThreadIndex()*10000+items);
	if (typeId == TestResource::typeId) {
		TestResource *tr = static_cast<TestResource*>(r);
		char s[1024];
		snprintf(s, sizeof(s), "%s%d-%d", idPrefix ? idPrefix : "", getThreadIndex(), items);
		tr->setStr(s);
		LOG_INFO_R(this, tr, "Creating TestResource (" << tr->getStr() << ")");
	} else {
		LOG_INFO_R(this, r, "Creating resource");
	}
	items++;
	ObjectUnlock();
	return r;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestInput(objects, id, threadIndex);
}
