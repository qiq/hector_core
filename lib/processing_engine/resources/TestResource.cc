
#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "TestResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

#ifndef WRAPPER

log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("resources.TestResource"));
TestResourceInfo TestResource::resourceInfo;

TestResourceInfo::TestResourceInfo() {
	SetTypeId(1);
	SetTypeString("TestResource");
	SetTypeStringTerse("TR");
	SetObjectName("TestResource");

	vector<ResourceAttrInfo*> *l = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestResource> *ai;

	ai = new ResourceAttrInfoT<TestResource>(GetTypeId());
	ai->InitInt32("id", &TestResource::GetId, &TestResource::SetId);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(GetTypeId());
	ai->InitInt32("status", &TestResource::GetStatus, &TestResource::SetStatus);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(GetTypeId());
	ai->InitString("str", &TestResource::GetStr, &TestResource::SetStr);
	l->push_back(ai);

	SetAttrInfoList(l);
}

Resource *TestResource::Clone() {
	return new TestResource(*this);
}

void TestResource::Clear() {
	Resource::Clear();
	str.clear();
}

bool TestResource::Serialize(ResourceOutputStream &output) {
	string strcopy(str);
	size_t idx;
	while ((idx = strcopy.find('\n')) != string::npos) {
		strcopy.replace(idx, 1, " ");
	}
	output.WriteString(strcopy);
	return true;
}

bool TestResource::Deserialize(ResourceInputStream &input, bool headerOnly) {
	if (headerOnly)
		return true;
	str.clear();
	return input.ReadString(&str);
}

bool TestResource::Skip(ResourceInputStream &input) {
	string s;
	return input.ReadString(&s);
}

string TestResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TR %d %d] %s", id, IsSetFlag(DELETED) ? -1 : status, str.c_str());
	return buf;
}

#else

extern "C" Resource* hector_resource_create() {
	return (Resource*)new TestResource();
}

#endif
