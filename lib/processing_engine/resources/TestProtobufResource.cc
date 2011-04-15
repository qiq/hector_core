
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

#ifndef WRAPPER

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));
TestProtobufResourceInfo TestProtobufResource::resourceInfo;

TestProtobufResourceInfo::TestProtobufResourceInfo() {
	SetTypeId(2);
	SetTypeString("TestProtobufResource");
	SetTypeStringTerse("TPR");
	SetObjectName("TestProtobufResource");

	vector<ResourceAttrInfo*> *l = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestProtobufResource> *ai;

	ai = new ResourceAttrInfoT<TestProtobufResource>(GetTypeId());
	ai->InitInt("id", &TestProtobufResource::GetId, &TestProtobufResource::SetId);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(GetTypeId());
	ai->InitInt("status", &TestProtobufResource::GetStatus, &TestProtobufResource::SetStatus);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(GetTypeId());
	ai->InitString("str", &TestProtobufResource::GetStr, &TestProtobufResource::SetStr);
	l->push_back(ai);

	SetAttrInfoList(l);
}

Resource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

void TestProtobufResource::Clear() {
	Resource::Clear();
	r.Clear();
}

bool TestProtobufResource::Serialize(ResourceOutputStream &output) {
	output.WriteVarint32(r.ByteSize());
	r.SerializeWithCachedSizes(output.GetCodedOutputStream());
	return true;
}

bool TestProtobufResource::Deserialize(ResourceInputStream &input) {
	uint32_t size;
	if (!input.ReadVarint32(&size))
                return false;
	google::protobuf::io::CodedInputStream::Limit l = input.PushLimit(size);
	bool result = r.ParseFromCodedStream(input.GetCodedInputStream());
	input.PopLimit(l);
	return result;
}

string TestProtobufResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TPR %d %d] %s", GetId(), IsSetFlag(DELETED) ? -1 : GetStatus(), GetStr().c_str());
	return buf;
}

#else

extern "C" Resource* create() {
	return (Resource*)new TestProtobufResource();
}

#endif
