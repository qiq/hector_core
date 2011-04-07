
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "TestResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("resources.TestResource"));

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
	output.WriteVarint32(strcopy.size());
	output.WriteString(strcopy);
	return true;
}

bool TestResource::Deserialize(ResourceInputStream &input) {
	uint32_t len;
	input.ReadVarint32(&len);
	str.clear();
	input.ReadString(&str, len);
	return true;
}

string TestResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TR %d %d] %s", id, IsSetFlag(DELETED) ? -1 : status, str.c_str());
	return buf;
}

vector<ResourceAttrInfo*> *TestResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestResource> *ai;

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitInt("id", &TestResource::GetId, &TestResource::SetId);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitInt("status", &TestResource::GetStatus, &TestResource::SetStatus);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitString("str", &TestResource::GetStr, &TestResource::SetStr);
	result->push_back(ai);

	return result;
}
