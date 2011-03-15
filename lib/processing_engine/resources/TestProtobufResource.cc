
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"

using namespace std;

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));

ProtobufResource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

void TestProtobufResource::Clear() {
	Resource::Clear();
	r.Clear();
}

string TestProtobufResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TPR %d %d] %s", GetId(), IsSetFlag(DELETED) ? -1 : GetStatus(), GetStr().c_str());
	return buf;
}

vector<ResourceAttrInfo*> *TestProtobufResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestProtobufResource> *ai;

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitInt("id", &TestProtobufResource::GetId, &TestProtobufResource::SetId);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitInt("status", &TestProtobufResource::GetStatus, &TestProtobufResource::SetStatus);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitString("str", &TestProtobufResource::GetStr, &TestProtobufResource::SetStr);
	result->push_back(ai);

	return result;
}
