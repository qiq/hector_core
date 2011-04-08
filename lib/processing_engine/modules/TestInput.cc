/**
 * Test module.
 */
#include <config.h>

#include "common.h"
#include "ProcessingEngine.h"
#include "TestInput.h"
#include "TestResource.h"
#include "TestProtobufResource.h"

using namespace std;

TestInput::TestInput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	idPrefix = NULL;
	resourceType = NULL;

	values = new ObjectValues<TestInput>(this);
	values->Add("items", &TestInput::GetItems);
	values->Add("maxItems", &TestInput::GetMaxItems, &TestInput::SetMaxItems, true);
	values->Add("idPrefix", &TestInput::GetIdPrefix, &TestInput::SetIdPrefix);
	values->Add("resourceType", &TestInput::GetResourceType, &TestInput::SetResourceType, true);
}

TestInput::~TestInput() {
	free(idPrefix);
	free(resourceType);

	delete values;
}

char *TestInput::GetItems(const char *name) {
	return int2str(items);
}

char *TestInput::GetMaxItems(const char *name) {
	return int2str(maxItems);
}

void TestInput::SetMaxItems(const char *name, const char *value) {
	maxItems = str2int(value);
}

char *TestInput::GetIdPrefix(const char *name) {
	return idPrefix ? strdup(idPrefix) : NULL;
}

void TestInput::SetIdPrefix(const char *name, const char *value) {
	free(idPrefix);
	idPrefix = strdup(value);
}

char *TestInput::GetResourceType(const char *name) {
	return resourceType ? strdup(resourceType) : NULL;
}

void TestInput::SetResourceType(const char *name, const char *value) {
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
	typeId = Resource::GetRegistry()->NameToId(resourceType);
	if (typeId < 0) {
		LOG_ERROR(this, "Resource " << resourceType << " not configured.");
		return false;
	}
	return true;
}

Resource *TestInput::ProcessInputSync(bool sleep) {
	if (maxItems && items >= maxItems)
		return NULL;
	Resource *r = Resource::GetRegistry()->AcquireResource(typeId);
	r->SetId(GetThreadIndex()*10000+items);
	if (TestResource::IsInstance(r)) {
		TestResource *tr = static_cast<TestResource*>(r);
		char s[1024];
		snprintf(s, sizeof(s), "%s%d-%d", idPrefix ? idPrefix : "", GetThreadIndex(), items);
		tr->SetStr(s);
		LOG_INFO_R(this, tr, "Creating TestResource (" << tr->GetStr() << ")");
	} else if (TestProtobufResource::IsInstance(r)) {
		TestProtobufResource *tr = static_cast<TestProtobufResource*>(r);
		char s[1024];
		snprintf(s, sizeof(s), "%s%d-%d", idPrefix ? idPrefix : "", GetThreadIndex(), items);
		tr->SetStr(s);
		LOG_INFO_R(this, tr, "Creating TestProtobufResource (" << tr->GetStr() << ")");
	} else {
		LOG_INFO_R(this, r, "Creating resource");
	}
	items++;
	return r;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestInput(objects, id, threadIndex);
}
